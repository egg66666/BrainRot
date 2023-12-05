#pragma once
#include "globals.h"
#include "module.h"
#include <cstdint>
#include <string_view>
#include <unordered_map>
#include <Windows.h>
#include <winternl.h>
struct OBJECT_BASIC_INFORMATION {
	ULONG Attributes;
	ULONG GrantedAccess;
	ULONG HandleCount;
	ULONG PointerCount;
	ULONG PagedPoolCharge;
	ULONG NonPagedPoolCharge;
	ULONG NameInfoSize;
	ULONG TypeInfoSize;
	ULONG SecurityDescriptorSize;
	LARGE_INTEGER CreationTime;
};

typedef struct _OBJECT_TYPE_INFORMATION {
	UNICODE_STRING TypeName;


	ULONG TotalNumberOfObjects;
	ULONG TotalNumberOfHandles;
	ULONG TotalPagedPoolUsage;
	ULONG TotalNonPagedPoolUsage;
	ULONG TotalNamePoolUsage;
	ULONG TotalHandleTableUsage;
	ULONG HighWaterNumberOfObjects;
	ULONG HighWaterNumberOfHandles;
	ULONG HighWaterPagedPoolUsage;
	ULONG HighWaterNonPagedPoolUsage;
	ULONG HighWaterNamePoolUsage;
	ULONG HighWaterHandleTableUsage;
	ULONG InvalidAttributes;
	GENERIC_MAPPING GenericMapping;
	ULONG ValidAccessMask;
	BOOLEAN SecurityRequired;
	BOOLEAN MaintainHandleCount;

	UCHAR TypeIndex;

	ULONG PoolType;
	ULONG DefaultPagedPoolCharge;
	ULONG DefaultNonPagedPoolCharge;


} OBJECT_TYPE_INFORMATION;

typedef struct _THREAD_BASIC_INFORMATION
{
	NTSTATUS ExitStatus;
	PTEB TebBaseAddress;
	CLIENT_ID ClientId;
	KAFFINITY AffinityMask;
	KPRIORITY Priority;
	KPRIORITY BasePriority;
} THREAD_BASIC_INFORMATION, * PTHREAD_BASIC_INFORMATION;

typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO
{
	ULONG ProcessId;
	BYTE ObjectTypeNumber;
	BYTE Flags;
	USHORT Handle;
	PVOID Object;
	ACCESS_MASK GrantedAccess;
} SYSTEM_HANDLE_TABLE_ENTRY_INFO, * PSYSTEM_HANDLE_TABLE_ENTRY_INFO;

typedef struct _SYSTEM_HANDLE_INFORMATION
{
	ULONG HandleCount;
	SYSTEM_HANDLE_TABLE_ENTRY_INFO Handles[ANYSIZE_ARRAY];
} SYSTEM_HANDLE_INFORMATION, * PSYSTEM_HANDLE_INFORMATION;


//class current_process : public process {
//	uint32_t process_id = GetCurrentProcessId();
//	std::unordered_map<std::string, _module*> modules;
//
//};

class process {
private:
	std::unordered_map<std::string, _module*> modules;
	std::uint32_t process_id = 0;
	HANDLE process_handle;
	std::string process_name;
	process() = default;
	process(const process&) = delete;
	process(process&&) = delete;
	process& operator=(const process&) = delete;
	process& operator=(process&&) = delete;
	std::uint32_t get_process_id_helper(std::string process_name);
	~process() {
		CloseHandle(this->process_handle);
	}

public:

	_module* operator[](std::string module_name) {
		if (this->modules.find(module_name) == this->modules.end()) {
			this->modules[module_name] = new _module(module_name);
		}
		return this->modules[module_name];
	}

	std::uint32_t get_process_id(std::string name) {
		if (!this->process_id) {
			return this->get_process_id_helper(name);
		}
		return this->process_id;
	}

	void init(std::string name) {
		this->process_id = this->get_process_id(name);
		this->process_name = name;
	}

	//this is a ref because it allows me to do cool shit like process["modulename"]
	static process& get() {
		static process instance;
		return instance;
	}

	PEB_LDR_DATA get_ldr_data();
	PEB get_peb();
	PROCESS_BASIC_INFORMATION get_process_basic_information();
	IMAGE_DOS_HEADER get_dos_header(_module* module_);
	IMAGE_NT_HEADERS get_nt_headers(_module* module_);
	IMAGE_SECTION_HEADER get_section_header(_module* module_, uint32_t index);
	IMAGE_EXPORT_DIRECTORY get_export_directory(_module* module_);
	HANDLE hijack_handle() {
		return this->hijack_handle(this->process_id);
	}
	HANDLE hijack_handle(std::uint32_t process_id);
};

