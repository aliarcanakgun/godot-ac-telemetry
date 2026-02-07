#pragma once

// ACStatusType will not be used bcz the telemetry will be recorded when only the game is not paused
// if you're going to create a live telemetry or something else, you need to edit _process() in ac_telemetry.cpp and fill_from_snapshot() in gd_telemetry_snapshot.cpp
enum ACStatusType {
    STATUS_OFF = 0,
    STATUS_REPLAY = 1,
    STATUS_LIVE = 2,
    STATUS_PAUSE = 3
};

enum ACSessionType {
    SESSION_UNKNOWN = -1,
    SESSION_PRACTICE = 0,
    SESSION_QUALIFY = 1,
    SESSION_RACE = 2,
    SESSION_HOTLAP = 3,
    SESSION_TIME_ATTACK = 4,
    SESSION_DRIFT = 5,
    SESSION_DRAG = 6
};

enum ACFlagType {
    FLAG_NONE = 0,
    FLAG_BLUE = 1,
    FLAG_YELLOW = 2,
    FLAG_BLACK = 3,
    FLAG_WHITE = 4,
    FLAG_CHECKERED = 5,
    FLAG_PENALTY = 6
};