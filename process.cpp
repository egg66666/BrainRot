#include "process.h"
#include "Util.h"
#include "ntapi.h"
#include <vector>
#include <algorithm>


uint32_t process::get_process_id_helper(std::string process_name) {

	unsigned long size = 1024;
	std::vector<unsigned char> buffer;
	process_name = util::string::str_to_lower(process_name);
	while (true) {
		if (!nt_query_system_information(SystemProcessInformation, buffer.data(), size, &size)) {
			buffer.resize(size);
		}
		else {
			break;
		}
	}
	SYSTEM_PROCESS_INFORMATION* process_info = reinterpret_cast<SYSTEM_PROCESS_INFORMATION*>(buffer.data());
	for (; ; process_info = reinterpret_cast<SYSTEM_PROCESS_INFORMATION*>(reinterpret_cast<BYTE*>(process_info) + process_info->NextEntryOffset)) {
		if (process_info->ImageName.Buffer && util::string::str_contains(process_name, util::string::wstring_to_string(util::string::str_to_lower(std::wstring(process_info->ImageName.Buffer, process_info->ImageName.Length / sizeof(wchar_t)))))) {
			return reinterpret_cast<uint32_t>(process_info->UniqueProcessId);
		}
		if (!process_info->NextEntryOffset) {
			break;
		}
	}
	return 0;
}

HANDLE process::hijack_handle(std::uint32_t process_id) {

	if (globals::handle) {
		return globals::handle;
	}

	constexpr ULONG PREFERRED_RIGHTS = PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION |
		PROCESS_QUERY_INFORMATION | PROCESS_SET_INFORMATION | PROCESS_CREATE_THREAD;

	struct HandlePriority {
		HANDLE handle;
		ULONG pid;

		ULONG rights;
		bool operator<(const HandlePriority& other) const {
			return rights > other.rights;
		}
	};


	std::vector<HandlePriority> suitableHandles;

	unsigned long size = sizeof(SYSTEM_HANDLE_INFORMATION);
	auto hInfo = reinterpret_cast<PSYSTEM_HANDLE_INFORMATION>(new BYTE[size]);
	ZeroMemory(hInfo, size);

	while (!nt_query_system_information((SYSTEM_INFORMATION_CLASS)16, hInfo, size, &size)) {
		delete[] hInfo;
		hInfo = reinterpret_cast<PSYSTEM_HANDLE_INFORMATION>(new BYTE[size]);
		ZeroMemory(hInfo, size);
	}

	for (uint32_t i = 0; i < hInfo->HandleCount; i++) {
		auto& handle = hInfo->Handles[i];
		if (!util::process::is_handle_valid(reinterpret_cast<HANDLE>(handle.Handle)) || handle.ObjectTypeNumber != 0x7 || handle.ProcessId == 4 || handle.GrantedAccess < PREFERRED_RIGHTS) {
			continue;
		}
		suitableHandles.push_back({ reinterpret_cast<HANDLE>(handle.Handle), handle.ProcessId, handle.GrantedAccess });
	}
	std::sort(suitableHandles.begin(), suitableHandles.end());

	delete[] hInfo;

	for (auto handle : suitableHandles) {
		CLIENT_ID cid = { .UniqueProcess = reinterpret_cast<HANDLE>(handle.pid), .UniqueThread = 0 };
		OBJECT_ATTRIBUTES oa = {};
		InitializeObjectAttributes(&oa, nullptr, 0, nullptr, nullptr);
		HANDLE hProcess = nullptr;
		auto ntret = open_process_raw(&hProcess, PROCESS_DUP_HANDLE, &oa, &cid);
		if (ntret != STATUS_SUCCESS) {
			if (util::process::is_handle_valid(hProcess))
			CloseHandle(hProcess);
			continue;
		}
		HANDLE dupHandle = nullptr;
		ntret = duplicate_object_raw(hProcess, handle.handle, GetCurrentProcess(), &dupHandle, handle.rights, 0, 0);
		if (ntret != STATUS_SUCCESS) {
			CloseHandle(hProcess);
			CloseHandle(dupHandle);
			continue;
		}
		auto handle_pid = GetProcessId(dupHandle);
		if (handle_pid != process_id) {
			CloseHandle(hProcess);
			CloseHandle(dupHandle);
			continue;
		}

		CloseHandle(hProcess);
		this->process_handle = dupHandle;
		globals::handle = dupHandle;
		return dupHandle;
	}
	return nullptr;
}

PEB_LDR_DATA process::get_ldr_data()
{
	PEB peb = get_peb();
	if (peb.Ldr == nullptr) return PEB_LDR_DATA();
	PEB_LDR_DATA ldr_data = read_virtual_memory<PEB_LDR_DATA>((void*)peb.Ldr);
	return ldr_data;
}

PEB process::get_peb()
{
	PROCESS_BASIC_INFORMATION pbi = get_process_basic_information();
	PEB* ppeb = pbi.PebBaseAddress;
	if (ppeb == nullptr) return PEB();
	PEB peb = read_virtual_memory<PEB>((void*)ppeb);
	return peb;
}

PROCESS_BASIC_INFORMATION process::get_process_basic_information()
{
	PROCESS_BASIC_INFORMATION pbi;
	ZeroMemory(&pbi, sizeof(PROCESS_BASIC_INFORMATION));
	bool success = nt_query_information_process(globals::handle, ProcessBasicInformation, &pbi, sizeof(PROCESS_BASIC_INFORMATION), nullptr) == STATUS_SUCCESS;
	if (!success) return PROCESS_BASIC_INFORMATION();
	return pbi;
}

IMAGE_DOS_HEADER process::get_dos_header(_module* module_)
{
	IMAGE_DOS_HEADER dos_header = read_virtual_memory<IMAGE_DOS_HEADER>(module_->base);
	if (dos_header.e_magic != IMAGE_DOS_SIGNATURE) return IMAGE_DOS_HEADER();
	return dos_header;
}

IMAGE_NT_HEADERS process::get_nt_headers(_module* module_)
{
	IMAGE_DOS_HEADER dos_header = get_dos_header(module_);
	if (dos_header.e_magic != IMAGE_DOS_SIGNATURE) return IMAGE_NT_HEADERS();
	IMAGE_NT_HEADERS nt_headers = read_virtual_memory<IMAGE_NT_HEADERS>((void*)((uintptr_t)module_->base + dos_header.e_lfanew));
	if (nt_headers.Signature != IMAGE_NT_SIGNATURE) return IMAGE_NT_HEADERS();
	return nt_headers;
}

IMAGE_SECTION_HEADER process::get_section_header(_module* module_, uint32_t index)
{
	IMAGE_DOS_HEADER dos_header = get_dos_header(module_);
	if (dos_header.e_magic != IMAGE_DOS_SIGNATURE) return IMAGE_SECTION_HEADER();
	IMAGE_NT_HEADERS nt_headers = get_nt_headers(module_);
	if (nt_headers.Signature != IMAGE_NT_SIGNATURE) return IMAGE_SECTION_HEADER();
	IMAGE_SECTION_HEADER section_header = read_virtual_memory<IMAGE_SECTION_HEADER>((void*)((uintptr_t)module_->base + dos_header.e_lfanew + sizeof(IMAGE_NT_HEADERS) + nt_headers.FileHeader.SizeOfOptionalHeader + (sizeof(IMAGE_SECTION_HEADER) * index)));
	if (section_header.VirtualAddress == 0) return IMAGE_SECTION_HEADER();
	return section_header;
}

IMAGE_EXPORT_DIRECTORY process::get_export_directory(_module* module_)
{
	IMAGE_NT_HEADERS nt_headers = get_nt_headers(module_);
	if (nt_headers.Signature != IMAGE_NT_SIGNATURE) return IMAGE_EXPORT_DIRECTORY();
	IMAGE_DATA_DIRECTORY export_directory = nt_headers.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
	if (export_directory.VirtualAddress == 0) return IMAGE_EXPORT_DIRECTORY();
	IMAGE_EXPORT_DIRECTORY export_dir = read_virtual_memory<IMAGE_EXPORT_DIRECTORY>((void*)((uintptr_t)module_->base + export_directory.VirtualAddress));
	if (export_dir.NumberOfFunctions == 0) return IMAGE_EXPORT_DIRECTORY();
	return export_dir;
}

