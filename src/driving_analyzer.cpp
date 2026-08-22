#include "driving_analyzer.h"
#include "sim_telemetry_manager.h"
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>

namespace godot {

godot::Array DrivingAnalyzer::analyze_lap(SimTelemetryManager* sim, const godot::Dictionary& lap, const godot::Dictionary& reference_lap) {
    godot::Array errors;
    if (!sim || lap.is_empty()) return errors;
    
    auto append_results = [&errors](const godot::Array& arr) {
        for (int i = 0; i < arr.size(); ++i) {
            errors.push_back(arr[i]);
        }
    };

    append_results(check_coasting(sim, lap, reference_lap));
    append_results(check_abs_abuse(sim, lap));
    append_results(check_trail_braking(sim, lap));
    append_results(check_shift_duration_and_downshift(sim, lap));
    append_results(check_over_slowing(sim, lap, reference_lap));
    append_results(check_snap_oversteer(sim, lap));
    append_results(check_throttle_flutter(sim, lap));
    append_results(check_pedal_overlap(sim, lap));

    return errors;
}

godot::Array DrivingAnalyzer::check_pedal_overlap(SimTelemetryManager* sim, const godot::Dictionary& lap) {
    godot::Array results;
    
    // get provider specific channel names for our standard names
    String time_ch = sim->get_channel_name("i_current_time");
    String gas_ch = sim->get_channel_name("throttle");
    String brake_ch = sim->get_channel_name("brake");
    String pos_ch = sim->get_channel_name("normalized_car_position"); // spline position

    // validate required channels exist
    if (!lap.has(time_ch) || !lap.has(gas_ch) || !lap.has(brake_ch) || !lap.has(pos_ch)) {
        return results;
    }

    PackedInt32Array time_data = lap[time_ch];
    PackedFloat32Array gas_data = lap[gas_ch];
    PackedFloat32Array brake_data = lap[brake_ch];
    PackedFloat32Array pos_data = lap[pos_ch];

    int size = time_data.size();
    if (size == 0 || size != gas_data.size() || size != brake_data.size() || size != pos_data.size()) {
        return results;
    }

    const int32_t* t_ptr = time_data.ptr();
    const float* g_ptr = gas_data.ptr();
    const float* b_ptr = brake_data.ptr();
    const float* p_ptr = pos_data.ptr();

    bool in_overlap = false;
    int overlap_start_time = 0;
    float overlap_start_pos = 0.0f;

    for (int i = 0; i < size; ++i) {
        if (g_ptr[i] > 0.10f && b_ptr[i] > 0.10f) {
            if (!in_overlap) {
                in_overlap = true;
                overlap_start_time = t_ptr[i];
                overlap_start_pos = p_ptr[i];
            }
        } else {
            if (in_overlap) {
                int duration = t_ptr[i] - overlap_start_time;
                if (duration > 200) { // > 200ms threshold
                    godot::Dictionary err;
                    err["type"] = "PEDAL_OVERLAP";
                    err["start_pos"] = overlap_start_pos;
                    err["end_pos"] = p_ptr[i];
                    err["score"] = (float)duration; // raw duration mapping
                    results.push_back(err);
                }
                in_overlap = false;
            }
        }
    }
    
    // check if lap ended while in overlap
    if (in_overlap) {
        int duration = t_ptr[size - 1] - overlap_start_time;
        if (duration > 200) {
            godot::Dictionary err;
            err["type"] = "PEDAL_OVERLAP";
            err["start_pos"] = overlap_start_pos;
            err["end_pos"] = p_ptr[size - 1];
            err["score"] = (float)duration;
            results.push_back(err);
        }
    }

    return results;
}

godot::Array DrivingAnalyzer::check_coasting(SimTelemetryManager* sim, const godot::Dictionary& lap, const godot::Dictionary& reference_lap) {
    godot::Array results;
    
    String time_ch = sim->get_channel_name("i_current_time");
    String gas_ch = sim->get_channel_name("throttle");
    String brake_ch = sim->get_channel_name("brake");
    String speed_ch = sim->get_channel_name("speedKmh");
    String pos_ch = sim->get_channel_name("normalized_car_position");

    if (!lap.has(time_ch) || !lap.has(gas_ch) || !lap.has(brake_ch) || !lap.has(speed_ch) || !lap.has(pos_ch)) {
        return results;
    }

    PackedInt32Array time_data = lap[time_ch];
    PackedFloat32Array gas_data = lap[gas_ch];
    PackedFloat32Array brake_data = lap[brake_ch];
    PackedFloat32Array speed_data = lap[speed_ch];
    PackedFloat32Array pos_data = lap[pos_ch];

    int size = time_data.size();
    if (size == 0 || size != gas_data.size() || size != brake_data.size() || size != speed_data.size() || size != pos_data.size()) {
        return results;
    }

    const int32_t* t_ptr = time_data.ptr();
    const float* g_ptr = gas_data.ptr();
    const float* b_ptr = brake_data.ptr();
    const float* s_ptr = speed_data.ptr();
    const float* p_ptr = pos_data.ptr();

    bool in_coasting = false;
    int coasting_start_time = 0;
    float coasting_start_pos = 0.0f;

    for (int i = 0; i < size; ++i) {
        if (g_ptr[i] < 0.05f && b_ptr[i] < 0.05f && s_ptr[i] > 50.0f) {
            if (!in_coasting) {
                in_coasting = true;
                coasting_start_time = t_ptr[i];
                coasting_start_pos = p_ptr[i];
            }
        } else {
            if (in_coasting) {
                int duration = t_ptr[i] - coasting_start_time;
                if (duration > 500) {
                    godot::Dictionary err;
                    err["type"] = "COASTING";
                    err["start_pos"] = coasting_start_pos;
                    err["end_pos"] = p_ptr[i];
                    err["score"] = (float)duration;
                    results.push_back(err);
                }
                in_coasting = false;
            }
        }
    }
    
    if (in_coasting) {
        int duration = t_ptr[size - 1] - coasting_start_time;
        if (duration > 500) {
            godot::Dictionary err;
            err["type"] = "COASTING";
            err["start_pos"] = coasting_start_pos;
            err["end_pos"] = p_ptr[size - 1];
            err["score"] = (float)duration;
            results.push_back(err);
        }
    }
    
    return results;
}

godot::Array DrivingAnalyzer::check_abs_abuse(SimTelemetryManager* sim, const godot::Dictionary& lap) {
    // TODO: implement abs over-engagement logic
    return godot::Array();
}

godot::Array DrivingAnalyzer::check_trail_braking(SimTelemetryManager* sim, const godot::Dictionary& lap) {
    // TODO: implement trail braking smoothness logic
    return godot::Array();
}

godot::Array DrivingAnalyzer::check_shift_duration_and_downshift(SimTelemetryManager* sim, const godot::Dictionary& lap) {
    // TODO: implement shift duration & downshift abuse logic
    return godot::Array();
}

godot::Array DrivingAnalyzer::check_over_slowing(SimTelemetryManager* sim, const godot::Dictionary& lap, const godot::Dictionary& reference_lap) {
    godot::Array results;
    if (reference_lap.is_empty()) return results; // we need a reference lap
    
    float track_length = 0.0f; // fallback for Spa
    godot::Dictionary static_data = sim->get_loaded_session_static_data();
    if (static_data.has("track_length")) {
        track_length = static_data["track_length"];
    }
    if (track_length == 0.0f) return results;

    String speed_ch = sim->get_channel_name("speed");
    String pos_ch = sim->get_channel_name("normalized_car_position");

    if (!lap.has(speed_ch) || !lap.has(pos_ch) || !reference_lap.has(speed_ch) || !reference_lap.has(pos_ch)) {
        return results;
    }

    PackedFloat32Array s_data = lap[speed_ch];
    PackedFloat32Array p_data = lap[pos_ch];
    PackedFloat32Array ref_s_data = reference_lap[speed_ch];
    PackedFloat32Array ref_p_data = reference_lap[pos_ch];

    int size = s_data.size();
    int ref_size = ref_s_data.size();
    
    if (size == 0 || size != p_data.size() || ref_size == 0 || ref_size != ref_p_data.size()) {
        return results;
    }

    const float* s_ptr = s_data.ptr();
    const float* p_ptr = p_data.ptr();
    const float* ref_s_ptr = ref_s_data.ptr();
    const float* ref_p_ptr = ref_p_data.ptr();

    float last_triggered_pos = -1.0f;
    // ensure window is distance-based
    // (approx 10 meters on each side = 20 meters total)
    int window = std::max(5, (int)(10.0 * sim->get_samples_per_meter()));
    
    for (int i = window; i < size - window; ++i) {
        // filter out high-speed straights,
        // check only if speed is < 200 km/h
        if (s_ptr[i] < 200.0f) {
            bool is_min = true;
            
            // check if it's the minimum in the window
            for (int j = i - window; j <= i + window; ++j) {
                if (s_ptr[j] < s_ptr[i]) {
                    is_min = false;
                    break;
                }
            }
            
            if (is_min) {
                // to avoid triggering multiple times on a flat bottom,
                // ensure the previous point was strictly greater,
                // or we are dropping into the minimum.
                if (s_ptr[i-1] > s_ptr[i] || (s_ptr[i-1] == s_ptr[i] && s_ptr[i-window] > s_ptr[i])) {
                    
                    float target_pos = p_ptr[i];
                    float ref_min_speed = 999.0f;
                    
                    // fast search for the minimum speed in the
                    // ref lap around the same spline position
                    for (int r = 0; r < ref_size; ++r) {
                        float ref_pos = ref_p_ptr[r];
                        float diff = std::abs(ref_pos - target_pos);
                        if (diff > 0.5f) diff = 1.0f - diff; // wrap around logic
                        
                        // shrink window to 30m
                        // to avoid cross-contaminating chicane apexes
                        float max_diff = 30.0f / track_length;
                        if (diff < max_diff) { 
                            if (ref_s_ptr[r] < ref_min_speed) {
                                ref_min_speed = ref_s_ptr[r];
                            }
                        }
                    }
                    
                    if (ref_min_speed < 999.0f) {
                        float speed_diff = ref_min_speed - s_ptr[i];
                        if (speed_diff > 5.0f) { // 5 km/h threshold
                            // ensure we don't spam errors
                            // for the same corner flat bottom (70m spacing)
                            float min_spacing = 70.0f / track_length;
                            if (last_triggered_pos < 0.0f || std::abs(target_pos - last_triggered_pos) > min_spacing) {
                                godot::Dictionary err;
                                err["type"] = "OVER_SLOWING";
                                err["start_pos"] = target_pos;
                                err["end_pos"] = -1.0f; // point error
                                err["score"] = speed_diff; // score is the km/h difference
                                results.push_back(err);
                                
                                last_triggered_pos = target_pos;
                            }
                        }
                    }
                }
            }
        }
    }
    
    return results;
}

godot::Array DrivingAnalyzer::check_snap_oversteer(SimTelemetryManager* sim, const godot::Dictionary& lap) {
    // TODO: implement snap oversteer / jerk analysis logic
    return godot::Array();
}

godot::Array DrivingAnalyzer::check_throttle_flutter(SimTelemetryManager* sim, const godot::Dictionary& lap) {
    // TODO: implement throttle flutter logic
    return godot::Array();
}

} // namespace godot
