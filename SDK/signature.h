#pragma once
#include "../process.h"

class signature {
public:
	signature(_module* module, const char* sig) : _signature(sig), module(module) {}
	//WIP, NOT WORKIIIING
	void* find_signature();
	void* find_signature_from_file();
	void* address() {
		if (!_address) {
			_address = find_signature_from_file();
		}
		return _address;
	}
private:
	const char* _signature;
	_module* module;
	void* _address = 0;

	bool is_signature_here(const unsigned char* addr, size_t max_length);
};