#include "ac_telemetry.h"
#include "helper.h"
#include <godot_cpp/core/class_db.hpp>
#include <fstream>
#include <cstdint>

using namespace godot;

ACTelemetry::ACTelemetry() {
    set_process(true);

    // initialize all pointers
    hMapPhysics = NULL;
    hMapGraphic = NULL;
    dataPhysics = nullptr;
    dataGraphic = nullptr;
    is_connected = false;
    is_logging = false;
}

ACTelemetry::~ACTelemetry() {
    // release the lap data from RAM immediately
    sessions_data.clear();
    sessions_data.shrink_to_fit();
    disconnect_from_ac();
}

void ACTelemetry::_bind_methods() {
    ClassDB::bind_method(D_METHOD("connect_to_ac"), &ACTelemetry::connect_to_ac);
    ClassDB::bind_method(D_METHOD("start_logging"), &ACTelemetry::start_logging);
    ClassDB::bind_method(D_METHOD("finish_logging"), &ACTelemetry::finish_logging);

    ClassDB::bind_method(D_METHOD("get_speed"), &ACTelemetry::get_speed);
    
    ClassDB::add_signal("ACTelemetry", MethodInfo("connection_lost"));

    // Bind Getter/Setter methods first
    ClassDB::bind_method(D_METHOD("is_connected_to_ac"), &ACTelemetry::is_connected_to_ac);   
    ClassDB::bind_method(D_METHOD("is_currently_logging"), &ACTelemetry::is_currently_logging);
    ClassDB::bind_method(D_METHOD("get_sample_interval"), &ACTelemetry::get_sample_interval);
    ClassDB::bind_method(D_METHOD("set_sample_interval"), &ACTelemetry::set_sample_interval);

    // Register Properties to Godot Inspector
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "sample_interval", godot::PROPERTY_HINT_NONE, "The interval to update telemetry. In seconds."), "set_sample_interval", "get_sample_interval");
    // ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_connected", godot::PROPERTY_HINT_NONE, "", godot::PROPERTY_USAGE_NONE), "", "is_connected_to_ac");
    // ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_logging", godot::PROPERTY_HINT_NONE, "", godot::PROPERTY_USAGE_NONE), "", "is_currently_logging");
}

bool ACTelemetry::is_connected_to_ac() const { return is_connected; }

bool ACTelemetry::is_currently_logging() const { return is_logging; }

double ACTelemetry::get_sample_interval() const { return sample_interval; }
void ACTelemetry::set_sample_interval(double p_sample_interval) { sample_interval = p_sample_interval; }

String ACTelemetry::connect_to_ac() {
    is_connected = false;

    // cleanup existing handles before retrying
    if (dataPhysics) { UnmapViewOfFile(dataPhysics); dataPhysics = nullptr; }
    if (dataGraphic) { UnmapViewOfFile(dataGraphic); dataGraphic = nullptr; }
    if (hMapPhysics) { CloseHandle(hMapPhysics); hMapPhysics = nullptr; }
    if (hMapGraphic) { CloseHandle(hMapGraphic); hMapGraphic = nullptr; }

    // try to open physics map
    hMapPhysics = OpenFileMappingA(FILE_MAP_READ, FALSE, "Local\\acpmf_physics");
    if (hMapPhysics == NULL) {
        DWORD err = GetLastError();
        return vformat("Physics Map Error (%d): %s", int64_t(err), win_error_string(err));
    }

    dataPhysics = (SPagePhysics*)MapViewOfFile(hMapPhysics, FILE_MAP_READ, 0, 0, sizeof(SPagePhysics));
    if (dataPhysics == nullptr) {
        CloseHandle(hMapPhysics); hMapPhysics = nullptr; // Belleğe eşleyemezsek handle'ı kapatmalıyız
        
        DWORD err = GetLastError();
        return vformat("Physics MapViewOfFile failed (%d): %s", int64_t(err), win_error_string(err));
    }

    // try to open graphic map
    hMapGraphic = OpenFileMappingA(FILE_MAP_READ, FALSE, "Local\\acpmf_graphics");
    if (hMapGraphic == NULL) {
        DWORD err = GetLastError();
        
        // clear the physics
        UnmapViewOfFile(dataPhysics); dataPhysics = nullptr;
        CloseHandle(hMapPhysics); hMapPhysics = nullptr;

        return vformat("Graphic Map Error (%d): %s", int64_t(err), win_error_string(err));
    }
    
    dataGraphic = (SPageGraphic*)MapViewOfFile(hMapGraphic, FILE_MAP_READ, 0, 0, sizeof(SPageGraphic));
    if (dataGraphic == nullptr) {
        // clear everything
        UnmapViewOfFile(dataPhysics); dataPhysics = nullptr;
        CloseHandle(hMapPhysics); hMapPhysics = nullptr;

        CloseHandle(hMapGraphic); hMapGraphic = nullptr;

        DWORD err = GetLastError();
        return vformat("Graphic MapViewOfFile failed (%d): %s", int64_t(err), win_error_string(err));
    }

    // finalize connection status
    is_connected = true;
    return "";
}

void ACTelemetry::disconnect_from_ac() {
    is_connected = false;
    if (dataPhysics) { UnmapViewOfFile(dataPhysics); dataPhysics = nullptr; }
    if (dataGraphic) { UnmapViewOfFile(dataGraphic); dataGraphic = nullptr; }
    if (hMapPhysics) { CloseHandle(hMapPhysics); hMapPhysics = nullptr; }
    if (hMapGraphic) { CloseHandle(hMapGraphic); hMapGraphic = nullptr; }
}

void ACTelemetry::start_logging() {
    sessions_data.clear();
    last_lap_count = 0;
    is_logging = true;
}

void ACTelemetry::_process(double delta) {
    // check if pointers are still valid
    if (!dataPhysics || !dataGraphic) {
        if (is_connected) {
            is_connected = false;
            emit_signal("connection_lost");
        }
        return;
    }

    // check for refresh time
    accum += delta;
    if (accum >= sample_interval) {
        session_time += sample_interval;
        accum -= sample_interval;
    } else {
        return;
    }
    
    // telemetry stuff
    if (is_logging) {
        // check for new lap in graphic page
        if (dataGraphic->completedLaps > last_lap_count || sessions_data.empty()) {
            sessions_data.push_back(std::vector<TelemetrySnapshot>());
            last_lap_count = dataGraphic->completedLaps;
            session_time = 0.0;
        }

        // create snapshot
        TelemetrySnapshot snapshot;
        snapshot.timestamp = session_time;
        snapshot.physics = *dataPhysics;
        snapshot.graphic = *dataGraphic;

        // store data from physics page into current lap
        sessions_data.back().push_back(snapshot);
    }
}

String ACTelemetry::finish_logging() {
    is_logging = false;

    std::ofstream outfile("last_session_data.bin", std::ios::binary);
    if (!outfile.is_open()) {
        return String("Failed open last session data");
    }

    for (const auto& lap : sessions_data) {
        uint64_t lap_size = static_cast<uint64_t>(lap.size());
        outfile.write(reinterpret_cast<const char*>(&lap_size), sizeof(lap_size));

        if (lap_size > 0) {
            outfile.write(reinterpret_cast<const char*>(lap.data()), lap_size * sizeof(TelemetrySnapshot));
            if (outfile.fail()) {
                outfile.close();
                return String("Write error last session data");
            }
        }
    }

    outfile.close();
    sessions_data.clear(); // free ram after saving

    return String("last_session_data.bin");
}

float ACTelemetry::get_speed() {
    if (dataPhysics) {
        return dataPhysics->speedKmh;
    }
    return 0.0f;
}