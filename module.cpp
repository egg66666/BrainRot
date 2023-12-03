#include "module.h"
#include "ntapi.h"
#include <algorithm>
#include <array>
bool _module::init_import(std::string name)
{
	name = util::string::str_to_lower(name);
	IMAGE_DOS_HEADER dos_header = util::pe::get_dos_header(this->base);
	if (dos_header.e_magic != IMAGE_DOS_SIGNATURE)
		return false;
	IMAGE_NT_HEADERS nt_headers = util::pe::get_nt_headers(this->base);
	if (nt_headers.Signature != IMAGE_NT_SIGNATURE)
		return false;
	IMAGE_DATA_DIRECTORY import_directory = nt_headers.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	if (import_directory.VirtualAddress == 0 || import_directory.Size == 0)
		return false;
	IMAGE_IMPORT_DESCRIPTOR import_descriptor = read_virtual_memory<IMAGE_IMPORT_DESCRIPTOR>((void*)((uintptr_t)this->base + import_directory.VirtualAddress));

	for (uint32_t idx = 0; import_descriptor.FirstThunk; import_descriptor = read_virtual_memory<IMAGE_IMPORT_DESCRIPTOR>((void*)((uintptr_t)this->base + import_directory.VirtualAddress + (idx++ * sizeof(IMAGE_IMPORT_DESCRIPTOR))))) {
		IMAGE_THUNK_DATA thunk_data_names = read_virtual_memory<IMAGE_THUNK_DATA>((void*)((uintptr_t)this->base + import_descriptor.OriginalFirstThunk));
		IMAGE_THUNK_DATA thunk_data_iat = read_virtual_memory<IMAGE_THUNK_DATA>((void*)((uintptr_t)this->base + import_descriptor.FirstThunk));
		for (uint32_t j = 1; thunk_data_names.u1.AddressOfData; j++) {
			IMAGE_IMPORT_BY_NAME import_by_name = *(IMAGE_IMPORT_BY_NAME*)read_virtual_memory<std::array<char, 256>>((void*)((uintptr_t)this->base + thunk_data_names.u1.AddressOfData)).data();
			std::string import_name = util::string::str_to_lower(util::string::demangle_import_name(import_by_name.Name));
			if (util::string::str_contains(name, import_name)) {
				_import* import_entry = new _import();
				import_entry->name = import_name;
				import_entry->address = (void*)((uintptr_t)this->base + thunk_data_iat.u1.Function);
				this->imports[import_name] = import_entry;
				return true;
			}
			thunk_data_names = read_virtual_memory<IMAGE_THUNK_DATA>((void*)((uintptr_t)this->base + import_descriptor.OriginalFirstThunk + (j * sizeof(IMAGE_THUNK_DATA))));
			thunk_data_iat = read_virtual_memory<IMAGE_THUNK_DATA>((void*)((uintptr_t)this->base + import_descriptor.FirstThunk + (j * sizeof(IMAGE_THUNK_DATA))));
		}
	}

	return false;
}

bool _module::init_export(std::string name)
{
	name = util::string::str_to_lower(name);
	IMAGE_EXPORT_DIRECTORY export_directory = util::pe::get_export_directory(this->base);
	if (export_directory.NumberOfFunctions == 0)
		return false;

	void* name_table = (void*)((uintptr_t)this->base + export_directory.AddressOfNames);
	void* function_table = (void*)((uintptr_t)this->base + export_directory.AddressOfFunctions);

	if (name_table == nullptr || function_table == nullptr)
		return false;
	
	uint32_t* name_table_entries = new uint32_t[export_directory.NumberOfNames];
	uint32_t* function_table_entries = new uint32_t[export_directory.NumberOfFunctions];
	
	read_virtual_memory_raw(globals::handle, name_table, name_table_entries, sizeof(uint32_t) * export_directory.NumberOfNames, nullptr);
	read_virtual_memory_raw(globals::handle, function_table, function_table_entries, sizeof(uint32_t) * export_directory.NumberOfFunctions, nullptr);

	for (uint32_t i = 0; i < export_directory.NumberOfNames; i++) {
		std::array<char, 64> export_name_array;
		read_virtual_memory_raw(globals::handle, (void*)((uintptr_t)this->base + name_table_entries[i]), export_name_array.data(), 64, nullptr);
		std::string export_name = util::string::str_to_lower(export_name_array.data());
		if (util::string::str_contains(export_name, name)) {
			_export* export_entry = new _export();
			export_entry->name = export_name;
			export_entry->address = (void*)((uintptr_t)this->base + function_table_entries[i]);
			this->exports[export_name] = export_entry;
			return true;
		}
	}

	return false;
}

bool _module::init_section(std::string name)
{
	name = util::string::str_to_lower(name);

	IMAGE_NT_HEADERS nt_headers = util::pe::get_nt_headers(this->base);

	for (uint32_t i = 0; i < nt_headers.FileHeader.NumberOfSections; i++) {
		IMAGE_SECTION_HEADER section_header = util::pe::get_section_header(this->base, i);
		std::string section_name = util::string::sanitize_string(util::string::str_to_lower(std::string(section_header.Name, section_header.Name + IMAGE_SIZEOF_SHORT_NAME)));
		if (util::string::str_contains(name, section_name)) {
			_section* section_entry = new _section();
			section_entry->name = section_name;
			section_entry->base = (void*)((uintptr_t)this->base + section_header.VirtualAddress);
			section_entry->size = section_header.Misc.VirtualSize;
			section_entry->RVA = (void*)section_header.VirtualAddress;
			this->sections[section_name] = section_entry;
			return true;
		}

	}

	return false;
}

bool _module::init_module()
{
	auto module_name_lower = util::string::str_to_lower(this->name);
	PEB_LDR_DATA ldr = util::pe::get_ldr_data();
	LIST_ENTRY* head = ldr.InMemoryOrderModuleList.Flink;
	LIST_ENTRY* next = head;

	do {
		LDR_DATA_TABLE_ENTRY current_entry = read_virtual_memory<LDR_DATA_TABLE_ENTRY>(CONTAINING_RECORD(next, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks));
		std::string current_module_name = util::string::str_to_lower(util::string::extract_name_from_path(util::string::unicode_string_to_string(current_entry.FullDllName)));
		if (util::string::str_contains(current_module_name, module_name_lower)) {
			IMAGE_NT_HEADERS nt_headers = util::pe::get_nt_headers(current_entry.DllBase);
			this->base = current_entry.DllBase;
			this->size = nt_headers.OptionalHeader.SizeOfImage;
			this->entry = (void*)nt_headers.OptionalHeader.AddressOfEntryPoint;
			this->name = current_module_name;
			return true;
		}
		next = current_entry.InMemoryOrderLinks.Flink;
	} while (head != next);

	return false;
}
