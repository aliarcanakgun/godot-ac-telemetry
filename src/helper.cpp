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

PackedFloat32Array calc_derivative(const std::vector<float>& values, const std::vector<int32_t>& time_ms, int smoothing_window, float multiplier) {
    PackedFloat32Array arr;
    if (values.empty() || time_ms.empty() || values.size() != time_ms.size()) return arr;
    arr.resize(values.size());
    float* ptr = arr.ptrw();
    
    // forward difference for the first point
    if (values.size() > 1) {
        float dt = (float)(time_ms[1] - time_ms[0]) / 1000.0f;
        ptr[0] = (dt > 0.0001f) ? ((values[1] - values[0]) / dt) * multiplier : 0.0f;
    } else {
        ptr[0] = 0.0f;
    }

    // central difference for the middle
    for (size_t i = 1; i < values.size() - 1; ++i) {
        float dt = (float)(time_ms[i + 1] - time_ms[i - 1]) / 1000.0f;
        if (dt > 0.0001f) {
            ptr[i] = ((values[i + 1] - values[i - 1]) / dt) * multiplier;
        } else {
            ptr[i] = 0.0f;
        }
    }

    // backward difference for the last point
    size_t last = values.size() - 1;
    if (last > 0) {
        float dt = (float)(time_ms[last] - time_ms[last - 1]) / 1000.0f;
        ptr[last] = (dt > 0.0001f) ? ((values[last] - values[last - 1]) / dt) * multiplier : 0.0f;
    } else if (last > 0) {
        ptr[last] = 0.0f;
    }
    
    smooth_float_array(arr, smoothing_window);
    return arr;
}
