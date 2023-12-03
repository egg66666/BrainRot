#include "ntapi.h"

NtReadVirtualMemoryFunc ntReadVirtualMemory = (NtReadVirtualMemoryFunc)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtReadVirtualMemory");
NtWriteVirtualMemoryFunc ntWriteVirtualMemory = (NtWriteVirtualMemoryFunc)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtWriteVirtualMemory");
NtOpenProcessFunc ntOpenProcess = (NtOpenProcessFunc)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtOpenProcess");
NtQuerySystemInformationFunc ntQuerySystemInformation = (NtQuerySystemInformationFunc)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQuerySystemInformation");
NtQueryInformationProcessFunc ntQueryInformationProcess = (NtQueryInformationProcessFunc)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationProcess");
NtDuplicateObjectFunc ntDuplicateObject = (NtDuplicateObjectFunc)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtDuplicateObject");

HANDLE open_process(ACCESS_MASK desired_access, POBJECT_ATTRIBUTES object_attributes, CLIENT_ID* client_id)
{
	OBJECT_ATTRIBUTES oa = { 0 };
	InitializeObjectAttributes(&oa, NULL, 0, NULL, NULL);
	HANDLE handle;
	NTSTATUS status = ntOpenProcess(&handle, desired_access, &oa, client_id);
	if (NT_SUCCESS(status))
		return handle;
	return NULL;
}

bool nt_query_system_information(SYSTEM_INFORMATION_CLASS system_information_class, void* system_information, unsigned long system_information_length, unsigned long* return_length = nullptr)
{
	NTSTATUS status = ntQuerySystemInformation(system_information_class, system_information, system_information_length, return_length);
	if (NT_SUCCESS(status))
		return true;
	return false;
}

bool nt_query_information_process(HANDLE process_handle, PROCESSINFOCLASS process_information_class, void* process_information, unsigned long process_information_length, unsigned long* return_length)
{
	NTSTATUS status = ntQueryInformationProcess(process_handle, process_information_class, process_information, process_information_length, return_length);
	if (NT_SUCCESS(status))
		return true;
	return false;
}
