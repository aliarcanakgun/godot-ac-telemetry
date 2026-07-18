#pragma once
#include <godot_cpp/variant/string.hpp>

#include <godot_cpp/variant/packed_float32_array.hpp>

#ifdef _WIN32
#include <windows.h>
godot::String win_error_string(DWORD err);
godot::String wchar_to_gdstring(const wchar_t* wstr, size_t len);
#endif

void smooth_float_array(godot::PackedFloat32Array& arr, int window_size = 15);
