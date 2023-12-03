#include <filesystem>
#include <string>
#include <locale>
#include <codecvt>
#include <algorithm>
#include <ranges>
#include <format>
#include <cmath>
#include <cctype>
#include <cwctype>
#include <math.h>
#include <windows.h>
#include "Util.h"
#include "globals.h"
#include "ntapi.h"
#include <DbgHelp.h>
#pragma comment(lib, "Dbghelp.lib")
namespace util {
	
	namespace string {



		std::wstring unicode_string_to_wstring(const UNICODE_STRING& unicode_string) {
			return std::wstring(unicode_string.Buffer, unicode_string.Length / sizeof(wchar_t));
		}

		std::wstring string_to_wstring(std::string& str) {
			int wlen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
			std::wstring wstr(wlen - 1, 0);
			MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], wlen);
			return wstr;
		}


		std::wstring string_to_wstring(std::string&& str) {
			return string_to_wstring(static_cast<std::string&>(str));
		}

		std::string wstring_to_string(std::wstring&& wstr) {
			return wstring_to_string(static_cast<std::wstring&>(wstr));
		}

		std::string ignore_leading_nullchars(char* string)
		{
			uint8_t amount = 0;

			for (const char* ptr = string; *ptr == '\0'; ++ptr) {
				amount++;
			}

			return std::string(string + amount);
		}

		std::string wstring_to_string(std::wstring& wstr) {
			int len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
			std::string str(len - 1, 0);
			WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], len, NULL, NULL);
			return str;
		}

		std::string str_to_lower(std::string& string) {
			std::ranges::transform(string, string.begin(), [](unsigned char c) { return std::tolower(c); });
			return string;
		}

		std::string str_to_lower(std::string_view string) {
			std::string result(string);
			std::ranges::transform(result, result.begin(), [](unsigned char c) { return std::tolower(c); });
			return result;
		}

		std::string demangle_import_name(const char* name)
		{
			char output[1024];
			std::uint32_t result = UnDecorateSymbolName(name, output, sizeof(output), UNDNAME_COMPLETE);
			if (result == 0) {

				return name;
			}
			return output;
		}

		std::wstring str_to_lower(std::wstring& wstring) {
			std::ranges::transform(wstring, wstring.begin(), [](wchar_t wc) { return std::towlower(wc); });
			return wstring;
		}

		std::wstring str_to_lower(std::wstring_view wstring) {
			std::wstring result(wstring);
			std::ranges::transform(result, result.begin(), [](wchar_t wc) { return std::towlower(wc); });
			return result;
		}

		std::string sanitize_string(std::string& string)
		{
			string.erase(std::remove(string.begin(), string.end(), '\0'), string.end());
			return string;
		}

		std::string sanitize_string(std::string&& string)
		{
			return sanitize_string(static_cast<std::string&>(string));
		}

		std::string sanitize_string(std::string_view string)
		{
			std::string result(string);
			result.erase(std::remove(result.begin(), result.end(), '\0'), result.end());
			return result;
		}

		std::wstring sanitize_string(std::wstring& string)
		{
			string.erase(std::remove(string.begin(), string.end(), L'\0'), string.end());
			return string;
		}

		std::wstring sanitize_string(std::wstring&& string)
		{
			return sanitize_string(static_cast<std::wstring&>(string));
		}

		std::wstring sanitize_string(std::wstring_view string)
		{
			std::wstring result(string);
			result.erase(std::remove(result.begin(), result.end(), L'\0'), result.end());

			return result;
		}

		std::string remove_null_chars_up_to(const std::string& input, size_t up_to) {
			std::string result;
			result.reserve(input.size());


			std::copy_if(input.begin(), input.begin() + min(input.size(), up_to),
						 std::back_inserter(result), [](char c) { return c != '\0'; });


			if (up_to < input.size()) {
				result.append(input.begin() + up_to, input.end());
			}

			return result;
		}

		std::wstring remove_null_chars_up_to(const std::wstring& input, size_t up_to) {
			std::wstring result;
			result.reserve(input.size());


			std::copy_if(input.begin(), input.begin() + min(input.size(), up_to),
						 std::back_inserter(result), [](wchar_t c) { return c != L'\0'; });

			if (up_to < input.size()) {
				result.append(input.begin() + up_to, input.end());
			}

			return result;
		}

		std::string unicode_string_to_string(const UNICODE_STRING& unicode_string) {
			if (unicode_string.Length == 0) {
				return std::string();
			}

			auto res = read_virtual_memory<std::array<wchar_t, 256>>(unicode_string.Buffer, unicode_string.Length);

			auto string = wstring_to_string(sanitize_string(std::wstring_view(res.data(), unicode_string.Length)));
			return string;


		}
		std::string extract_name_from_path(std::string& path) {
			size_t pos = path.find_last_of("\\");
			if (pos == std::string::npos) {
				return path;
			}
			std::string normalized_path = std::string(path);
			auto name = normalized_path.substr(pos + 1);
			return name;
		}

		std::string extract_name_from_path(std::string&& path) {
			return extract_name_from_path(static_cast<std::string&>(path));
		}

		std::string extract_name_from_path(std::wstring& path) {
			size_t pos = path.find_last_of(L'\\');
			if (pos == std::wstring::npos) {
				return wstring_to_string(path);
			}
			std::string normalized_path = wstring_to_string(std::wstring(path));
			auto name = normalized_path.substr(pos + 1);
			return name;
		}

		std::string extract_name_from_path(std::wstring&& path) {
			return extract_name_from_path(static_cast<std::wstring&>(path));
		}

		std::string extract_name_from_path(const std::wstring_view& path) {
			size_t pos = path.find_last_of(L'\\');
			if (pos == std::wstring_view::npos) {
				return wstring_to_string(std::wstring(path));
			}
			return wstring_to_string(std::wstring(path.substr(pos + 1)));
		}

		std::string extract_name_from_path(const std::string_view& path) {
			size_t pos = path.find_last_of('\\');
			if (pos == std::string_view::npos) {
				return std::string(path);
			}
			return std::string(path.substr(pos + 1));
		}
	}
	namespace pe {
		PEB_LDR_DATA get_ldr_data()
		{
			PEB peb = get_peb();
			if (peb.Ldr == nullptr) return PEB_LDR_DATA();
			PEB_LDR_DATA ldr_data = read_virtual_memory<PEB_LDR_DATA>((void*)peb.Ldr);
			return ldr_data;
		}

		PEB get_peb()
		{
			PROCESS_BASIC_INFORMATION pbi = get_process_basic_information();
			PEB* ppeb = pbi.PebBaseAddress;
			if (ppeb == nullptr) return PEB();
			PEB peb = read_virtual_memory<PEB>((void*)ppeb);
			return peb;
		}

		PROCESS_BASIC_INFORMATION get_process_basic_information()
		{
			PROCESS_BASIC_INFORMATION pbi;
			ZeroMemory(&pbi, sizeof(PROCESS_BASIC_INFORMATION));
			bool success = nt_query_information_process(globals::handle, ProcessBasicInformation, &pbi, sizeof(PROCESS_BASIC_INFORMATION), nullptr);
			if (!success) 
				return PROCESS_BASIC_INFORMATION();
			return pbi;
		}

		IMAGE_DOS_HEADER get_dos_header(void* module_base)
		{
			IMAGE_DOS_HEADER dos_header = read_virtual_memory<IMAGE_DOS_HEADER>(module_base);
			return dos_header;
		}

		IMAGE_NT_HEADERS get_nt_headers(void* module_base)
		{
			IMAGE_DOS_HEADER dos_header = get_dos_header(module_base);

			IMAGE_NT_HEADERS nt_headers = read_virtual_memory<IMAGE_NT_HEADERS>((void*)((uintptr_t)module_base + dos_header.e_lfanew));

			return nt_headers;
		}

		IMAGE_SECTION_HEADER get_section_header(void* module_base, uint32_t index)
		{
			IMAGE_DOS_HEADER dos_header = get_dos_header(module_base);

			IMAGE_NT_HEADERS nt_headers = get_nt_headers(module_base);

			IMAGE_SECTION_HEADER section_header = read_virtual_memory<IMAGE_SECTION_HEADER>((void*)((uintptr_t)module_base + dos_header.e_lfanew + 4 + sizeof(IMAGE_NT_HEADERS) + nt_headers.FileHeader.SizeOfOptionalHeader + (sizeof(IMAGE_SECTION_HEADER) * index)));
			if (section_header.VirtualAddress == 0) return IMAGE_SECTION_HEADER();
			return section_header;
		}

		IMAGE_EXPORT_DIRECTORY get_export_directory(void* module_base)
		{
			IMAGE_NT_HEADERS nt_headers = get_nt_headers(module_base);

			IMAGE_DATA_DIRECTORY export_directory = nt_headers.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
			if (export_directory.VirtualAddress == 0)
				return IMAGE_EXPORT_DIRECTORY();
			IMAGE_EXPORT_DIRECTORY export_dir = read_virtual_memory<IMAGE_EXPORT_DIRECTORY>((void*)((uintptr_t)module_base + export_directory.VirtualAddress));
			if (export_dir.NumberOfFunctions == 0) 
				return IMAGE_EXPORT_DIRECTORY();
			return export_dir;
		}
	}
}