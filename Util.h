#pragma once
#include <Windows.h>
#include <winternl.h>
#include <iostream>
#include <string>
#include <vector>
#include <string_view>

namespace util {

	namespace string {

		std::string unicode_string_to_string(const UNICODE_STRING& unicode_string);
		std::wstring string_to_wstring(std::string& string);
		std::wstring string_to_wstring(std::string&& string);
		std::string wstring_to_string(std::wstring& wstring);
		std::string wstring_to_string(std::wstring&& wstring);
		std::string ignore_leading_nullchars(char* string);
		std::string str_to_lower(std::string& string);
		std::string str_to_lower(std::string_view string);
		std::string demangle_import_name(const char* name);
		std::wstring str_to_lower(std::wstring& string);
		std::wstring str_to_lower(std::wstring_view string);
		std::string sanitize_string(std::string& string);
		std::string sanitize_string(std::string&& string);
		std::string sanitize_string(std::string_view string);
		std::wstring sanitize_string(std::wstring& string);
		std::wstring sanitize_string(std::wstring&& string);
		std::wstring sanitize_string(std::wstring_view string);
		std::string sanitize_path(std::string& path);
		std::string sanitize_path(std::string&& path);
		std::string extract_name_from_path(const std::string_view path);
		std::string extract_name_from_path(const std::wstring_view path);
		std::string extract_name_from_path(std::wstring&& path);
		std::string extract_name_from_path(std::string&& path);
		std::string extract_name_from_path(std::string& path);
		std::string extract_name_from_path(std::wstring& path);
		constexpr bool str_contains(std::string_view string, const std::string_view substring)
		{
			return string.find(substring) != std::string_view::npos;
		}

		constexpr bool str_contains(std::wstring_view string, const std::wstring_view substring)
		{
			return string.find(substring) != std::wstring_view::npos;

		}
	}
	namespace process {


		constexpr bool is_handle_valid(HANDLE handle)
		{
			if (handle && handle != INVALID_HANDLE_VALUE)
			{
				return true;
			}

			return false;
		}
	}
	namespace pe {

		extern PEB_LDR_DATA get_ldr_data();
		extern PEB get_peb();
		extern PROCESS_BASIC_INFORMATION get_process_basic_information();
		extern IMAGE_DOS_HEADER get_dos_header(void* module_base);
		extern IMAGE_NT_HEADERS get_nt_headers(void* module_base);
		extern IMAGE_SECTION_HEADER get_section_header(void* module_base, uint32_t index);
		extern IMAGE_EXPORT_DIRECTORY get_export_directory(void* module_base);
	}
}