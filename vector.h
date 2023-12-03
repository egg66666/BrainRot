#pragma once
#include <cmath>
#include <math.h>
#include <array>
#include <vector>
#include <cstdint>
#include <type_traits>
#define M_PI 3.14159265358979323846
#pragma pack(push, 1)
template <typename T = float>
class vector2 {
public:
	vector2() : x(T()), y(T()) {}
	vector2(T x, T y) : x(x), y(y) {}
	vector2(const vector2& v) = default;
	vector2(vector2&& v) noexcept = default;
	vector2& operator=(const vector2& v) = default;
	vector2& operator=(vector2&& v) noexcept = default;

	vector2 operator+(const vector2& v) const { return vector2(x + v.x, y + v.y); }
	vector2 operator-(const vector2& v) const { return vector2(x - v.x, y - v.y); }
	vector2 operator*(const vector2& v) const { return vector2(x * v.x, y * v.y); }
	vector2 operator/(const vector2& v) const { return v.x != 0 && v.y != 0 ? vector2(x / v.x, y / v.y) : vector2(); }
	vector2 operator+(T v) const { return vector2(x + v, y + v); }
	vector2 operator-(T v) const { return vector2(x - v, y - v); }
	vector2 operator*(T v) const { return vector2(x * v, y * v); }
	vector2 operator/(T v) const { return v != 0 ? vector2(x / v, y / v) : vector2(); }
	vector2& operator+=(const vector2& v) { x += v.x; y += v.y; return *this; }
	vector2& operator-=(const vector2& v) { x -= v.x; y -= v.y; return *this; }
	vector2& operator*=(const vector2& v) { x *= v.x; y *= v.y; return *this; }
	vector2& operator/=(const vector2& v) { if (v.x != 0 && v.y != 0) { x /= v.x; y /= v.y; } return *this; }
	vector2& operator+=(T v) { x += v; y += v; return *this; }
	vector2& operator-=(T v) { x -= v; y -= v; return *this; }
	vector2& operator*=(T v) { x *= v; y *= v; return *this; }
	vector2& operator/=(T v) { if (v != 0) { x /= v; y /= v; } return *this; }

	T length() const { return sqrt(x * x + y * y); }
	void normalize() {
		T len = length();
		if (len != 0) {
			x /= len;
			y /= len;
		}
	}

	void clamp(const vector2& min, const vector2& max) {
		if (x < min.x) x = min.x;
		if (y < min.y) y = min.y;
		if (x > max.x) x = max.x;
		if (y > max.y) y = max.y;
	}

	void clamp_angle(const vector2& min, const vector2& max) {
		x = wrap_angle(x, min.x, max.x);
		y = wrap_angle(y, min.y, max.y);
	}

	T to_angle() const { return atan2(y, x); }

	void to_radians() { x = x * static_cast<T>(M_PI / 180.0); y = y * static_cast<T>(M_PI / 180.0); }
	void to_degrees() { x = x * static_cast<T>(180.0 / M_PI); y = y * static_cast<T>(180.0 / M_PI); }

	T x, y;
private:
	T wrap_angle(T angle, T min_angle, T max_angle) {
		T range = max_angle - min_angle;
		while (angle < min_angle) angle += range;
		while (angle > max_angle) angle -= range;
		return angle;
	}
};
#pragma pack(pop)
#pragma pack(push, 1)
template <typename T = float>
class vector3 {
public:
	vector3() : x(T()), y(T()), z(T()) {}
	vector3(T x, T y, T z) : x(x), y(y), z(z) {}
	vector3(const vector3& v) = default;
	vector3(vector3&& v) noexcept = default;
	vector3& operator=(const vector3& v) = default;
	vector3& operator=(vector3&& v) noexcept = default;

	vector3 operator+(const vector3& v) const { return vector3(x + v.x, y + v.y, z + v.z); }
	vector3 operator-(const vector3& v) const { return vector3(x - v.x, y - v.y, z - v.z); }
	vector3 operator*(const vector3& v) const { return vector3(x * v.x, y * v.y, z * v.z); }
	vector3 operator/(const vector3& v) const { return v.x != 0 && v.y != 0 && v.z != 0 ? vector3(x / v.x, y / v.y, z / v.z) : vector3(); }
	vector3 operator+(T v) const { return vector3(x + v, y + v, z + v); }
	vector3 operator-(T v) const { return vector3(x - v, y - v, z - v); }
	vector3 operator*(T v) const { return vector3(x * v, y * v, z * v); }
	vector3 operator/(T v) const { return v != 0 ? vector3(x / v, y / v, z / v) : vector3(); }
	vector3& operator+=(const vector3& v) { x += v.x; y += v.y; z += v.z; return *this; }
	vector3& operator-=(const vector3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
	vector3& operator*=(const vector3& v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
	vector3& operator/=(const vector3& v) { if (v.x != 0 && v.y != 0 && v.z != 0) { x /= v.x; y /= v.y; z /= v.z; } return *this; }
	vector3& operator+=(T v) { x += v; y += v; z += v; return *this; }
	vector3& operator-=(T v) { x -= v; y -= v; z -= v; return *this; }
	vector3& operator*=(T v) { x *= v; y *= v; z *= v; return *this; }
	vector3& operator/=(T v) { if (v != 0) { x /= v; y /= v; z /= v; } return *this; }

	T length() const { return sqrt(x * x + y * y + z * z); }
	void normalize() {
		T len = length();
		if (len != 0) {
			x /= len;
			y /= len;
			z /= len;
		}
	}

	void clamp(const vector3& min, const vector3& max) {
		if (x < min.x) x = min.x;
		if (y < min.y) y = min.y;
		if (z < min.z) z = min.z;
		if (x > max.x) x = max.x;
		if (y > max.y) y = max.y;
		if (z > max.z) z = max.z;
	}

	void clamp_angle(const vector3& min, const vector3& max) {
		x = wrap_angle(x, min.x, max.x);
		y = wrap_angle(y, min.y, max.y);
		z = wrap_angle(z, min.z, max.z);
	}


	T to_angle() const { return atan2(y, x); }

	void to_radians() { x = x * static_cast<T>(M_PI / 180.0); y = y * static_cast<T>(M_PI / 180.0); z = z * static_cast<T>(M_PI / 180.0); }
	void to_degrees() { x = x * static_cast<T>(180.0 / M_PI); y = y * static_cast<T>(180.0 / M_PI); z = z * static_cast<T>(180.0 / M_PI); }

	T x, y, z;

private:
	T wrap_angle(T angle, T min_angle, T max_angle) {
		T range = max_angle - min_angle;
		while (angle < min_angle) angle += range;
		while (angle > max_angle) angle -= range;
		return angle;
	}
};
#pragma pack(pop)
#pragma pack(push, 1)
template <typename T = float>
class vector4 {
public:
	vector4() : x(T()), y(T()), z(T()), w(T()) {}
	vector4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
	vector4(const vector4& v) = default;
	vector4(vector4&& v) noexcept = default;
	vector4& operator=(const vector4& v) = default;
	vector4& operator=(vector4&& v) noexcept = default;

	vector4 operator+(const vector4& v) const { return vector4(x + v.x, y + v.y, z + v.z, w + v.w); }
	vector4 operator-(const vector4& v) const { return vector4(x - v.x, y - v.y, z - v.z, w - v.w); }
	vector4 operator*(const vector4& v) const { return vector4(x * v.x, y * v.y, z * v.z, w * v.w); }
	vector4 operator/(const vector4& v) const { return v.x != 0 && v.y != 0 && v.z != 0 && v.w != 0 ? vector4(x / v.x, y / v.y, z / v.z, w / v.w) : vector4(); }
	vector4 operator+(T v) const { return vector4(x + v, y + v, z + v, w + v); }
	vector4 operator-(T v) const { return vector4(x - v, y - v, z - v, w - v); }
	vector4 operator*(T v) const { return vector4(x * v, y * v, z * v, w * v); }
	vector4 operator/(T v) const { return v != 0 ? vector4(x / v, y / v, z / v, w / v) : vector4(); }
	vector4& operator+=(const vector4& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
	vector4& operator-=(const vector4& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
	vector4& operator*=(const vector4& v) { x *= v.x; y *= v.y; z *= v.z; w *= v.w; return *this; }
	vector4& operator/=(const vector4& v) { if (v.x != 0 && v.y != 0 && v.z != 0 && v.w != 0) { x /= v.x; y /= v.y; z /= v.z; w /= v.w; } return *this; }
	vector4& operator+=(T v) { x += v; y += v; z += v; w += v; return *this; }
	vector4& operator-=(T v) { x -= v; y -= v; z -= v; w -= v; return *this; }
	vector4& operator*=(T v) { x *= v; y *= v; z *= v; w *= v; return *this; }
	vector4& operator/=(T v) { if (v != 0) { x /= v; y /= v; z /= v; w /= v; } return *this; }

	T length() const { return sqrt(x * x + y * y + z * z + w * w); }
	void normalize() {
		T len = length();
		if (len != 0) {
			x /= len;
			y /= len;
			z /= len;
			w /= len;
		}
	}

	void clamp(const vector4& min, const vector4& max) {
		if (x < min.x) x = min.x;
		if (y < min.y) y = min.y;
		if (z < min.z) z = min.z;
		if (w < min.w) w = min.w;
		if (x > max.x) x = max.x;
		if (y > max.y) y = max.y;
		if (z > max.z) z = max.z;
		if (w > max.w) w = max.w;
	}

	void clamp_angle(const vector4& min, const vector4& max) {
		x = wrap_angle(x, min.x, max.x);
		y = wrap_angle(y, min.y, max.y);
		z = wrap_angle(z, min.z, max.z);
		w = wrap_angle(w, min.w, max.w);
	}


	T to_angle() const { return atan2(y, x); }

	void to_radians() { x = x * static_cast<T>(M_PI / 180.0); y = y * static_cast<T>(M_PI / 180.0); z = z * static_cast<T>(M_PI / 180.0); w = w * static_cast<T>(M_PI / 180.0); }
	void to_degrees() { x = x * static_cast<T>(180.0 / M_PI); y = y * static_cast<T>(180.0 / M_PI); z = z * static_cast<T>(180.0 / M_PI); w = w * static_cast<T>(180.0 / M_PI); }
	T x, y, z, w;
private:
	T wrap_angle(T angle, T min_angle, T max_angle) {
		T range = max_angle - min_angle;
		while (angle < min_angle) angle += range;
		while (angle > max_angle) angle -= range;
		return angle;
	}
};
#pragma pack(pop)