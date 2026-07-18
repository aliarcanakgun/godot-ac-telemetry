#pragma once

#include "helper.h"
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace godot;

String win_error_string(DWORD err) {
    char *msg = nullptr;

    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        err,
        0,
        (LPSTR)&msg,
        0,
        nullptr
    );

    String s = msg ? String(msg).strip_edges() : "Unknown error";
    if (msg) LocalFree(msg);
    return s;
}

String wchar_to_gdstring(const wchar_t* wstr, size_t len) {
    if (!wstr || len == 0) return String("");
    return String::utf16((const char16_t*)wstr, (int64_t)wcsnlen(wstr, len)).strip_edges();
}

void smooth_float_array(godot::PackedFloat32Array& arr, int window_size) {
    int n = arr.size();
    if (n < 2) return;

    std::vector<float> smoothed_result(n);
    const float* ptr = arr.ptr();
    
    int half_window = window_size / 2;
    for (int i = 0; i < n; ++i) {
        if (std::isnan(ptr[i])) {
            smoothed_result[i] = ptr[i];
            continue;
        }
        
        float sum = 0.0f;
        int count = 0;
        
        for (int j = std::max(0, i - half_window); j <= std::min(n - 1, i + half_window); ++j) {
            if (!std::isnan(ptr[j])) {
                sum += ptr[j];
                count++;
            }
        }
        
        smoothed_result[i] = count > 0 ? (sum / count) : ptr[i];
    }
    
    float* out_ptr = arr.ptrw();
    for (int i = 0; i < n; ++i) {
        out_ptr[i] = smoothed_result[i];
    }
}
