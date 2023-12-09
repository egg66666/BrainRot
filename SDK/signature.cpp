#include "signature.h"
#include "../ntapi.h"
#include "../globals.h"
#include <fstream>
#include <vector>
#include <array>
#define __cpp_lib_print
#include <print>
void* signature::find_signature()
{
	std::array<unsigned char, 0x2000> buffer;
	uintptr_t start_address = (uintptr_t)module->base + 0x1000;
	uintptr_t end_address = (uintptr_t)module->base + module->size;

	for (uintptr_t idx = start_address; idx < end_address; idx += 0x1000) {
		size_t read_size = (idx + 0x1000 < end_address) ? 0x2000 : (end_address - idx);
		read_virtual_memory_raw(globals::handle, (void*)idx, buffer.data(), read_size, nullptr);

		for (size_t i = 0; i < read_size; ++i) {
			if (is_signature_here(buffer.data() + i, read_size - i)) {
				
				auto relative_offset = *(uint32_t*)(buffer.data() + i + 3);
				uintptr_t instruction_address = idx - (uintptr_t)module->base + i + 7;

				this->_address = (void*)instruction_address;
				return (void*)instruction_address;
			}
		}
	}

	return nullptr;
}

void* signature::find_signature_from_file()
{
	
	std::string path = module->path;  

	std::ifstream file(path, std::ios::binary);
	if (!file) {
		return nullptr; 
	}

	std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(file), {});

	for (size_t i = 0; i < buffer.size(); ++i) {
		if (is_signature_here(&buffer[i], buffer.size() - i)) {
			for (uint32_t j = 0; j < 32; j++) {
				std::print("{:02X} ", buffer[i + j]);
			}
			auto relative_offset = *(uint32_t*)(buffer.data() + i + 3); 
			uintptr_t instruction_address = relative_offset + i + 7 + 0xC00; 
			
			this->_address = (void*)instruction_address;
			return this->_address;
		}
	}

	return nullptr;
}

bool signature::is_signature_here(const unsigned char* addr, size_t max_length)
{
	for (size_t i = 0, j = 0; j < strlen(_signature) && i < max_length; ++i, ++j) {
		if (_signature[j] == ' ') {
			i--; 
			continue;
		}

		if (_signature[j] == '?') {
			++j; 
			continue;
		}

		char hex_byte[3] = { _signature[j], _signature[j + 1], '\0' };
		unsigned char byte = static_cast<unsigned char>(std::strtoul(hex_byte, nullptr, 16));
		++j;

		if (addr[i] != byte) {
			return false;
		}
	}

	return true;
}