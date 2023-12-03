#pragma once
#include <Windows.h>
#include <winternl.h>
#include "globals.h"
#include <type_traits>
#define STATUS_SUCCESS 0x00000000

using NtReadVirtualMemoryFunc = NTSTATUS(NTAPI*)(HANDLE ProcessHandle, void* BaseAddress, void* Buffer, unsigned long NumberOfBytesToRead, unsigned long* NumberOfBytesRead);
using NtWriteVirtualMemoryFunc = NTSTATUS(NTAPI*)(HANDLE ProcessHandle, void* BaseAddress, void* Buffer, unsigned long NumberOfBytesToWrite, unsigned long* NumberOfBytesWritten);
using NtOpenProcessFunc = NTSTATUS(NTAPI*)(HANDLE* ProcessHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, CLIENT_ID* ClientId);
using NtQueryInformationProcessFunc = NTSTATUS(NTAPI*)(HANDLE ProcessHandle, PROCESSINFOCLASS ProcessInformationClass, void* ProcessInformation, unsigned long ProcessInformationLength, unsigned long* ReturnLength);
using NtQuerySystemInformationFunc = NTSTATUS(NTAPI*)(SYSTEM_INFORMATION_CLASS SystemInformationClass, void* SystemInformation, unsigned long SystemInformationLength, unsigned long* ReturnLength);
using NtDuplicateObjectFunc = NTSTATUS(NTAPI*)(HANDLE SourceProcessHandle, HANDLE SourceHandle, HANDLE TargetProcessHandle, HANDLE* TargetHandle, ACCESS_MASK DesiredAccess, BOOLEAN InheritHandle, unsigned long Options);
using NtQueryInformationThreadFunc = NTSTATUS(NTAPI*)(HANDLE ThreadHandle, THREADINFOCLASS ThreadInformationClass, void* ThreadInformation, unsigned long ThreadInformationLength, unsigned long* ReturnLength);
using NtAdjustPrivilegesTokenFunc = NTSTATUS(NTAPI*)(HANDLE TokenHandle, BOOLEAN DisableAllPrivileges, PTOKEN_PRIVILEGES NewState, unsigned long BufferLength, PTOKEN_PRIVILEGES PreviousState, unsigned long* ReturnLength);

extern NtReadVirtualMemoryFunc ntReadVirtualMemory;
extern NtWriteVirtualMemoryFunc ntWriteVirtualMemory;
extern NtOpenProcessFunc ntOpenProcess;
extern NtQueryInformationProcessFunc ntQueryInformationProcess;
extern NtQuerySystemInformationFunc ntQuerySystemInformation;
extern NtDuplicateObjectFunc ntDuplicateObject;
extern NtQueryInformationThreadFunc ntQueryInformationThread;
extern NtAdjustPrivilegesTokenFunc ntAdjustPrivilegesToken;

inline NTSTATUS read_virtual_memory_raw(HANDLE handle, void* address, void* buffer, unsigned long buffer_size, unsigned long* bytes_read) {
	return ntReadVirtualMemory(handle, address, buffer, buffer_size, bytes_read);
}

inline NTSTATUS write_virtual_memory_raw(HANDLE handle, void* address, void* buffer, unsigned long buffer_size, unsigned long* bytes_written) {
	return ntWriteVirtualMemory(handle, address, buffer, buffer_size, bytes_written);
}

template <typename T>
T read_virtual_memory(void* address, unsigned long buffer_size = sizeof(T), HANDLE handle = globals::handle) {
	T buffer;
	ZeroMemory(&buffer, buffer_size);
	unsigned long bytes_read;
	NTSTATUS status;
	if constexpr (std::is_array<T>::value) {

		status = read_virtual_memory_raw(handle, address, buffer.data(), buffer_size, &bytes_read);
	}
	else if constexpr (std::is_pointer<T>::value) {
		status = read_virtual_memory_raw(handle, address, buffer, buffer_size, &bytes_read);
	}
	else {
		status = read_virtual_memory_raw(handle, address, &buffer, buffer_size, &bytes_read);
	}
	if (status != STATUS_SUCCESS || bytes_read != buffer_size) {
		return T();
	}
	return buffer;
}

template <typename T>
bool write_virtual_memory(void* address, T buffer, unsigned long buffer_size = sizeof(T), HANDLE handle = globals::handle) {
	unsigned long bytes_written;
	NTSTATUS status = write_virtual_memory_raw(handle, address, &buffer, buffer_size, &bytes_written);
	if (status != STATUS_SUCCESS || bytes_written != buffer_size) {
		return false;
	}
	return true;
}

inline NTSTATUS open_process_raw(HANDLE* handle, ACCESS_MASK desired_access, POBJECT_ATTRIBUTES object_attributes, CLIENT_ID* client_id) {
	return ntOpenProcess(handle, desired_access, object_attributes, client_id);
}

extern HANDLE open_process(ACCESS_MASK desired_access, POBJECT_ATTRIBUTES object_attributes, CLIENT_ID* client_id);

inline NTSTATUS duplicate_object_raw(HANDLE source_process_handle, HANDLE source_handle, HANDLE target_process_handle, HANDLE* target_handle, ACCESS_MASK desired_access, BOOLEAN inherit_handle, unsigned long options) {
	return ntDuplicateObject(source_process_handle, source_handle, target_process_handle, target_handle, desired_access, inherit_handle, options);
}

extern bool nt_query_system_information(SYSTEM_INFORMATION_CLASS system_information_class, void* system_information, unsigned long system_information_length, unsigned long* return_length);
extern bool nt_query_information_process(HANDLE process_handle, PROCESSINFOCLASS process_information_class, void* process_information, unsigned long process_information_length, unsigned long* return_length);