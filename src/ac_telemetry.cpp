#include "ac_telemetry.h"
#include "helper.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/project_settings.hpp>
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
    // release the datas from ram immediately
    sessions_data.clear();
    sessions_data.shrink_to_fit();

    loaded_data.clear();
    loaded_data.shrink_to_fit();

    disconnect_from_ac();
}

void ACTelemetry::_bind_methods() {
    ClassDB::bind_method(D_METHOD("connect_to_ac"), &ACTelemetry::connect_to_ac);
    ClassDB::bind_method(D_METHOD("disconnect_from_ac"), &ACTelemetry::disconnect_from_ac);

    ClassDB::bind_method(D_METHOD("start_logging"), &ACTelemetry::start_logging);
    ClassDB::bind_method(D_METHOD("finish_logging", "output_file_path"), &ACTelemetry::finish_logging);

    ClassDB::bind_method(D_METHOD("load_session_data", "file_path"), &ACTelemetry::load_session_data);

    ClassDB::bind_method(D_METHOD("get_speed"), &ACTelemetry::get_speed);
    
    ClassDB::add_signal("ACTelemetry", MethodInfo("connection_lost"));

    // bind getter/setter methods first
    ClassDB::bind_method(D_METHOD("is_connected_to_ac"), &ACTelemetry::is_connected_to_ac);   
    ClassDB::bind_method(D_METHOD("is_currently_logging"), &ACTelemetry::is_currently_logging);

    ClassDB::bind_method(D_METHOD("get_sample_interval"), &ACTelemetry::get_sample_interval);
    ClassDB::bind_method(D_METHOD("set_sample_interval"), &ACTelemetry::set_sample_interval);

    ClassDB::bind_method(D_METHOD("get_save_file_signature"), &ACTelemetry::get_save_file_signature);
    ClassDB::bind_method(D_METHOD("set_save_file_signature"), &ACTelemetry::set_save_file_signature);

    // register properties to godot inspector
    // TODO: ADD DOCUMENTATION: "The interval to update telemetry. In seconds. Can't be changed while logging."
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "sample_interval"), "set_sample_interval", "get_sample_interval");
    // TODO: ADD DOCUMENTATION: "The signature string written at the beginning of the binary save file to identify it. Can't be changed while logging."
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "save_file_signature"), "set_save_file_signature", "get_save_file_signature");
    
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

String ACTelemetry::finish_logging(String output_file_path) {
    if (!is_connected) return String("AC is not connected.");
    if (!is_logging) return String("Telemetry is not working.");
    if (output_file_path.is_empty()) return String("Output file path is empty.");

    is_logging = false;

    // convert godot path (res:// or user://) to filesystem path if needed
    String os_path = output_file_path;
    if (os_path.begins_with("res://") || os_path.begins_with("user://")) {
        os_path = ProjectSettings::get_singleton()->globalize_path(os_path);
    }

    // keep CharString in local var so get_data() pointer stays valid
    CharString cs = os_path.utf8();
    std::string path(cs.get_data(), cs.length());

    std::ofstream outfile(path, std::ios::binary);
    if (!outfile.is_open()) {
        return String("Could not open file for writing: " + os_path);
    }

    // write the signature
    CharString utf8_signature = save_file_signature.utf8();
    outfile.write(utf8_signature.get_data(), utf8_signature.length());
    if (outfile.fail()) {
        outfile.close();
        return String("Write error while writing signature to '" + os_path + "'");
    }

    // write static map/data (or whatever you call it)
    if (!dataStatic) {
        outfile.close();
        return String("Static data pointer is null; cannot write SPageStatic to '" + os_path + "'");
    }
    outfile.write(reinterpret_cast<const char*>(dataStatic), sizeof(SPageStatic));
    if (outfile.fail()) {
        outfile.close();
        return String("Write error while writing static SPageStatic to '" + os_path + "'");
    }

    // write sample interval value
    outfile.write(reinterpret_cast<const char*>(&sample_interval), sizeof(double));
    if (outfile.fail()) {
        outfile.close();
        return String("Write error while writing sample_interval (double) to '" + os_path + "'");
    }

    // write total laps count
    uint64_t total_laps = sessions_data.size();
    outfile.write(reinterpret_cast<const char*>(&total_laps), sizeof(total_laps));
    if (outfile.fail()) {
        outfile.close();
        return String("Write error while writing total_laps (" + String::num_uint64(total_laps) + ") to '" + os_path + "'");
    }

    // write sessions (include lap index in messages)
    for (size_t idx = 0; idx < sessions_data.size(); ++idx) {
        const auto &lap = sessions_data[idx];
        uint64_t lap_size = static_cast<uint64_t>(lap.size());
        outfile.write(reinterpret_cast<const char*>(&lap_size), sizeof(lap_size));

        if (outfile.fail()) {
            outfile.close();
            return String("Write error while writing lap_size for lap " + String::num_uint64(idx) + " to '" + os_path + "'");
        }

        if (lap_size > 0) {
            outfile.write(reinterpret_cast<const char*>(lap.data()), lap_size * sizeof(TelemetrySnapshot));
            if (outfile.fail()) {
                outfile.close();
                return String("Write error while writing telemetry data for lap " + String::num_uint64(idx) + " (snapshots: " + String::num_uint64(lap_size) + ") to '" + os_path + "'");
            }
        }
    }

    outfile.close();
    sessions_data.clear(); // free ram after saving

    return os_path;
}

String ACTelemetry::load_session_data(String file_path) {
    if (file_path.is_empty()) return String("File path is empty.");

    // clear previous data
    loaded_data.clear();

    // convert godot path (res:// or user://) to filesystem path if needed
    String os_path = file_path;
    if (os_path.begins_with("res://") || os_path.begins_with("user://")) {
        os_path = ProjectSettings::get_singleton()->globalize_path(os_path);
    }

    // keep CharString in local var so get_data() pointer stays valid
    CharString cs = os_path.utf8();
    std::string path(cs.get_data(), cs.length());

    // open the file in binary mode
    std::ifstream infile(path, std::ios::binary);
    if (!infile.is_open()) {
        return String("Could not open file for reading: ") + os_path;
    }

    // read and check signature
    // we use a buffer with size + 1 for safety
    int sig_len = save_file_signature.utf8().length();
    std::vector<char> sig_buffer(sig_len);
    infile.read(sig_buffer.data(), sig_len);
    
    String read_sig = String::utf8(sig_buffer.data(), sig_len);
    if (read_sig != save_file_signature) {
        infile.close();
        return String("Invalid file signature. Expected: " + save_file_signature + " but got: " + read_sig);
    }

    // read static data
    infile.read(reinterpret_cast<char*>(&loaded_static), sizeof(SPageStatic));
    if (infile.fail()) {
        infile.close();
        return String("Failed to read static data");
    }

    // read sample interval
    infile.read(reinterpret_cast<char*>(&sample_interval), sizeof(double));
    if (infile.fail()) {
        infile.close();
        return String("Failed to read sample interval");
    }

    // read total laps count
    uint64_t total_laps = 0;
    infile.read(reinterpret_cast<char*>(&total_laps), sizeof(total_laps));
    if (infile.fail()) {
        infile.close();
        return String("Failed to read total laps count");
    }

    // read each lap data
    for (uint64_t i = 0; i < total_laps; ++i) {
        uint64_t lap_size = 0;
        infile.read(reinterpret_cast<char*>(&lap_size), sizeof(lap_size));
        
        if (infile.fail()) {
            infile.close();
            return String("Unexpected end of file while reading lap size");
        }

        if (lap_size > 0) {
            std::vector<TelemetrySnapshot> lap_data;
            lap_data.resize(lap_size);

            // read entire lap block into vector memory
            infile.read(reinterpret_cast<char*>(lap_data.data()), lap_size * sizeof(TelemetrySnapshot));
            
            if (infile.fail()) {
                infile.close();
                return String("Failed to read snapshot data for lap " + String::num_uint64(i));
            }

            loaded_data.push_back(lap_data);
        }
    }

    infile.close();
    return String("");
}


float ACTelemetry::get_speed() {
    if (dataPhysics) {
        return dataPhysics->speedKmh;
    }
    return 0.0f;
}