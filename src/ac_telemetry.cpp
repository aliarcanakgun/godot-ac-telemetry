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
    ClassDB::bind_method(D_METHOD("disconnect_from_ac"), &ACTelemetry::disconnect_from_ac);

    ClassDB::bind_method(D_METHOD("start_logging"), &ACTelemetry::start_logging);
    ClassDB::bind_method(D_METHOD("finish_logging"), &ACTelemetry::finish_logging);

    ClassDB::bind_method(D_METHOD("get_speed"), &ACTelemetry::get_speed);
    
    ClassDB::add_signal("ACTelemetry", MethodInfo("connection_lost"));

    // bind getter/setter methods first
    ClassDB::bind_method(D_METHOD("is_connected_to_ac"), &ACTelemetry::is_connected_to_ac);   
    ClassDB::bind_method(D_METHOD("is_currently_logging"), &ACTelemetry::is_currently_logging);
    ClassDB::bind_method(D_METHOD("get_sample_interval"), &ACTelemetry::get_sample_interval);
    ClassDB::bind_method(D_METHOD("set_sample_interval"), &ACTelemetry::set_sample_interval);

    // register properties to godot inspector
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "sample_interval", godot::PROPERTY_HINT_NONE, "The interval to update telemetry. In seconds. Can't be changed while logging."), "set_sample_interval", "get_sample_interval");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "save_file_signature", godot::PROPERTY_HINT_NONE, "The signature string written at the beginning of the binary save file to identify it. Can't be changed while logging."), "get_save_file_signature", "set_save_file_signature");
}

bool ACTelemetry::is_connected_to_ac() const { return is_connected; }

bool ACTelemetry::is_currently_logging() const { return is_logging; }

double ACTelemetry::get_sample_interval() const { return sample_interval; }
void ACTelemetry::set_sample_interval(double p_sample_interval) {
    if (p_sample_interval <= 0.0) return;
    if (!is_logging) sample_interval = p_sample_interval; // disable while logging
}

String ACTelemetry::get_save_file_signature() const { return save_file_signature; }
void ACTelemetry::set_save_file_signature(String p_signature) {
    if (p_signature.is_empty()) return;
    if (!is_logging) save_file_signature = p_signature; // disable while logging
}

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
        CloseHandle(hMapPhysics); hMapPhysics = nullptr;
        
        DWORD err = GetLastError();
        return vformat("Physics MapViewOfFile failed (%d): %s", int64_t(err), win_error_string(err));
    }


    // try to open graphic map
    hMapGraphic = OpenFileMappingA(FILE_MAP_READ, FALSE, "Local\\acpmf_graphics");
    if (hMapGraphic == NULL) {
        // clear physics
        UnmapViewOfFile(dataPhysics); dataPhysics = nullptr;
        CloseHandle(hMapPhysics); hMapPhysics = nullptr;
        
        DWORD err = GetLastError();
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


    // try to open static map
    hMapStatic = OpenFileMappingA(FILE_MAP_READ, FALSE, "Local\\acpmf_static");
    if (hMapStatic == NULL) {
        // clear physics
        UnmapViewOfFile(dataPhysics); dataPhysics = nullptr;
        CloseHandle(hMapPhysics); hMapPhysics = nullptr;
        
        // clear graphic
        UnmapViewOfFile(dataGraphic); dataGraphic = nullptr;
        CloseHandle(hMapGraphic); hMapGraphic = nullptr;
        
        DWORD err = GetLastError();
        return vformat("Static Map Error (%d): %s", int64_t(err), win_error_string(err));
    }
    
    dataStatic = (SPageStatic*)MapViewOfFile(hMapStatic, FILE_MAP_READ, 0, 0, sizeof(SPageStatic));
    if (dataStatic == nullptr) {
        // clear everything
        UnmapViewOfFile(dataPhysics); dataPhysics = nullptr;
        CloseHandle(hMapPhysics); hMapPhysics = nullptr;

        UnmapViewOfFile(dataGraphic); dataGraphic = nullptr;
        CloseHandle(hMapGraphic); hMapGraphic = nullptr;

        CloseHandle(hMapStatic); hMapStatic = nullptr;

        DWORD err = GetLastError();
        return vformat("Static MapViewOfFile failed (%d): %s", int64_t(err), win_error_string(err));
    }

    // finalize connection status
    is_connected = true;
    return String("");
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
        // check if game is not live
        if (dataGraphic->status != AC_LIVE) { return; }

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
    if (!is_connected) return String("AC is not connected");
    if (!is_logging) return String("Telemetry is not working");

    is_logging = false;

    std::ofstream outfile("last_session_data.bin", std::ios::binary);
    if (!outfile.is_open()) {
        return String("Failed open last session data");
    }

    // write the signature
    CharString utf8_signature = save_file_signature.utf8();
    outfile.write(utf8_signature.get_data(), utf8_signature.length());
    if (outfile.fail()) {
        outfile.close();
        return String("Write error while writing signature");
    }

    // write static map/data (or whatever you call it)
    outfile.write(reinterpret_cast<const char*>(dataStatic), sizeof(SPageStatic));
    if (outfile.fail()) {
        outfile.close();
        return String("Write error while writing static map");
    }

    // write sample interval value
    outfile.write(reinterpret_cast<const char*>(&sample_interval), sizeof(double));
    if (outfile.fail()) {
        outfile.close();
        return String("Write error while writing sample interval value");
    }

    // write total laps count
    uint64_t total_laps = sessions_data.size();
    outfile.write(reinterpret_cast<const char*>(&total_laps), sizeof(total_laps));
    if (outfile.fail()) {
        outfile.close();
        return String("Write error while writing total laps count");
    }

    // write sessions
    for (const auto& lap : sessions_data) {
        uint64_t lap_size = static_cast<uint64_t>(lap.size());
        outfile.write(reinterpret_cast<const char*>(&lap_size), sizeof(lap_size));

        if (lap_size > 0) {
            outfile.write(reinterpret_cast<const char*>(lap.data()), lap_size * sizeof(TelemetrySnapshot));
            if (outfile.fail()) {
                outfile.close();
                return String("Write error while writing telemetry data");
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