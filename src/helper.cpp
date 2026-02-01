#include "helper.h"

godot::String win_error_string(DWORD err) {
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

    godot::String s = msg ? godot::String(msg).strip_edges() : "Unknown error";
    if (msg) LocalFree(msg);
    return s;
}
