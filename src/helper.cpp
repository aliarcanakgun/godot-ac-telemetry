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
    return String::utf16((const char16_t*)wstr, (int64_t)wcsnlen(wstr, len)).strip_edges();
}
