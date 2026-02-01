#pragma once
#include <godot_cpp/variant/string.hpp>

#ifdef _WIN32
#include <windows.h>
godot::String win_error_string(DWORD err);
#endif
