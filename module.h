#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <type_traits>
#include "Util.h"

struct _import {
	std::string name;
	void* address;
	uint16_t ordinal;
};

struct _export {
	std::string name;
	void* address;
	uint16_t ordinal;
};

struct _section {
	std::string name;
	void* base;
	uint32_t size;
	void* RVA;
};

class _module
{
private:
	std::unordered_map<std::string, _import*> imports;
	std::unordered_map<std::string, _export*> exports;
	std::unordered_map<std::string, _section*> sections;
	bool init_import(std::string name);
	bool init_export(std::string name);
	bool init_section(std::string name);
	bool init_module();
public:
	~_module() {
		for (auto& i : imports) {
			delete i.second;
		}
		for (auto& e : exports) {
			delete e.second;
		}
		for (auto& s : sections) {
			delete s.second;
		}
	}
	std::string name;
	std::string path;
	void* base;
	uintptr_t size;
	void* entry;

	_module() = default;

	_module(std::string name) {
		this->name = name;
		init_module();
	}

	_module&& operator=(std::string name) {
		this->name = name;
		init_module();
		return std::move(*this);
	}

	template<typename T = _export>
	T* get(std::string name) {
		
		name = util::string::str_to_lower(name);
		if constexpr (std::is_base_of<_export, T>::value) {
			if (exports.find(name) != exports.end()) {
				return exports[name];
			}
			else {
				if (init_export(name)) return exports[name];
				else return nullptr;
			}
		}
		else if constexpr (std::is_base_of<_import, T>::value) {
			if (imports.find(name) != imports.end()) {
				return (T*)imports[name];
			}
			else {
				if (init_import(name)) return imports[name];
				else return nullptr;
			}
		}
		else if constexpr (std::is_base_of<_section, T>::value) {
			if (sections.find(name) != sections.end()) {
				return (T*)sections[name];
			}
			else {
				if (init_section(name)) return sections[name];
				else return nullptr;
			}
		}
		return nullptr;
	}
};

