#pragma once
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <vector>
#include <array>

#ifdef _WIN32
#include <windows.h>
godot::String win_error_string(DWORD err);
godot::String wchar_to_gdstring(const wchar_t* wstr, size_t len);
#endif

void smooth_float_array(godot::PackedFloat32Array& arr, int window_size = 15);
godot::PackedFloat32Array calc_derivative(const std::vector<float>& values, const std::vector<int32_t>& time_ms, int smoothing_window, float multiplier = 1.0f);

// helper functions to build arrays fast
template<typename T>
inline godot::PackedFloat32Array to_float_array(const std::vector<T>& vec) {
    godot::PackedFloat32Array arr;
    arr.resize(vec.size());
    float* ptr = arr.ptrw();
    for (size_t i = 0; i < vec.size(); ++i) {
        ptr[i] = static_cast<float>(vec[i]);
    }
    return arr;
}

template<typename T>
inline godot::PackedInt32Array to_int_array(const std::vector<T>& vec) {
    godot::PackedInt32Array arr;
    arr.resize(vec.size());
    int32_t* ptr = arr.ptrw();
    for (size_t i = 0; i < vec.size(); ++i) {
        ptr[i] = static_cast<int32_t>(vec[i]);
    }
    return arr;
}

template<typename T>
inline godot::PackedInt32Array to_int_array_offset(const std::vector<T>& vec, int offset) {
    godot::PackedInt32Array arr;
    arr.resize(vec.size());
    int32_t* ptr = arr.ptrw();
    for (size_t i = 0; i < vec.size(); ++i) {
        ptr[i] = static_cast<int32_t>(vec[i]) + offset;
    }
    return arr;
}

template<size_t N>
inline godot::PackedStringArray to_string_array(const std::vector<std::array<wchar_t, N>>& vec) {
    godot::PackedStringArray arr;
    arr.resize(vec.size());
    godot::String* ptr = arr.ptrw();
    for (size_t i = 0; i < vec.size(); ++i) {
        ptr[i] = godot::String(vec[i].data());
    }
    return arr;
}

template<typename T>
inline godot::PackedFloat32Array to_float_array_deg(const std::vector<T>& vec) {
    godot::PackedFloat32Array arr;
    arr.resize(vec.size());
    float* ptr = arr.ptrw();
    for (size_t i = 0; i < vec.size(); ++i) {
        ptr[i] = static_cast<float>(vec[i]) * 57.29578f;
    }
    return arr;
}

template<typename T>
inline godot::PackedFloat32Array to_float_array_rpm(const std::vector<T>& vec) {
    godot::PackedFloat32Array arr;
    arr.resize(vec.size());
    float* ptr = arr.ptrw();
    for (size_t i = 0; i < vec.size(); ++i) {
        ptr[i] = static_cast<float>(vec[i]) * 9.549297f;
    }
    return arr;
}

template <typename T>
inline godot::PackedFloat32Array to_float_array_pct(const std::vector<T>& vec) {
    godot::PackedFloat32Array arr;
    arr.resize(vec.size());
    float* ptr = arr.ptrw();
    for (size_t i = 0; i < vec.size(); ++i) {
        ptr[i] = static_cast<float>(vec[i]) * 100.0f;
    }
    return arr;
}

template <typename T>
inline godot::PackedFloat32Array to_float_array_mm(const std::vector<T>& vec) {
    godot::PackedFloat32Array arr;
    arr.resize(vec.size());
    float* ptr = arr.ptrw();
    for (size_t i = 0; i < vec.size(); ++i) {
        ptr[i] = static_cast<float>(vec[i]) * 1000.0f;
    }
    return arr;
}
