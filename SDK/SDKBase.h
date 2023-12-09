#pragma once
#include <shared_mutex>
#include <memory>
#include <thread>
#include "../process.h"
#define MEMBER_ACCESSOR(data, type, name, offset) \
    type name() { \
        return *reinterpret_cast<type*>(data + offset); \
    }


class sdk_base
{
public:
	virtual bool update() = 0;
	virtual bool init() = 0;
	std::shared_mutex mutex;
	process& _process;
};