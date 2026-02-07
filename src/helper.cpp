#pragma once

#include "helper.h"
#include <string>

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
    String out = String((const char16_t*)wstr);
    return out.strip_edges();
}

// String wchar_to_gdstring(const wchar_t* wstr, size_t len) {
//     if (!wstr || len == 0) return String("");
//     int needed = WideCharToMultiByte(CP_UTF8, 0, wstr, (int)len, NULL, 0, NULL, NULL);
//     if (needed <= 0) return String("");
//     std::string out;
//     out.resize(needed);
//     WideCharToMultiByte(CP_UTF8, 0, wstr, (int)len, &out[0], needed, NULL, NULL);
//     // trim trailing nulls if any
//     size_t real_len = strnlen(out.c_str(), out.size());
//     return String::utf8(out.c_str(), (int)real_len);
// }
