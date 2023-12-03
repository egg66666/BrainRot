#pragma once
#include <array>
#include <cmath>
#include <cstdint>
#include <math.h>
#include <type_traits>
#include <vector>

#pragma pack(push, 1)
template<typename T = float, uint32_t X = 4, uint32_t Y = 4>
class matrix {
public:
    matrix() : data() {}
    matrix(const std::array<std::array<T, X>, Y>& data) : data(data) {}
    matrix(const matrix& other) : data(other.data) {}
    matrix(matrix&& other) noexcept : data(std::move(other.data)) {}

    matrix& operator=(const matrix& other) {
        if (this != &other) {
            data = other.data;
        }
        return *this;
    }

    matrix& operator=(matrix&& other) noexcept {
        if (this != &other) {
            data = std::move(other.data);
        }
        return *this;
    }

    ~matrix() {}

    auto operator[](const uint32_t index) const {
        return data[index];
    }

    std::array<std::array<T, X>, Y> data;
};
#pragma pack(pop)