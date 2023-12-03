#pragma once
#include <cmath>
#include <cstdint>
template <typename T, uint32_t X, uint32_t Y>
class matrix;
template <typename T>
class vector2;
template <typename T>
class vector3;
template <typename T>
class vector4;

enum class multiply_type : uint32_t {
	LEFT,
	RIGHT
};

template <multiply_type multiply = multiply_type::LEFT>
bool __fastcall world_to_screen(const vector3<float>& world_pos, vector2<float>& screen_pos,
								const matrix<float, 4, 4>& view_matrix, const vector2<float>& screen_size);