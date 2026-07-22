#include "ac_telemetry.h"
#include "telemetry_data_structs.h"
#include "gd_lap_telemetry.h"
#include "helper.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <fstream>
#include <cstdint>

using namespace godot;

ACTelemetry::ACTelemetry() {
    set_process(true);

    // initialize pointers
    hMapPhysics = NULL;
    hMapGraphic = NULL;
    hMapStatic = NULL;

    dataPhysics = nullptr;
    dataGraphic = nullptr;
    dataStatic = nullptr;

    is_connected = false;
    last_physics_packet_id = -1;
    is_logging = false;
    last_i_current_time = 0;
}

ACTelemetry::~ACTelemetry() {
    is_logging = false;
    if (logging_thread.joinable()) {
        logging_thread.join();
    }

    // release the datas
    sessions_data.clear();
    sessions_data.shrink_to_fit();

    loaded_session_data.clear();
    loaded_session_data.shrink_to_fit();

    disconnect_from_ac();
}

void ACTelemetry::_bind_methods() {
    BIND_ENUM_CONSTANT(STATUS_OFF);
    BIND_ENUM_CONSTANT(STATUS_REPLAY);
    BIND_ENUM_CONSTANT(STATUS_LIVE);
    BIND_ENUM_CONSTANT(STATUS_PAUSE);

    BIND_ENUM_CONSTANT(SESSION_UNKNOWN);
    BIND_ENUM_CONSTANT(SESSION_PRACTICE);
    BIND_ENUM_CONSTANT(SESSION_QUALIFY);
    BIND_ENUM_CONSTANT(SESSION_RACE);
    BIND_ENUM_CONSTANT(SESSION_HOTLAP);
    BIND_ENUM_CONSTANT(SESSION_TIME_ATTACK);
    BIND_ENUM_CONSTANT(SESSION_DRIFT);
    BIND_ENUM_CONSTANT(SESSION_DRAG);

    BIND_ENUM_CONSTANT(FLAG_NONE);
    BIND_ENUM_CONSTANT(FLAG_BLUE);
    BIND_ENUM_CONSTANT(FLAG_YELLOW);
    BIND_ENUM_CONSTANT(FLAG_BLACK);
    BIND_ENUM_CONSTANT(FLAG_WHITE);
    BIND_ENUM_CONSTANT(FLAG_CHECKERED);
    BIND_ENUM_CONSTANT(FLAG_PENALTY);

    ClassDB::bind_method(D_METHOD("connect_to_ac"), &ACTelemetry::connect_to_ac);
    ClassDB::bind_method(D_METHOD("disconnect_from_ac"), &ACTelemetry::disconnect_from_ac);

    ClassDB::bind_method(D_METHOD("start_logging", "output_file_path"), &ACTelemetry::start_logging);
    ClassDB::bind_method(D_METHOD("finish_logging", "output_file_path"), &ACTelemetry::finish_logging, DEFVAL(""));

    ClassDB::bind_method(D_METHOD("get_live_static_data"), &ACTelemetry::get_live_static_data);
    ClassDB::bind_method(D_METHOD("get_live_snapshot"), &ACTelemetry::get_live_snapshot);

    ClassDB::bind_method(D_METHOD("load_session_data", "file_path"), &ACTelemetry::load_session_data);
    ClassDB::bind_method(D_METHOD("get_session_metadata", "file_path"), &ACTelemetry::get_session_metadata);
    ClassDB::bind_method(D_METHOD("get_loaded_session_metadata"), &ACTelemetry::get_loaded_session_metadata);
    ClassDB::bind_method(D_METHOD("close_loaded_session"), &ACTelemetry::close_loaded_session);
    ClassDB::bind_method(D_METHOD("get_loaded_session_lap_count"), &ACTelemetry::get_loaded_session_lap_count);
    ClassDB::bind_method(D_METHOD("get_loaded_session_sample_interval"), &ACTelemetry::get_loaded_session_sample_interval);
    ClassDB::bind_method(D_METHOD("get_loaded_session_samples_per_meter"), &ACTelemetry::get_loaded_session_samples_per_meter);
    ClassDB::bind_method(D_METHOD("get_loaded_session_lap_data", "lap_index"), &ACTelemetry::get_loaded_session_lap_data);
    ClassDB::bind_method(D_METHOD("get_loaded_session_lap_stats", "lap_index"), &ACTelemetry::get_loaded_session_lap_stats);
    ClassDB::bind_method(D_METHOD("get_loaded_session_static_data"), &ACTelemetry::get_loaded_session_static_data);
    ClassDB::bind_method(D_METHOD("get_loaded_session_lap_fuel_consumption", "lap_index"), &ACTelemetry::get_loaded_session_lap_fuel_consumption);
    ClassDB::bind_method(D_METHOD("get_loaded_session_total_fuel_consumption"), &ACTelemetry::get_loaded_session_total_fuel_consumption);
    ClassDB::bind_method(D_METHOD("get_loaded_session_total_laps"), &ACTelemetry::get_loaded_session_total_laps);
    ClassDB::bind_method(D_METHOD("calculate_lap_time_delta", "target_file_path", "target_lap_index", "current_file_path", "current_lap_index", "reference_positions"), &ACTelemetry::calculate_lap_time_delta, DEFVAL(0), DEFVAL(""), DEFVAL(0), DEFVAL(PackedFloat32Array()));

    ClassDB::add_signal("ACTelemetry", MethodInfo("connection_lost"));

    // getters/setters
    ClassDB::bind_method(D_METHOD("is_connected_to_ac"), &ACTelemetry::is_connected_to_ac);   
    ClassDB::bind_method(D_METHOD("is_currently_logging"), &ACTelemetry::is_currently_logging);
    ClassDB::bind_method(D_METHOD("get_ac_status"), &ACTelemetry::get_ac_status);

    ClassDB::bind_method(D_METHOD("get_sample_interval"), &ACTelemetry::get_sample_interval);
    ClassDB::bind_method(D_METHOD("set_sample_interval"), &ACTelemetry::set_sample_interval);

    ClassDB::bind_method(D_METHOD("get_samples_per_meter"), &ACTelemetry::get_samples_per_meter);
    ClassDB::bind_method(D_METHOD("set_samples_per_meter"), &ACTelemetry::set_samples_per_meter);

    ClassDB::bind_method(D_METHOD("get_save_file_signature"), &ACTelemetry::get_save_file_signature);
    ClassDB::bind_method(D_METHOD("set_save_file_signature"), &ACTelemetry::set_save_file_signature);

    // properties
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "sample_interval"), "set_sample_interval", "get_sample_interval");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "samples_per_meter"), "set_samples_per_meter", "get_samples_per_meter");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "save_file_signature"), "set_save_file_signature", "get_save_file_signature");
    
}

bool ACTelemetry::is_connected_to_ac() const { return is_connected; }
bool ACTelemetry::is_currently_logging() const { return is_logging; }

int ACTelemetry::get_ac_status() {
    if (!is_connected || !dataGraphic) return 0; // AC_OFF
    return dataGraphic->status;
}

double ACTelemetry::get_sample_interval() const { return sample_interval; }
void ACTelemetry::set_sample_interval(double p_sample_interval) {
    if (p_sample_interval <= 0.0) return;
    if (!is_logging) sample_interval = p_sample_interval;
}

double ACTelemetry::get_samples_per_meter() const { return samples_per_meter; }
void ACTelemetry::set_samples_per_meter(double p_samples_per_meter) {
    if (p_samples_per_meter <= 0.0) return;
    if (!is_logging) samples_per_meter = p_samples_per_meter;
}

String ACTelemetry::get_save_file_signature() const { return save_file_signature; }
void ACTelemetry::set_save_file_signature(String p_signature) {
    if (p_signature.is_empty()) return;
    if (!is_logging) save_file_signature = p_signature;
}

String ACTelemetry::connect_to_ac() {
    is_connected = false;

    // cleanup existing handles
    if (dataPhysics) { UnmapViewOfFile(dataPhysics); dataPhysics = nullptr; }
    if (dataGraphic) { UnmapViewOfFile(dataGraphic); dataGraphic = nullptr; }
    if (hMapPhysics) { CloseHandle(hMapPhysics); hMapPhysics = nullptr; }
    if (hMapGraphic) { CloseHandle(hMapGraphic); hMapGraphic = nullptr; }


    // physics map
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


    // graphic map
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


    // static map
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

    is_connected = true;
    return String("");
}

void ACTelemetry::disconnect_from_ac() {
    finish_logging();

    is_connected = false;
    if (dataPhysics) { UnmapViewOfFile(dataPhysics); dataPhysics = nullptr; }
    if (dataGraphic) { UnmapViewOfFile(dataGraphic); dataGraphic = nullptr; }
    if (dataStatic) { UnmapViewOfFile(dataStatic); dataStatic = nullptr; }
    if (hMapPhysics) { CloseHandle(hMapPhysics); hMapPhysics = nullptr; }
    if (hMapGraphic) { CloseHandle(hMapGraphic); hMapGraphic = nullptr; }
    if (hMapStatic) { CloseHandle(hMapStatic); hMapStatic = nullptr; }
}

void ACTelemetry::_process(double delta) {
    if (game_disconnected.load()) {
        game_disconnected.store(false);
        if (is_connected) { // connection lost
            finish_logging(session_output_file_path);
            is_connected = false;
            emit_signal("connection_lost");
        }
        return;
    }

    if (!dataPhysics || !dataGraphic) {
        if (is_connected) { // connection lost
            is_connected = false;
            finish_logging(session_output_file_path);
            emit_signal("connection_lost");
        }
        return;
    }
}

String ACTelemetry::start_logging(String output_file_path) {
    if (output_file_path.is_empty()) return String("Output file path is empty.");
    if (is_logging) return String("Already logging.");
    session_output_file_path = output_file_path;

    sessions_data.clear();
    last_lap_count = 0;
    last_physics_packet_id = -1;
    last_i_current_time = 0;
    last_recorded_meter = -1.0;
    game_disconnected.store(false);
    is_logging = true;

    logging_thread = std::thread(&ACTelemetry::logging_loop, this);

    return String("");
}

String ACTelemetry::finish_logging(String output_file_path) {
    if (!is_connected) return String("AC is not connected.");
    if (!is_logging) return String("Telemetry is not working.");

    is_logging = false;
    if (logging_thread.joinable()) {
        logging_thread.join();
    }

    // remove empty/junk laps
    for (auto it = sessions_data.begin(); it != sessions_data.end(); ) {
        if (it->speedKmh.empty() || it->iCurrentTime.empty() || it->timestamp.size() < 10) {
            it = sessions_data.erase(it);
        } else {
            ++it;
        }
    }
    
    if (sessions_data.empty()) return String("No valid telemetry data was recorded.");

    // get output path
    String output = output_file_path;
    if (output.is_empty()) output = session_output_file_path;

    // convert godot path to fs path
    String os_path = output;
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

    // write samples per meter
    outfile.write(reinterpret_cast<const char*>(&samples_per_meter), sizeof(double));
    if (outfile.fail()) {
        outfile.close();
        return String("Write error while writing samples_per_meter (double) to '" + os_path + "'");
    }

    // write total laps count
    uint64_t total_laps = sessions_data.size();
    outfile.write(reinterpret_cast<const char*>(&total_laps), sizeof(total_laps));
    if (outfile.fail()) {
        outfile.close();
        return String("Write error while writing total_laps (" + String::num_int64(total_laps) + ") to '" + os_path + "'");
    }

    // allocate space for lap offsets (ToC)
    std::streampos offsets_pos = outfile.tellp();
    std::vector<uint64_t> lap_offsets(total_laps, 0);
    outfile.write(reinterpret_cast<const char*>(lap_offsets.data()), total_laps * sizeof(uint64_t));
    if (outfile.fail()) {
        outfile.close();
        return String("Write error while writing lap offsets placeholder to '" + os_path + "'");
    }

    // write sessions
    for (uint64_t idx = 0; idx < sessions_data.size(); ++idx) {
        lap_offsets[idx] = outfile.tellp();
        sessions_data[idx].write_to_stream(outfile);

        if (outfile.fail()) {
            outfile.close();
            return String("Write error while writing telemetry data for lap " + String::num_uint64(idx) + " to '" + os_path + "'");
        }
    }

    // rewrite real offsets
    outfile.seekp(offsets_pos);
    outfile.write(reinterpret_cast<const char*>(lap_offsets.data()), total_laps * sizeof(uint64_t));

    outfile.close();
    sessions_data.clear();

    return os_path;
}

String ACTelemetry::_open_session_file(const String& file_path, std::ifstream& infile, SPageStatic& out_static, double& out_sample_interval, double& out_samples_per_meter, uint64_t& out_lap_count, std::vector<uint64_t>& out_lap_offsets) {
    if (file_path.is_empty()) return "file path is empty";

    String os_path = file_path;
    if (os_path.begins_with("res://") || os_path.begins_with("user://")) {
        os_path = ProjectSettings::get_singleton()->globalize_path(os_path);
    }

    CharString cs = os_path.utf8();
    std::string path(cs.get_data(), cs.length());
    
    infile.open(path, std::ios::binary);
    if (!infile.is_open()) return "could not open file: " + os_path;

    // check signature
    int sig_len = save_file_signature.utf8().length();
    std::vector<char> sig_buffer(sig_len);
    infile.read(sig_buffer.data(), sig_len);
    
    String read_sig = String::utf8(sig_buffer.data(), sig_len);
    if (read_sig != save_file_signature) {
        infile.close();
        return "invalid signature";
    }

    // read static data
    if (infile.read(reinterpret_cast<char*>(&out_static), sizeof(SPageStatic)).fail()) {
        infile.close(); return "failed reading static data";
    }
    
    // read sample interval
    if (infile.read(reinterpret_cast<char*>(&out_sample_interval), sizeof(double)).fail()) {
        infile.close(); return "failed reading sample interval";
    }
    
    // read samples per meter
    if (infile.read(reinterpret_cast<char*>(&out_samples_per_meter), sizeof(double)).fail()) {
        infile.close(); return "failed reading samples per meter";
    }

    // read total laps count
    if (infile.read(reinterpret_cast<char*>(&out_lap_count), sizeof(uint64_t)).fail()) {
        infile.close(); return "failed reading lap count";
    }
    
    // read lap offsets
    out_lap_offsets.resize(out_lap_count);
    if (out_lap_count > 0) {
        if (infile.read(reinterpret_cast<char*>(out_lap_offsets.data()), out_lap_count * sizeof(uint64_t)).fail()) {
            infile.close(); return "failed reading lap offsets";
        }
    }

    return "";
}

String ACTelemetry::load_session_data(String file_path) {
    loaded_session_data.clear();
    loaded_session_lap_offsets.clear();

    std::ifstream infile;
    uint64_t count = 0;
    String err = _open_session_file(file_path, infile, loaded_session_static_data, loaded_session_sample_interval, loaded_session_samples_per_meter, count, loaded_session_lap_offsets);
    if (!err.is_empty()) return err;

    loaded_session_lap_count = count;

    for (int i = 0; i < loaded_session_lap_count; ++i) {
        LapDataChannels lap_data;
        infile.seekg(loaded_session_lap_offsets[i]);
        lap_data.read_from_stream(infile);
        if (infile.fail()) {
            return String("failed to read snapshot data for lap ") + String::num_int64(i);
        }
        
        if (lap_data.speedKmh.empty()) continue;
        
        // compatibility for old telemetry files (before commit 'e35eb69')
        if (!lap_data.normalizedCarPosition.empty()) {
            float base_offset = 0.0f;
            float prev_pos = lap_data.normalizedCarPosition[0];
            for (size_t j = 1; j < lap_data.normalizedCarPosition.size(); ++j) {
                float curr_pos = lap_data.normalizedCarPosition[j];
                if (curr_pos - prev_pos < -0.5f) {
                    base_offset += 1.0f;
                } else if (curr_pos - prev_pos > 0.5f) {
                    base_offset -= 1.0f;
                }
                prev_pos = curr_pos;
                lap_data.normalizedCarPosition[j] = curr_pos + base_offset;
            }
        }
        
        loaded_session_data.push_back(lap_data);
    }

    infile.close();
    return "";
}

Dictionary ACTelemetry::_calculate_session_metadata(const SPageStatic& stat, uint64_t count, const std::vector<LapDataChannels>& laps) {
    Dictionary meta;

    meta["track_name"] = wchar_to_gdstring(stat.track, 33);
    meta["track_config"] = wchar_to_gdstring(stat.trackConfiguration, 33);
    meta["car_name"] = wchar_to_gdstring(stat.carModel, 33);
    meta["total_laps"] = (int)count;
    meta["sector_count"] = stat.sectorCount;

    Array laps_arr;
    int best_lap_time = 0;
    Dictionary sector_positions_norm;
    Dictionary sector_positions_m;

    for (uint64_t i = 0; i < count; i++) {
        const LapDataChannels& lap = laps[i];

        if (lap.speedKmh.empty()) continue;

        Dictionary lap_stats;
        int lap_time = 0;
        Dictionary sector_times;
        float top_speed = 0.0f;
        int current_sec_idx = lap.currentSectorIndex.empty() ? 0 : lap.currentSectorIndex[0];

        for (size_t j = 0; j < lap.speedKmh.size(); j++) {
            if (!lap.currentSectorIndex.empty() && lap.currentSectorIndex[j] != current_sec_idx) {
                if (lap.lastSectorTime[j] > 0 && lap.lastSectorTime[j] <= lap.iCurrentTime[j] + 2000) {
                    sector_times[current_sec_idx] = lap.lastSectorTime[j];
                }
                
                if (!sector_positions_norm.has(current_sec_idx) && j < lap.normalizedCarPosition.size()) {
                    float pos = lap.normalizedCarPosition[j];
                    sector_positions_norm[current_sec_idx] = pos;
                    sector_positions_m[current_sec_idx] = pos * stat.trackSPlineLength;
                }
                
                current_sec_idx = lap.currentSectorIndex[j];
            }
            if (lap.speedKmh[j] > top_speed) {
                top_speed = lap.speedKmh[j];
            }
        }

        bool is_completed = false;
        if (lap.normalizedCarPosition.size() > 1) {
            float total_progression = 0.0f;
            for (size_t k = 1; k < lap.normalizedCarPosition.size(); k++) {
                float diff = lap.normalizedCarPosition[k] - lap.normalizedCarPosition[k-1];
                if (diff < -0.5f) diff += 1.0f;
                if (diff > 0.5f) diff -= 1.0f;
                
                if (diff > 0.0f && diff < 0.1f) {
                    total_progression += diff;
                }
            }
            if (total_progression > 0.90f) {
                is_completed = true;
            }
        }

        int next_lap_best_time = 0;
        // try getting exact time from next lap if exists
        if (i + 1 < count) {
            const LapDataChannels& next_lap = laps[i + 1];
            
            if (!next_lap.iBestTime.empty()) {
                next_lap_best_time = next_lap.iBestTime[0];
            }

            if (!next_lap.iLastTime.empty()) {
                lap_time = 0;
                for (int t : next_lap.iLastTime) {
                    if (t > lap_time) lap_time = t;
                }
            }
            if (lap_time == 0 && !lap.iCurrentTime.empty()) {
                lap_time = lap.iCurrentTime.back();
            }

            if (is_completed && !next_lap.currentSectorIndex.empty() && !next_lap.lastSectorTime.empty()) {
                for (size_t k = 0; k < next_lap.currentSectorIndex.size(); k++) {
                    if (next_lap.currentSectorIndex[k] == 0 && next_lap.lastSectorTime[k] > 0) {
                        sector_times[current_sec_idx] = next_lap.lastSectorTime[k];
                        break;
                    }
                }
            }
        } else {
            lap_time = lap.iCurrentTime.empty() ? 0 : lap.iCurrentTime.back();
            is_completed = false;
        }

        bool is_valid = is_completed;
        if (is_valid) {
            for (size_t k = 0; k < lap.numberOfTyresOut.size(); k++) {
                if (lap.numberOfTyresOut[k] >= 4 || lap.penaltyTime[k] > 0.0f || lap.flag[k] == AC_PENALTY_FLAG || lap.isInPitLane[k] == 1) {
                    is_valid = false;
                    break;
                }
            }
            // check if it's best. if it is, next lap's best time will be this lap's time
            if (is_valid && !lap.iBestTime.empty() && lap.iBestTime[0] > 0 && lap_time > 0 && lap_time < lap.iBestTime[0]) {
                if (next_lap_best_time > 0 && next_lap_best_time != lap_time) {
                    is_valid = false;
                }
            }
        }

        lap_stats["lap_time_ms"] = lap_time;
        lap_stats["sector_times_ms"] = sector_times;
        lap_stats["top_speed_kmh"] = top_speed;
        lap_stats["snapshot_count"] = (int)lap.speedKmh.size();
        lap_stats["is_completed"] = is_completed;
        lap_stats["is_valid"] = is_valid;

        laps_arr.push_back(lap_stats);

        if (is_valid && lap_time > 0 && (best_lap_time == 0 || lap_time < best_lap_time)) {
            best_lap_time = lap_time;
        }
    }

    Dictionary best_sectors_dict;
    for (int i = 0; i < laps_arr.size(); i++) {
        Dictionary lap_dict = laps_arr[i];
        if (!lap_dict["is_completed"]) continue;
        
        Dictionary sectors = lap_dict["sector_times_ms"];
        Array keys = sectors.keys();
        for (int k = 0; k < keys.size(); k++) {
            int sec_idx = keys[k];
            int time = sectors[sec_idx];
            if (time <= 0) continue;
            
            if (!best_sectors_dict.has(sec_idx) || time < (int)best_sectors_dict[sec_idx]) {
                best_sectors_dict[sec_idx] = time;
            }
        }
    }

    meta["best_lap_time"] = best_lap_time;
    meta["best_sectors"] = best_sectors_dict;
    meta["sector_positions_norm"] = sector_positions_norm;
    meta["sector_positions_m"] = sector_positions_m;
    meta["laps"] = laps_arr;

    return meta;
}

Dictionary ACTelemetry::get_session_metadata(String file_path) {
    Dictionary meta;
    std::ifstream infile;
    SPageStatic stat;
    double interval = 0, spm = 0;
    uint64_t count = 0;
    std::vector<uint64_t> offsets;

    String err = _open_session_file(file_path, infile, stat, interval, spm, count, offsets);
    if (!err.is_empty()) {
        meta["error"] = err;
        return meta;
    }

    std::vector<LapDataChannels> laps(count);
    for (uint64_t i = 0; i < count; i++) {
        infile.seekg(offsets[i]);
        laps[i].read_metadata_from_stream(infile);
    }

    infile.close();

    return _calculate_session_metadata(stat, count, laps);
}

Dictionary ACTelemetry::get_loaded_session_metadata() {
    if (loaded_session_lap_count < 0) {
        Dictionary meta;
        meta["error"] = "no session loaded";
        return meta;
    }
    return _calculate_session_metadata(loaded_session_static_data, loaded_session_lap_count, loaded_session_data);
}

Dictionary ACTelemetry::get_live_static_data() {
    if (!dataStatic) return Dictionary();
    return _static_to_dict(*dataStatic);
}

Ref<GDLapTelemetry> ACTelemetry::get_live_snapshot() {
    Ref<GDLapTelemetry> snapshot;
    snapshot.instantiate();

    if (!is_connected || !dataPhysics || !dataGraphic) {
        return snapshot;
    }

    if (!sessions_data.empty()) {
        std::lock_guard<std::mutex> lock(data_mutex);
        snapshot->fill_from_channels(sessions_data.back());
    }

    return snapshot;
}

Ref<GDLapTelemetry> ACTelemetry::get_loaded_session_lap_data(int lap_index) {
    if (lap_index < 0 || lap_index >= loaded_session_data.size()) {
        Ref<GDLapTelemetry> empty;
        empty.instantiate();
        return empty;
    }

    Ref<GDLapTelemetry> data;
    data.instantiate();
    data->fill_from_channels(loaded_session_data[lap_index]);

    return data;
}

Dictionary ACTelemetry::get_loaded_session_lap_stats(int lap_index) {
    Dictionary stats;
    if (lap_index < 0 || lap_index >= loaded_session_data.size()) return stats;

    const auto &lap = loaded_session_data[lap_index];
    if (lap.timestamp.empty()) return stats;

    int lap_time = 0;
    Dictionary sector_times;
    float top_speed = 0.0f;
    int current_sec_idx = lap.currentSectorIndex[0];

    for (size_t i = 0; i < lap.timestamp.size(); i++) {
        // detect sector change
        if (lap.currentSectorIndex[i] != current_sec_idx) {
            if (lap.lastSectorTime[i] > 0 && lap.lastSectorTime[i] <= lap.iCurrentTime[i] + 2000) {
                sector_times[current_sec_idx] = lap.lastSectorTime[i];
            }
            current_sec_idx = lap.currentSectorIndex[i];
        }

        if (lap.speedKmh[i] > top_speed) {
            top_speed = lap.speedKmh[i];
        }
    }

    bool is_completed = false;
    if (lap.normalizedCarPosition.size() > 1) {
        float total_progression = 0.0f;
        for (size_t k = 1; k < lap.normalizedCarPosition.size(); k++) {
            float diff = lap.normalizedCarPosition[k] - lap.normalizedCarPosition[k-1];
            if (diff < -0.5f) diff += 1.0f;
            if (diff > 0.5f) diff -= 1.0f;
            
            if (diff > 0.0f && diff < 0.1f) {
                total_progression += diff;
            }
        }
        if (total_progression > 0.90f) {
            is_completed = true;
        }
    }

    // try to get the exact lap_time and final sector time from the next lap
    if (lap_index + 1 < loaded_session_data.size() && !loaded_session_data[lap_index + 1].timestamp.empty()) {
        const auto &next_lap = loaded_session_data[lap_index + 1];
        
        lap_time = 0;
        for (int t : next_lap.iLastTime) {
            if (t > lap_time) lap_time = t;
        }
        if (lap_time == 0 && !lap.iCurrentTime.empty()) {
            lap_time = lap.iCurrentTime.back();
        }
        
        if (is_completed) {
            for (size_t k = 0; k < next_lap.currentSectorIndex.size(); k++) {
                if (next_lap.currentSectorIndex[k] == 0 && next_lap.lastSectorTime[k] > 0) {
                    sector_times[current_sec_idx] = next_lap.lastSectorTime[k];
                    break;
                }
            }
        }
    } else {
        lap_time = lap.iCurrentTime.empty() ? 0 : lap.iCurrentTime.back();
        is_completed = false;
    }

    stats["lap_time_ms"] = lap_time;
    stats["sector_times_ms"] = sector_times;
    stats["top_speed_kmh"] = top_speed;
    stats["snapshot_count"] = (int)lap.timestamp.size();
    stats["is_completed"] = is_completed;
    
    return stats;
}

Dictionary ACTelemetry::get_loaded_session_static_data() {
    return _static_to_dict(loaded_session_static_data);
}

int ACTelemetry::get_loaded_session_lap_count() {
    return loaded_session_lap_count;
}

double ACTelemetry::get_loaded_session_sample_interval() {
    return loaded_session_sample_interval;
}

double ACTelemetry::get_loaded_session_samples_per_meter() {
    return loaded_session_samples_per_meter;
}

double ACTelemetry::get_loaded_session_lap_fuel_consumption(int lap_index) {
    if (lap_index < 0 || lap_index >= loaded_session_data.size()) return 0.0;
    const auto &lap = loaded_session_data[lap_index];
    if (lap.fuel.empty()) return 0.0;

    double consumed = 0.0;
    float prev_fuel = lap.fuel[0];
    for (size_t i = 1; i < lap.fuel.size(); i++) {
        float current_fuel = lap.fuel[i];
        if (current_fuel < prev_fuel) {
            consumed += (prev_fuel - current_fuel);
        }
        prev_fuel = current_fuel;
    }
    return consumed;
}

double ACTelemetry::get_loaded_session_total_fuel_consumption() {
    double total = 0.0;
    for (int i = 0; i < loaded_session_data.size(); i++) {
        total += get_loaded_session_lap_fuel_consumption(i);
    }
    return total;
}

double ACTelemetry::get_loaded_session_total_laps() {
    double total_driven_fraction = 0.0;
    bool has_prev = false;
    double prev_pos = 0.0;
    
    for (size_t i = 0; i < loaded_session_data.size(); i++) {
        const auto &lap = loaded_session_data[i];
        if (lap.normalizedCarPosition.empty()) continue;
        
        for (size_t j = 0; j < lap.normalizedCarPosition.size(); j++) {
            double curr_pos = lap.normalizedCarPosition[j];
            
            if (has_prev) {
                double diff = curr_pos - prev_pos;
                
                if (diff < -0.5 && prev_pos > 0.8 && curr_pos < 0.2) {
                    diff += 1.0;
                } else if (diff > 0.5 && prev_pos < 0.2 && curr_pos > 0.8) {
                    diff -= 1.0;
                }
                
                if (std::abs(diff) > 0.1) {
                    diff = 0.0; // ignore large jumps
                }
                
                total_driven_fraction += diff;
            }
            
            prev_pos = curr_pos;
            has_prev = true;
        }
    }
    
    return total_driven_fraction;
}

void ACTelemetry::close_loaded_session() {
    loaded_session_data.clear();
    loaded_session_data.shrink_to_fit();

    loaded_session_lap_offsets.clear();
    loaded_session_lap_offsets.shrink_to_fit();

    loaded_session_sample_interval = 0.0;
    loaded_session_samples_per_meter = 0.0;
    loaded_session_lap_count = -1;
    loaded_session_static_data = {};
}

PackedFloat32Array ACTelemetry::calculate_lap_time_delta(String target_file_path, int target_lap_index, String current_file_path, int current_lap_index, PackedFloat32Array reference_positions) {
    PackedFloat32Array result;
    if (target_file_path.is_empty() || target_lap_index < 0 || current_lap_index < 0) return result;
    
    String actual_curr_path = current_file_path.is_empty() ? target_file_path : current_file_path;

    auto load_lap = [this](String file_path, int lap_index, LapDataChannels &out_lap) -> bool {
        std::ifstream infile;
        SPageStatic stat;
        double interval, spm;
        uint64_t count;
        std::vector<uint64_t> offsets;

        if (!_open_session_file(file_path, infile, stat, interval, spm, count, offsets).is_empty()) {
            return false;
        }

        if (lap_index < 0 || lap_index >= count) return false;

        infile.seekg(offsets[lap_index]);
        out_lap.read_from_stream(infile);
        return true;
    };

    LapDataChannels target_lap, current_lap;
    if (!load_lap(target_file_path, target_lap_index, target_lap)) return result;
    
    if (actual_curr_path == target_file_path && current_lap_index == target_lap_index) {
        current_lap = target_lap;
    } else {
        if (!load_lap(actual_curr_path, current_lap_index, current_lap)) return result;
    }

    const std::vector<float>& raw_target_pos = target_lap.normalizedCarPosition;
    const std::vector<double>& target_time = target_lap.timestamp;
    
    const std::vector<float>& raw_current_pos = current_lap.normalizedCarPosition;
    const std::vector<double>& current_time = current_lap.timestamp;

    int target_n = raw_target_pos.size();
    int current_n = raw_current_pos.size();
    if (target_n < 2 || current_n < 2) return result;

    const float* ref_data = nullptr;
    int ref_n = 0;
    if (reference_positions.size() > 0) {
        ref_data = reference_positions.ptr();
        ref_n = reference_positions.size();
    } else {
        ref_data = raw_current_pos.data();
        ref_n = raw_current_pos.size();
    }

    if (ref_n == 0) return result;

    result.resize(ref_n);
    float* ptr = result.ptrw();

    // handle wrap-around at the end of the lap for target
    std::vector<float> target_pos(target_n);
    float last_target_raw_p = raw_target_pos[0];
    float target_p_offset = 0.0f;
    for (int i = 0; i < target_n; ++i) {
        float raw_p = raw_target_pos[i];
        if (i > 0 && raw_p < last_target_raw_p - 0.5f) {
            target_p_offset += 1.0f;
        }
        last_target_raw_p = raw_p;
        target_pos[i] = raw_p + target_p_offset;
    }

    // handle wrap-around for current
    std::vector<float> curr_pos(current_n);
    float last_curr_raw_p = raw_current_pos[0];
    float curr_p_offset = 0.0f;
    for (int i = 0; i < current_n; ++i) {
        float raw_p = raw_current_pos[i];
        if (i > 0 && raw_p < last_curr_raw_p - 0.5f) {
            curr_p_offset += 1.0f;
        }
        last_curr_raw_p = raw_p;
        curr_pos[i] = raw_p + curr_p_offset;
    }

    int target_idx = 0;
    int curr_idx = 0;

    float last_ref_raw_p = ref_data[0];
    float ref_p_offset = 0.0f;

    for (int i = 0; i < ref_n; ++i) {
        float raw_p = ref_data[i];
        
        if (i > 0 && raw_p < last_ref_raw_p - 0.5f) {
            ref_p_offset += 1.0f;
        }
        last_ref_raw_p = raw_p;
        float ref_p = raw_p + ref_p_offset;

        // interpolate target
        while (target_idx < target_n - 1 && target_pos[target_idx + 1] < ref_p) {
            target_idx++;
        }

        float t_time;
        bool t_valid = true;
        if (target_idx >= target_n - 1) {
            if (ref_p > target_pos[target_n - 1]) {
                t_valid = false;
            } else {
                t_time = (float)target_time[target_n - 1];
            }
        } else if (ref_p < target_pos[0]) {
            t_valid = false;
        } else {
            float p1 = target_pos[target_idx];
            float p2 = target_pos[target_idx + 1];
            float t1 = (float)target_time[target_idx];
            float t2 = (float)target_time[target_idx + 1];
            
            if (p2 - p1 > 0.000001f) {
                float t = (ref_p - p1) / (p2 - p1);
                t_time = t1 + t * (t2 - t1);
            } else {
                t_time = t1;
            }
        }

        // interpolate current
        while (curr_idx < current_n - 1 && curr_pos[curr_idx + 1] < ref_p) {
            curr_idx++;
        }

        float c_time;
        bool c_valid = true;
        if (curr_idx >= current_n - 1) {
            if (ref_p > curr_pos[current_n - 1]) {
                c_valid = false;
            } else {
                c_time = (float)current_time[current_n - 1];
            }
        } else if (ref_p < curr_pos[0]) {
            c_valid = false;
        } else {
            float p1 = curr_pos[curr_idx];
            float p2 = curr_pos[curr_idx + 1];
            float t1 = (float)current_time[curr_idx];
            float t2 = (float)current_time[curr_idx + 1];
            
            if (p2 - p1 > 0.000001f) {
                float t = (ref_p - p1) / (p2 - p1);
                c_time = t1 + t * (t2 - t1);
            } else {
                c_time = t1;
            }
        }
        
        if (!t_valid || !c_valid) {
            ptr[i] = std::numeric_limits<float>::quiet_NaN();
        } else {
            ptr[i] = c_time - t_time;
        }
    }

    // calculated math channels usually suffer from high-frequency noise
    // due to position quantization and interpolation limits.
    // we apply a centered moving average to smooth the delta without introducing phase shift.
    smooth_float_array(result, 51); // 51 samples for smoother delta at high-res telemetry
    return result;
}

Dictionary ACTelemetry::_static_to_dict(const SPageStatic &s) {
    Dictionary d;

    d["smVersion"] = wchar_to_gdstring(s.smVersion, 15);
    d["acVersion"] = wchar_to_gdstring(s.acVersion, 15);

    d["numberOfSessions"] = s.numberOfSessions;
    d["numCars"] = s.numCars;
    d["carModel"] = wchar_to_gdstring(s.carModel, 33);
    d["track"] = wchar_to_gdstring(s.track, 33);
    d["playerName"] = wchar_to_gdstring(s.playerName, 33);
    d["playerSurname"] = wchar_to_gdstring(s.playerSurname, 33);
    d["playerNick"] = wchar_to_gdstring(s.playerNick, 33);
    d["sectorCount"] = s.sectorCount;
    
    d["maxTorque"] = s.maxTorque;
    d["maxPower"] = s.maxPower;
    d["maxRpm"] = s.maxRpm;
    d["maxFuel"] = s.maxFuel;
    d["suspensionMaxTravel"] = Vector4(s.suspensionMaxTravel[0], s.suspensionMaxTravel[1], s.suspensionMaxTravel[2], s.suspensionMaxTravel[3]);
    d["tyreRadius"] = Vector4(s.tyreRadius[0], s.tyreRadius[1], s.tyreRadius[2], s.tyreRadius[3]);
    d["maxTurboBoost"] = s.maxTurboBoost;

    d["penaltiesEnabled"] = s.penaltiesEnabled;

    d["aidFuelRate"] = s.aidFuelRate;
    d["aidTireRate"] = s.aidTireRate;
    d["aidMechanicalDamage"] = s.aidMechanicalDamage;
    d["aidAllowTyreBlankets"] = s.aidAllowTyreBlankets;
    d["aidStability"] = s.aidStability;
    d["aidAutoClutch"] = s.aidAutoClutch;
    d["aidAutoBlip"] = s.aidAutoBlip;

    d["hasDRS"] = s.hasDRS;
    d["hasERS"] = s.hasERS;
    d["hasKERS"] = s.hasKERS;
    d["kersMaxJ"] = s.kersMaxJ;
    d["engineBrakeSettingsCount"] = s.engineBrakeSettingsCount;
    d["ersPowerControllerCount"] = s.ersPowerControllerCount;
    
    d["trackSPlineLength"] = s.trackSPlineLength;
    d["trackConfiguration"] = wchar_to_gdstring(s.trackConfiguration, 33);
    
    d["ersMaxJ"] = s.ersMaxJ;
    d["isTimedRace"] = s.isTimedRace;
    d["hasExtraLap"] = s.hasExtraLap;
    d["carSkin"] = wchar_to_gdstring(s.carSkin, 33);
    d["reversedGridPositions"] = s.reversedGridPositions;
    d["pitWindowStart"] = s.pitWindowStart;
    d["pitWindowEnd"] = s.pitWindowEnd;
    d["isOnline"] = s.isOnline;

    return d;
}

void ACTelemetry::logging_loop() {
    auto interval = std::chrono::duration_cast<std::chrono::steady_clock::duration>(std::chrono::duration<double>(sample_interval));
    auto next_tick = std::chrono::steady_clock::now();

    timeBeginPeriod(1);

    int stale_counter = 0;
    int graphic_stale_counter = 0;
    int local_last_graphic_packet_id = -1;
    int max_stale_ticks = static_cast<int>(3.0 / sample_interval); // 3 secs timeout

    while (is_logging) {
        auto now = std::chrono::steady_clock::now();
        if (next_tick < now) {
            next_tick = now + interval;
        } else {
            next_tick += interval;
        }

        if (dataGraphic && dataPhysics) {
            
            if (dataGraphic->status == AC_OFF) {
                game_disconnected.store(true);
            }

            // graphics packet updates as long as the game process is alive
            // so we're detecting game crash or close with this
            if (dataGraphic->packetId == local_last_graphic_packet_id) {
                graphic_stale_counter++;
                if (graphic_stale_counter > max_stale_ticks) {
                    game_disconnected.store(true);
                }
            } else {
                graphic_stale_counter = 0;
            }
            local_last_graphic_packet_id = dataGraphic->packetId;
            
            
            if (dataGraphic->status != AC_LIVE) {
                stale_counter = 0;
                std::this_thread::sleep_until(next_tick);
                continue;
            }

            if (dataPhysics->packetId == last_physics_packet_id) {
                std::this_thread::sleep_until(next_tick);
                continue;
            }
            last_physics_packet_id = dataPhysics->packetId;

            {
                std::lock_guard<std::mutex> lock(data_mutex);

                bool lap_changed = false;
                if (dataGraphic->iCurrentTime < last_i_current_time) {
                    lap_changed = true;
                }
                last_i_current_time = dataGraphic->iCurrentTime;
                
                if (dataGraphic->completedLaps > last_lap_count) {
                    lap_changed = true;
                    last_lap_count = dataGraphic->completedLaps;
                }

                // detect spatial lap crossing to avoid delayed lap timers
                if (!sessions_data.empty() && !sessions_data.back().normalizedCarPosition.empty()) {
                    float last_pos = sessions_data.back().normalizedCarPosition.back();
                    if (last_pos > 0.8f && dataGraphic->normalizedCarPosition < 0.2f) {
                        lap_changed = true;
                    }
                    
                    float jump = std::abs(dataGraphic->normalizedCarPosition - last_pos);
                    if (jump > 0.1f && !(last_pos > 0.8f && dataGraphic->normalizedCarPosition < 0.2f)) {
                        lap_changed = true;
                    }
                }

                if (lap_changed || sessions_data.empty()) {
                    bool should_push = true;
                    if (!sessions_data.empty()) {
                        auto& last_lap = sessions_data.back();
                        if (last_lap.timestamp.size() < 10) {
                            should_push = false;
                        }
                    }

                    if (should_push) {
                        sessions_data.push_back(LapDataChannels());
                    } else {
                        sessions_data.back().clear();
                    }
                    last_recorded_meter = -INFINITY;
                }

                // track continuous spline distance
                // graphic position updates at 60Hz, physics at 333Hz.
                // using graphic pos directly causes staircase artifacts.
                // we integrate physics speed for smooth high-frequency distance
                // and soft-correct it to graphic pos.
                static double internal_meter = -1.0;
                
                if (last_recorded_meter == -INFINITY) {
                    internal_meter = -1.0; // reset on new lap
                }
                
                double graphic_spline_pos = dataGraphic->normalizedCarPosition * dataStatic->trackSPlineLength;
                
                if (internal_meter < 0.0 || dataStatic->trackSPlineLength <= 0.0) {
                    internal_meter = graphic_spline_pos;
                } else {
                    int packet_diff = dataPhysics->packetId - last_physics_packet_id;
                    if (packet_diff > 0 && packet_diff < 100) {
                        double dt = packet_diff / 333.333333333; // 333Hz physics tick
                        internal_meter += (dataPhysics->speedKmh / 3.6) * dt;
                    }
                    
                    // complementary filter: soft-correct towards graphic_spline_pos
                    double diff = graphic_spline_pos - internal_meter;
                    // handle track wrap-around
                    if (diff < -dataStatic->trackSPlineLength / 2.0) diff += dataStatic->trackSPlineLength;
                    if (diff > dataStatic->trackSPlineLength / 2.0) diff -= dataStatic->trackSPlineLength;
                    
                    if (std::abs(diff) < 20.0) {
                        internal_meter += diff * 0.05; // 5% correction per tick
                    } else {
                        internal_meter = graphic_spline_pos; // hard reset on big jump
                    }
                }
                
                if (dataStatic->trackSPlineLength > 0.0) {
                    while (internal_meter >= dataStatic->trackSPlineLength) internal_meter -= dataStatic->trackSPlineLength;
                    while (internal_meter < 0.0) internal_meter += dataStatic->trackSPlineLength;
                }

                double spline_pos = internal_meter;
                double dist_diff = 0.0;
                
                if (last_recorded_meter >= 0.0) {
                    dist_diff = spline_pos - last_recorded_meter;
                    // fix distance jump when track spline loops
                    if (dist_diff < -dataStatic->trackSPlineLength / 2.0) {
                        dist_diff += dataStatic->trackSPlineLength;
                    } else if (dist_diff > dataStatic->trackSPlineLength / 2.0) {
                        dist_diff -= dataStatic->trackSPlineLength;
                    }
                }
                
                double current_meter = (last_recorded_meter < 0.0) ? spline_pos : (last_recorded_meter + dist_diff);
                double distance_threshold = 1.0 / samples_per_meter;

                // check if we moved enough to record a new sample
                if (last_recorded_meter < 0.0 || std::abs(current_meter - last_recorded_meter) >= distance_threshold) {
                    double old_last_recorded_meter = last_recorded_meter;
                    double direction = 1.0;
                    
                    if (last_recorded_meter < 0.0) {
                        last_recorded_meter = current_meter;
                    } else {
                        double jump_size = std::abs(current_meter - last_recorded_meter);
                        
                        // handle teleport jumps (session restart, pit return, etc..)
                        // if the distance jumped is too large in a single frame
                        // we don't try to interpolate all the way back (cuz that jump is impossible)
                        if (jump_size > 20.0 && dataStatic->trackSPlineLength > 0.0 && 
                            jump_size < dataStatic->trackSPlineLength - 20.0) {
                            last_recorded_meter = current_meter;
                        } else {
                            direction = (current_meter > last_recorded_meter) ? 1.0 : -1.0;
                            last_recorded_meter += distance_threshold * direction;
                            
                            if (dataStatic->trackSPlineLength > 0.0) {
                                if (last_recorded_meter >= dataStatic->trackSPlineLength) {
                                    last_recorded_meter -= dataStatic->trackSPlineLength;
                                } else if (last_recorded_meter < 0.0) {
                                    last_recorded_meter += dataStatic->trackSPlineLength;
                                }
                            }
                        }
                    }
                    
                    auto& lap = sessions_data.back();

                    // time interpolation pass
                    // since we trigger records based on high-frequency distance (complementary filter above),
                    // we must also interpolate the 60hz graphic time to match the exact moment of this sample.
                    // we use the 333hz physics packet difference to smoothly advance the clock.
                    double smoothed_timestamp = dataGraphic->iCurrentTime / 1000.0;
                    int32_t smoothed_iCurrentTime = dataGraphic->iCurrentTime;
                    double smoothed_normalizedCarPosition = dataGraphic->normalizedCarPosition;
                    float smoothed_distanceTraveled = dataGraphic->distanceTraveled;

                    if (dataStatic->trackSPlineLength > 0.0) {
                        smoothed_normalizedCarPosition = last_recorded_meter / dataStatic->trackSPlineLength;
                    }

                    if (!lap.timestamp.empty() && old_last_recorded_meter >= 0.0) {
                        int packet_diff = dataPhysics->packetId - lap.packetId_physics.back();
                        if (packet_diff > 0 && packet_diff < 1000) {
                            double dt = packet_diff * (1.0 / 333.333333333); // ac physics is 333hz
                            smoothed_timestamp = lap.timestamp.back() + dt;
                            smoothed_iCurrentTime = lap.iCurrentTime.back() + static_cast<int32_t>(dt * 1000.0);
                        }
                        smoothed_distanceTraveled = lap.distanceTraveled.back() + distance_threshold * direction;
                    }

                    // graphic basics
                    lap.timestamp.push_back(smoothed_timestamp);
                    lap.packetId_graphic.push_back(dataGraphic->packetId);
                    lap.iCurrentTime.push_back(smoothed_iCurrentTime);
                    lap.iLastTime.push_back(dataGraphic->iLastTime);
                    lap.iBestTime.push_back(dataGraphic->iBestTime);
                    lap.normalizedCarPosition.push_back(smoothed_normalizedCarPosition);
                    lap.distanceTraveled.push_back(smoothed_distanceTraveled);
                    lap.replayTimeMultiplier.push_back(dataGraphic->replayTimeMultiplier);
                    lap.numberOfLaps.push_back(dataGraphic->numberOfLaps);
                    lap.completedLaps.push_back(dataGraphic->completedLaps);
                    
                    // graphic strings
                    auto push_str = [](auto& target_vec, const wchar_t* src, size_t size) {
                        target_vec.emplace_back();
                        std::memcpy(target_vec.back().data(), src, size * sizeof(wchar_t));
                    };
                    push_str(lap.currentTime, dataGraphic->currentTime, 15);
                    push_str(lap.lastTime, dataGraphic->lastTime, 15);
                    push_str(lap.bestTime, dataGraphic->bestTime, 15);
                    push_str(lap.split, dataGraphic->split, 15);
                    push_str(lap.tyreCompound, dataGraphic->tyreCompound, 33);

                    // physics basics
                    lap.packetId_physics.push_back(dataPhysics->packetId);
                    lap.gas.push_back(dataPhysics->gas);
                    lap.brake.push_back(dataPhysics->brake);
                    lap.fuel.push_back(dataPhysics->fuel);
                    lap.gear.push_back(dataPhysics->gear);
                    lap.rpms.push_back(dataPhysics->rpms);
                    lap.steerAngle.push_back(dataPhysics->steerAngle);
                    lap.speedKmh.push_back(dataPhysics->speedKmh);
                    lap.isAIControlled.push_back(dataPhysics->isAIControlled);

                    // vectors
                    lap.velocity_x.push_back(dataPhysics->velocity[0]);
                    lap.velocity_y.push_back(dataPhysics->velocity[1]);
                    lap.velocity_z.push_back(dataPhysics->velocity[2]);
                    
                    lap.accG_x.push_back(dataPhysics->accG[0]);
                    lap.accG_y.push_back(dataPhysics->accG[1]);
                    lap.accG_z.push_back(dataPhysics->accG[2]);

                    // wheels
                    lap.wheelSlip_fl.push_back(dataPhysics->wheelSlip[0]);
                    lap.wheelSlip_fr.push_back(dataPhysics->wheelSlip[1]);
                    lap.wheelSlip_rl.push_back(dataPhysics->wheelSlip[2]);
                    lap.wheelSlip_rr.push_back(dataPhysics->wheelSlip[3]);

                    lap.wheelLoad_fl.push_back(dataPhysics->wheelLoad[0]);
                    lap.wheelLoad_fr.push_back(dataPhysics->wheelLoad[1]);
                    lap.wheelLoad_rl.push_back(dataPhysics->wheelLoad[2]);
                    lap.wheelLoad_rr.push_back(dataPhysics->wheelLoad[3]);

                    lap.wheelsPressure_fl.push_back(dataPhysics->wheelsPressure[0]);
                    lap.wheelsPressure_fr.push_back(dataPhysics->wheelsPressure[1]);
                    lap.wheelsPressure_rl.push_back(dataPhysics->wheelsPressure[2]);
                    lap.wheelsPressure_rr.push_back(dataPhysics->wheelsPressure[3]);

                    lap.wheelAngularSpeed_fl.push_back(dataPhysics->wheelAngularSpeed[0]);
                    lap.wheelAngularSpeed_fr.push_back(dataPhysics->wheelAngularSpeed[1]);
                    lap.wheelAngularSpeed_rl.push_back(dataPhysics->wheelAngularSpeed[2]);
                    lap.wheelAngularSpeed_rr.push_back(dataPhysics->wheelAngularSpeed[3]);

                    lap.tyreWear_fl.push_back(dataPhysics->tyreWear[0]);
                    lap.tyreWear_fr.push_back(dataPhysics->tyreWear[1]);
                    lap.tyreWear_rl.push_back(dataPhysics->tyreWear[2]);
                    lap.tyreWear_rr.push_back(dataPhysics->tyreWear[3]);

                    lap.tyreDirtyLevel_fl.push_back(dataPhysics->tyreDirtyLevel[0]);
                    lap.tyreDirtyLevel_fr.push_back(dataPhysics->tyreDirtyLevel[1]);
                    lap.tyreDirtyLevel_rl.push_back(dataPhysics->tyreDirtyLevel[2]);
                    lap.tyreDirtyLevel_rr.push_back(dataPhysics->tyreDirtyLevel[3]);

                    lap.tyreCoreTemperature_fl.push_back(dataPhysics->tyreCoreTemperature[0]);
                    lap.tyreCoreTemperature_fr.push_back(dataPhysics->tyreCoreTemperature[1]);
                    lap.tyreCoreTemperature_rl.push_back(dataPhysics->tyreCoreTemperature[2]);
                    lap.tyreCoreTemperature_rr.push_back(dataPhysics->tyreCoreTemperature[3]);

                    lap.camberRAD_fl.push_back(dataPhysics->camberRAD[0]);
                    lap.camberRAD_fr.push_back(dataPhysics->camberRAD[1]);
                    lap.camberRAD_rl.push_back(dataPhysics->camberRAD[2]);
                    lap.camberRAD_rr.push_back(dataPhysics->camberRAD[3]);

                    lap.suspensionTravel_fl.push_back(dataPhysics->suspensionTravel[0]);
                    lap.suspensionTravel_fr.push_back(dataPhysics->suspensionTravel[1]);
                    lap.suspensionTravel_rl.push_back(dataPhysics->suspensionTravel[2]);
                    lap.suspensionTravel_rr.push_back(dataPhysics->suspensionTravel[3]);

                    lap.brakeTemp_fl.push_back(dataPhysics->brakeTemp[0]);
                    lap.brakeTemp_fr.push_back(dataPhysics->brakeTemp[1]);
                    lap.brakeTemp_rl.push_back(dataPhysics->brakeTemp[2]);
                    lap.brakeTemp_rr.push_back(dataPhysics->brakeTemp[3]);

                    lap.tyreTempI_fl.push_back(dataPhysics->tyreTempI[0]);
                    lap.tyreTempI_fr.push_back(dataPhysics->tyreTempI[1]);
                    lap.tyreTempI_rl.push_back(dataPhysics->tyreTempI[2]);
                    lap.tyreTempI_rr.push_back(dataPhysics->tyreTempI[3]);

                    lap.tyreTempM_fl.push_back(dataPhysics->tyreTempM[0]);
                    lap.tyreTempM_fr.push_back(dataPhysics->tyreTempM[1]);
                    lap.tyreTempM_rl.push_back(dataPhysics->tyreTempM[2]);
                    lap.tyreTempM_rr.push_back(dataPhysics->tyreTempM[3]);

                    lap.tyreTempO_fl.push_back(dataPhysics->tyreTempO[0]);
                    lap.tyreTempO_fr.push_back(dataPhysics->tyreTempO[1]);
                    lap.tyreTempO_rl.push_back(dataPhysics->tyreTempO[2]);
                    lap.tyreTempO_rr.push_back(dataPhysics->tyreTempO[3]);

                    // tyre contact point
                    lap.tyreContactPoint_fl_x.push_back(dataPhysics->tyreContactPoint[0][0]);
                    lap.tyreContactPoint_fl_y.push_back(dataPhysics->tyreContactPoint[0][1]);
                    lap.tyreContactPoint_fl_z.push_back(dataPhysics->tyreContactPoint[0][2]);
                    lap.tyreContactPoint_fr_x.push_back(dataPhysics->tyreContactPoint[1][0]);
                    lap.tyreContactPoint_fr_y.push_back(dataPhysics->tyreContactPoint[1][1]);
                    lap.tyreContactPoint_fr_z.push_back(dataPhysics->tyreContactPoint[1][2]);
                    lap.tyreContactPoint_rl_x.push_back(dataPhysics->tyreContactPoint[2][0]);
                    lap.tyreContactPoint_rl_y.push_back(dataPhysics->tyreContactPoint[2][1]);
                    lap.tyreContactPoint_rl_z.push_back(dataPhysics->tyreContactPoint[2][2]);
                    lap.tyreContactPoint_rr_x.push_back(dataPhysics->tyreContactPoint[3][0]);
                    lap.tyreContactPoint_rr_y.push_back(dataPhysics->tyreContactPoint[3][1]);
                    lap.tyreContactPoint_rr_z.push_back(dataPhysics->tyreContactPoint[3][2]);

                    // tyre contact normal
                    lap.tyreContactNormal_fl_x.push_back(dataPhysics->tyreContactNormal[0][0]);
                    lap.tyreContactNormal_fl_y.push_back(dataPhysics->tyreContactNormal[0][1]);
                    lap.tyreContactNormal_fl_z.push_back(dataPhysics->tyreContactNormal[0][2]);
                    lap.tyreContactNormal_fr_x.push_back(dataPhysics->tyreContactNormal[1][0]);
                    lap.tyreContactNormal_fr_y.push_back(dataPhysics->tyreContactNormal[1][1]);
                    lap.tyreContactNormal_fr_z.push_back(dataPhysics->tyreContactNormal[1][2]);
                    lap.tyreContactNormal_rl_x.push_back(dataPhysics->tyreContactNormal[2][0]);
                    lap.tyreContactNormal_rl_y.push_back(dataPhysics->tyreContactNormal[2][1]);
                    lap.tyreContactNormal_rl_z.push_back(dataPhysics->tyreContactNormal[2][2]);
                    lap.tyreContactNormal_rr_x.push_back(dataPhysics->tyreContactNormal[3][0]);
                    lap.tyreContactNormal_rr_y.push_back(dataPhysics->tyreContactNormal[3][1]);
                    lap.tyreContactNormal_rr_z.push_back(dataPhysics->tyreContactNormal[3][2]);

                    // tyre contact heading
                    lap.tyreContactHeading_fl_x.push_back(dataPhysics->tyreContactHeading[0][0]);
                    lap.tyreContactHeading_fl_y.push_back(dataPhysics->tyreContactHeading[0][1]);
                    lap.tyreContactHeading_fl_z.push_back(dataPhysics->tyreContactHeading[0][2]);
                    lap.tyreContactHeading_fr_x.push_back(dataPhysics->tyreContactHeading[1][0]);
                    lap.tyreContactHeading_fr_y.push_back(dataPhysics->tyreContactHeading[1][1]);
                    lap.tyreContactHeading_fr_z.push_back(dataPhysics->tyreContactHeading[1][2]);
                    lap.tyreContactHeading_rl_x.push_back(dataPhysics->tyreContactHeading[2][0]);
                    lap.tyreContactHeading_rl_y.push_back(dataPhysics->tyreContactHeading[2][1]);
                    lap.tyreContactHeading_rl_z.push_back(dataPhysics->tyreContactHeading[2][2]);
                    lap.tyreContactHeading_rr_x.push_back(dataPhysics->tyreContactHeading[3][0]);
                    lap.tyreContactHeading_rr_y.push_back(dataPhysics->tyreContactHeading[3][1]);
                    lap.tyreContactHeading_rr_z.push_back(dataPhysics->tyreContactHeading[3][2]);

                    // physics advanced
                    lap.drs.push_back(dataPhysics->drs);
                    lap.tc.push_back(dataPhysics->tc);
                    lap.heading.push_back(dataPhysics->heading);
                    lap.pitch.push_back(dataPhysics->pitch);
                    lap.roll.push_back(dataPhysics->roll);
                    lap.cgHeight.push_back(dataPhysics->cgHeight);
                    lap.pitLimiterOn.push_back(dataPhysics->pitLimiterOn);
                    lap.abs.push_back(dataPhysics->abs);
                    lap.kersCharge.push_back(dataPhysics->kersCharge);
                    lap.kersInput.push_back(dataPhysics->kersInput);
                    lap.autoShifterOn.push_back(dataPhysics->autoShifterOn);
                    lap.rideHeight_f.push_back(dataPhysics->rideHeight[0]);
                    lap.rideHeight_r.push_back(dataPhysics->rideHeight[1]);
                    lap.turboBoost.push_back(dataPhysics->turboBoost);
                    lap.ballast.push_back(dataPhysics->ballast);
                    lap.airDensity.push_back(dataPhysics->airDensity);
                    lap.airTemp.push_back(dataPhysics->airTemp);
                    lap.roadTemp.push_back(dataPhysics->roadTemp);
                    lap.localAngularVel_x.push_back(dataPhysics->localAngularVel[0]);
                    lap.localAngularVel_y.push_back(dataPhysics->localAngularVel[1]);
                    lap.localAngularVel_z.push_back(dataPhysics->localAngularVel[2]);
                    lap.finalFF.push_back(dataPhysics->finalFF);
                    lap.performanceMeter.push_back(dataPhysics->performanceMeter);
                    lap.engineBrake.push_back(dataPhysics->engineBrake);
                    lap.ersRecoveryLevel.push_back(dataPhysics->ersRecoveryLevel);
                    lap.ersPowerLevel.push_back(dataPhysics->ersPowerLevel);
                    lap.ersHeatCharging.push_back(dataPhysics->ersHeatCharging);
                    lap.ersIsCharging.push_back(dataPhysics->ersIsCharging);
                    lap.kersCurrentKJ.push_back(dataPhysics->kersCurrentKJ);
                    lap.drsAvailable.push_back(dataPhysics->drsAvailable);
                    lap.drsEnabled.push_back(dataPhysics->drsEnabled);
                    lap.clutch.push_back(dataPhysics->clutch);
                    lap.brakeBias.push_back(dataPhysics->brakeBias);
                    lap.localVelocity_x.push_back(dataPhysics->localVelocity[0]);
                    lap.localVelocity_y.push_back(dataPhysics->localVelocity[1]);
                    lap.localVelocity_z.push_back(dataPhysics->localVelocity[2]);

                    lap.carDamage_0.push_back(dataPhysics->carDamage[0]);
                    lap.carDamage_1.push_back(dataPhysics->carDamage[1]);
                    lap.carDamage_2.push_back(dataPhysics->carDamage[2]);
                    lap.carDamage_3.push_back(dataPhysics->carDamage[3]);
                    lap.carDamage_4.push_back(dataPhysics->carDamage[4]);
                    lap.numberOfTyresOut.push_back(dataPhysics->numberOfTyresOut);

                    // graphic
                    lap.status.push_back(dataGraphic->status);
                    lap.session.push_back(dataGraphic->session);
                    lap.position.push_back(dataGraphic->position);
                    lap.sessionTimeLeft.push_back(dataGraphic->sessionTimeLeft);
                    lap.isInPit.push_back(dataGraphic->isInPit);
                    lap.currentSectorIndex.push_back(dataGraphic->currentSectorIndex);
                    lap.lastSectorTime.push_back(dataGraphic->lastSectorTime);
                    lap.carCoordinates_x.push_back(dataGraphic->carCoordinates[0]);
                    lap.carCoordinates_y.push_back(dataGraphic->carCoordinates[1]);
                    lap.carCoordinates_z.push_back(dataGraphic->carCoordinates[2]);
                    lap.penaltyTime.push_back(dataGraphic->penaltyTime);
                    lap.flag.push_back(dataGraphic->flag);
                    lap.idealLineOn.push_back(dataGraphic->idealLineOn);
                    lap.isInPitLane.push_back(dataGraphic->isInPitLane);
                    lap.surfaceGrip.push_back(dataGraphic->surfaceGrip);
                    lap.mandatoryPitDone.push_back(dataGraphic->mandatoryPitDone);
                    lap.windSpeed.push_back(dataGraphic->windSpeed);
                    lap.windDirection.push_back(dataGraphic->windDirection);
                }
            }
        }

        std::this_thread::sleep_until(next_tick);
    }

    timeEndPeriod(1);
}