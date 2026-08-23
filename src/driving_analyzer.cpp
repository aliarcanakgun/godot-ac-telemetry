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
    godot::Array results;
    
    String time_ch = sim->get_channel_name("i_current_time");
    String brake_ch = sim->get_channel_name("brake");
    String abs_ch = sim->get_channel_name("abs");
    String pos_ch = sim->get_channel_name("normalized_car_position");

    if (!lap.has(time_ch) || !lap.has(brake_ch) || !lap.has(abs_ch) || !lap.has(pos_ch)) {
        return results;
    }

    PackedInt32Array time_data = lap[time_ch];
    PackedFloat32Array brake_data = lap[brake_ch];
    PackedFloat32Array abs_data = lap[abs_ch];
    PackedFloat32Array pos_data = lap[pos_ch];

    int size = time_data.size();
    if (size == 0 || size != brake_data.size() || size != abs_data.size() || size != pos_data.size()) {
        return results;
    }

    const int32_t* t_ptr = time_data.ptr();
    const float* b_ptr = brake_data.ptr();
    const float* a_ptr = abs_data.ptr();
    const float* p_ptr = pos_data.ptr();

    bool in_braking = false;
    int braking_start_time = 0;
    float braking_start_pos = 0.0f;
    int abs_active_time = 0;
    int last_time = 0;

    for (int i = 0; i < size; ++i) {
        if (b_ptr[i] > 5.0f) { // 5% threshold
            if (!in_braking) {
                in_braking = true;
                braking_start_time = t_ptr[i];
                braking_start_pos = p_ptr[i];
                abs_active_time = 0;
                last_time = t_ptr[i];
            } else {
                int dt = t_ptr[i] - last_time;
                if (a_ptr[i] > 0.5f) { // abs is active (50% threshold)
                    abs_active_time += dt;
                }
                last_time = t_ptr[i];
            }
        } else {
            if (in_braking) {
                int total_braking_time = t_ptr[i] - braking_start_time;
                if (total_braking_time > 200) { // 200ms threshold
                    float abs_ratio = (float)abs_active_time / (float)total_braking_time * 100.0f;
                    if (abs_ratio > 30.0f) { // 30% abs engagement
                        godot::Dictionary err;
                        err["type"] = "ABS_ABUSE";
                        err["start_pos"] = braking_start_pos;
                        err["end_pos"] = p_ptr[i];
                        err["score"] = abs_ratio;
                        results.push_back(err);
                    }
                }
                in_braking = false;
            }
        }
    }
    
    // check if lap ended while braking
    if (in_braking) {
        int total_braking_time = t_ptr[size - 1] - braking_start_time;
        if (total_braking_time > 200) {
            float abs_ratio = (float)abs_active_time / (float)total_braking_time * 100.0f;
            if (abs_ratio > 30.0f) {
                godot::Dictionary err;
                err["type"] = "ABS_ABUSE";
                err["start_pos"] = braking_start_pos;
                err["end_pos"] = p_ptr[size - 1];
                err["score"] = abs_ratio;
                results.push_back(err);
            }
        }
    }

    return results;
}

godot::Array DrivingAnalyzer::check_trail_braking(SimTelemetryManager* sim, const godot::Dictionary& lap) {
    godot::Array results;
    
    String time_ch = sim->get_channel_name("i_current_time");
    String brake_ch = sim->get_channel_name("brake");
    String pos_ch = sim->get_channel_name("normalized_car_position");

    if (!lap.has(time_ch) || !lap.has(brake_ch) || !lap.has(pos_ch)) {
        return results;
    }

    PackedInt32Array time_data = lap[time_ch];
    PackedFloat32Array brake_data = lap[brake_ch];
    PackedFloat32Array pos_data = lap[pos_ch];

    int size = time_data.size();
    if (size == 0 || size != brake_data.size() || size != pos_data.size()) {
        return results;
    }

    const int32_t* t_ptr = time_data.ptr();
    const float* b_ptr = brake_data.ptr();
    const float* p_ptr = pos_data.ptr();

    int braking_start_idx = -1;
    bool in_braking = false;

    auto analyze_zone = [&](int start_idx, int end_idx) {
        if (end_idx - start_idx < 5) return; // not enough data

        // find peak brake
        int peak_idx = start_idx;
        float peak_brake = b_ptr[start_idx];
        for (int j = start_idx; j <= end_idx; ++j) {
            if (b_ptr[j] > peak_brake) {
                peak_brake = b_ptr[j];
                peak_idx = j;
            }
        }

        if (peak_brake < 80.0f) return; // not a heavy braking zone

        // analyze release phase
        int n_samples = 0;
        float min_deriv = 0.0f;
        double sum_deriv = 0.0;
        double sum_sq_deriv = 0.0;

        for (int j = peak_idx + 1; j <= end_idx; ++j) {
            float dt_sec = (t_ptr[j] - t_ptr[j - 1]) / 1000.0f;
            if (dt_sec <= 0.001f) continue;

            float db = b_ptr[j] - b_ptr[j - 1];
            float deriv = db / dt_sec;

            if (n_samples == 0 || deriv < min_deriv) {
                min_deriv = deriv;
            }

            sum_deriv += deriv;
            sum_sq_deriv += (deriv * deriv);
            n_samples++;
        }

        if (n_samples > 1) {
            if (min_deriv < -400.0f) { // > 400%/s release rate means sudden release
                double mean = sum_deriv / n_samples;
                double variance = std::max(0.0, (sum_sq_deriv / n_samples) - (mean * mean));
                double std_dev = std::sqrt(variance);
                
                godot::Dictionary err;
                err["type"] = "TRAIL_BRAKING_SMOOTHNESS";
                err["start_pos"] = p_ptr[peak_idx];
                err["end_pos"] = p_ptr[end_idx];
                err["score"] = std::abs(min_deriv); // score as the peak release rate
                results.push_back(err);
            }
        }
    };

    for (int i = 0; i < size; ++i) {
        if (b_ptr[i] > 10.0f) {
            if (!in_braking) {
                in_braking = true;
                braking_start_idx = i;
            }
        } else {
            if (in_braking) {
                analyze_zone(braking_start_idx, i);
                in_braking = false;
            }
        }
    }
    
    // check if lap ended while braking
    if (in_braking) {
        analyze_zone(braking_start_idx, size - 1);
    }

    return results;
}

godot::Array DrivingAnalyzer::check_shift_duration_and_downshift(SimTelemetryManager* sim, const godot::Dictionary& lap) {
    godot::Array results;
    
    String time_ch = sim->get_channel_name("i_current_time");
    String gear_ch = sim->get_channel_name("gear");
    String rpm_ch = sim->get_channel_name("rpm");
    String speed_ch = sim->get_channel_name("speed");
    String pos_ch = sim->get_channel_name("normalized_car_position");

    if (!lap.has(time_ch) || !lap.has(gear_ch) || !lap.has(rpm_ch) || !lap.has(speed_ch) || !lap.has(pos_ch)) {
        return results;
    }

    PackedInt32Array time_data = lap[time_ch];
    PackedInt32Array gear_data = lap[gear_ch];
    PackedInt32Array rpm_data = lap[rpm_ch];
    PackedFloat32Array speed_data = lap[speed_ch];
    PackedFloat32Array pos_data = lap[pos_ch];

    int size = time_data.size();
    if (size == 0 || size != gear_data.size() || size != rpm_data.size() || size != speed_data.size() || size != pos_data.size()) {
        return results;
    }

    const int32_t* t_ptr = time_data.ptr();
    const int32_t* g_ptr = gear_data.ptr();
    const int32_t* r_ptr = rpm_data.ptr();
    const float* s_ptr = speed_data.ptr();
    const float* p_ptr = pos_data.ptr();

    godot::Dictionary static_data = sim->get_loaded_session_static_data();
    float max_rpm = 8000.0f; // fallback
    if (static_data.has("max_rpm")) max_rpm = static_data["max_rpm"];

    bool in_neutral = false;
    int neutral_start_time = 0;
    float neutral_start_pos = 0.0f;

    for (int i = 1; i < size; ++i) {
        // slow shift (time spent in neutral)
        // 0 is neutral, -1 is reverse
        if (g_ptr[i] == 0 && s_ptr[i] > 50.0f) {
            if (!in_neutral) {
                in_neutral = true;
                neutral_start_time = t_ptr[i];
                neutral_start_pos = p_ptr[i];
            }
        } else {
            if (in_neutral) {
                int shift_duration = t_ptr[i] - neutral_start_time;
                if (shift_duration > 500) { // 500ms threshold
                    godot::Dictionary err;
                    err["type"] = "SLOW_SHIFT";
                    err["start_pos"] = neutral_start_pos;
                    err["end_pos"] = p_ptr[i];
                    err["score"] = (float)shift_duration;
                    results.push_back(err);
                }
                in_neutral = false;
            }
        }

        // downshift abuse (early downshifting)
        if (g_ptr[i] < g_ptr[i - 1] && g_ptr[i] > 0) { // downshifted (excluding neutral)
            // check rpm right after downshift
            float current_rpm = (float)r_ptr[i];
            float rpm_ratio = current_rpm / max_rpm;
            
            if (rpm_ratio > 0.95f) { // > 95% of max rpm
                godot::Dictionary err;
                err["type"] = "AGGRESSIVE_DOWNSHIFT";
                err["start_pos"] = p_ptr[i];
                err["end_pos"] = -1.0f; // point error
                err["score"] = rpm_ratio * 100.0f; // score as percentage
                results.push_back(err);
            }
        }
    }

    if (in_neutral) {
        int shift_duration = t_ptr[size - 1] - neutral_start_time;
        if (shift_duration > 500) {
            godot::Dictionary err;
            err["type"] = "SLOW_SHIFT";
            err["start_pos"] = neutral_start_pos;
            err["end_pos"] = p_ptr[size - 1];
            err["score"] = (float)shift_duration;
            results.push_back(err);
        }
    }

    return results;
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
