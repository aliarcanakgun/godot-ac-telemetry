#include "gd_lap_telemetry.h"
#include "helper.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>

using namespace godot;

GDLapTelemetry::GDLapTelemetry() {}
GDLapTelemetry::~GDLapTelemetry() {}

void GDLapTelemetry::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_channels"), &GDLapTelemetry::get_channels);
    ClassDB::bind_method(D_METHOD("get_channel_average", "channel_name"), &GDLapTelemetry::get_channel_average);
}

// helper functions to build arrays fast
template<typename T>
static PackedFloat32Array to_float_array(const std::vector<T>& vec) {
    PackedFloat32Array arr;
    arr.resize(vec.size());
    float* ptr = arr.ptrw();
    for (size_t i = 0; i < vec.size(); ++i) {
        ptr[i] = static_cast<float>(vec[i]);
    }
    return arr;
}

template<typename T>
static PackedInt32Array to_int_array(const std::vector<T>& vec) {
    PackedInt32Array arr;
    arr.resize(vec.size());
    int32_t* ptr = arr.ptrw();
    for (size_t i = 0; i < vec.size(); ++i) {
        ptr[i] = static_cast<int32_t>(vec[i]);
    }
    return arr;
}

template<typename T>
static PackedInt32Array to_int_array_offset(const std::vector<T>& vec, int offset) {
    PackedInt32Array arr;
    arr.resize(vec.size());
    int32_t* ptr = arr.ptrw();
    for (size_t i = 0; i < vec.size(); ++i) {
        ptr[i] = static_cast<int32_t>(vec[i]) + offset;
    }
    return arr;
}

template<size_t N>
static PackedStringArray to_string_array(const std::vector<std::array<wchar_t, N>>& vec) {
    PackedStringArray arr;
    arr.resize(vec.size());
    String* ptr = arr.ptrw();
    for (size_t i = 0; i < vec.size(); ++i) {
        ptr[i] = String(vec[i].data());
    }
    return arr;
}

template<typename T>
static PackedFloat32Array to_float_array_deg(const std::vector<T>& vec) {
    PackedFloat32Array arr;
    arr.resize(vec.size());
    float* ptr = arr.ptrw();
    for (size_t i = 0; i < vec.size(); ++i) {
        ptr[i] = static_cast<float>(vec[i]) * 57.29578f;
    }
    return arr;
}

template<typename T>
static PackedFloat32Array to_float_array_rpm(const std::vector<T>& vec) {
    PackedFloat32Array arr;
    arr.resize(vec.size());
    float* ptr = arr.ptrw();
    for (size_t i = 0; i < vec.size(); ++i) {
        ptr[i] = static_cast<float>(vec[i]) * 9.549297f;
    }
    return arr;
}

template <typename T>
static PackedFloat32Array to_float_array_pct(const std::vector<T>& vec) {
    PackedFloat32Array arr;
    arr.resize(vec.size());
    float* ptr = arr.ptrw();
    for (size_t i = 0; i < vec.size(); ++i) {
        ptr[i] = static_cast<float>(vec[i]) * 100.0f;
    }
    return arr;
}

template <typename T>
static PackedFloat32Array to_float_array_mm(const std::vector<T>& vec) {
    PackedFloat32Array arr;
    arr.resize(vec.size());
    float* ptr = arr.ptrw();
    for (size_t i = 0; i < vec.size(); ++i) {
        ptr[i] = static_cast<float>(vec[i]) * 1000.0f;
    }
    return arr;
}

static PackedFloat32Array calc_derivative(const std::vector<float>& values, const std::vector<int32_t>& time_ms, int smoothing_window, float multiplier = 1.0f) {
    PackedFloat32Array arr;
    if (values.empty() || time_ms.empty() || values.size() != time_ms.size()) return arr;
    arr.resize(values.size());
    float* ptr = arr.ptrw();
    
    // forward difference for the first point
    if (values.size() > 1) {
        float dt = (float)(time_ms[1] - time_ms[0]) / 1000.0f;
        ptr[0] = (dt > 0.0001f) ? ((values[1] - values[0]) / dt) * multiplier : 0.0f;
    } else {
        ptr[0] = 0.0f;
    }

    // central difference for the middle
    for (size_t i = 1; i < values.size() - 1; ++i) {
        float dt = (float)(time_ms[i + 1] - time_ms[i - 1]) / 1000.0f;
        if (dt > 0.0001f) {
            ptr[i] = ((values[i + 1] - values[i - 1]) / dt) * multiplier;
        } else {
            ptr[i] = 0.0f;
        }
    }

    // backward difference for the last point
    size_t last = values.size() - 1;
    if (last > 0) {
        float dt = (float)(time_ms[last] - time_ms[last - 1]) / 1000.0f;
        ptr[last] = (dt > 0.0001f) ? ((values[last] - values[last - 1]) / dt) * multiplier : 0.0f;
    } else if (last > 0) {
        ptr[last] = 0.0f;
    }
    
    smooth_float_array(arr, smoothing_window);
    return arr;
}

void GDLapTelemetry::set_samples_per_meter(float p_spm) {
    samples_per_meter = p_spm;
}

void GDLapTelemetry::fill_from_channels(const LapDataChannels &c) {
    cached_channels.clear();
    
    int derivative_smoothing_window = std::max(3, (int)(5.0 * samples_per_meter));
    if (derivative_smoothing_window % 2 == 0) derivative_smoothing_window += 1;
    
    // graphic basics
    cached_channels["timestamp"] = to_float_array(c.timestamp);
    cached_channels["packetId_graphic"] = to_int_array(c.packetId_graphic);
    cached_channels["iCurrentTime"] = to_int_array(c.iCurrentTime);
    cached_channels["iLastTime"] = to_int_array(c.iLastTime);
    cached_channels["iBestTime"] = to_int_array(c.iBestTime);
    cached_channels["normalizedCarPosition"] = to_float_array(c.normalizedCarPosition);
    cached_channels["distanceTraveled"] = to_float_array(c.distanceTraveled);
    cached_channels["replayTimeMultiplier"] = to_float_array(c.replayTimeMultiplier);
    cached_channels["numberOfLaps"] = to_int_array(c.numberOfLaps);
    cached_channels["completedLaps"] = to_int_array(c.completedLaps);
    
    // graphic strings
    cached_channels["currentTime"] = to_string_array(c.currentTime);
    cached_channels["lastTime"] = to_string_array(c.lastTime);
    cached_channels["bestTime"] = to_string_array(c.bestTime);
    cached_channels["split"] = to_string_array(c.split);
    cached_channels["tyreCompound"] = to_string_array(c.tyreCompound);

    // basics
    cached_channels["packetId_physics"] = to_int_array(c.packetId_physics);
    cached_channels["gas"] = to_float_array_pct(c.gas);
    cached_channels["brake"] = to_float_array_pct(c.brake);
    cached_channels["fuel"] = to_float_array(c.fuel);
    cached_channels["gear"] = to_int_array_offset(c.gear, -1);
    cached_channels["rpms"] = to_int_array(c.rpms);
    cached_channels["steerAngle"] = to_float_array(c.steerAngle);
    cached_channels["speedKmh"] = to_float_array(c.speedKmh);
    cached_channels["isAIControlled"] = to_int_array(c.isAIControlled);
    
    PackedFloat32Array gd_gas = cached_channels["gas"];
    PackedFloat32Array gd_brake = cached_channels["brake"];
    
    PackedFloat32Array combined_pedals;
    combined_pedals.resize(gd_gas.size());
    float* combined_ptr = combined_pedals.ptrw();
    
    for (int i = 0; i < gd_gas.size(); i++) {
        combined_ptr[i] = (gd_gas[i] - gd_brake[i]) * 100.0f;
    }
    cached_channels["gasBrakeCombined"] = combined_pedals;

    // vectors
    cached_channels["velocity_x"] = to_float_array(c.velocity_x);
    cached_channels["velocity_y"] = to_float_array(c.velocity_y);
    cached_channels["velocity_z"] = to_float_array(c.velocity_z);
    
    cached_channels["accG_x"] = to_float_array(c.accG_x);
    cached_channels["accG_y"] = to_float_array(c.accG_y);
    cached_channels["accG_z"] = to_float_array(c.accG_z);

    // wheels (fl, fr, rl, rr)
    cached_channels["wheelSlip_fl"] = to_float_array(c.wheelSlip_fl);
    cached_channels["wheelSlip_fr"] = to_float_array(c.wheelSlip_fr);
    cached_channels["wheelSlip_rl"] = to_float_array(c.wheelSlip_rl);
    cached_channels["wheelSlip_rr"] = to_float_array(c.wheelSlip_rr);

    cached_channels["wheelLoad_fl"] = to_float_array(c.wheelLoad_fl);
    cached_channels["wheelLoad_fr"] = to_float_array(c.wheelLoad_fr);
    cached_channels["wheelLoad_rl"] = to_float_array(c.wheelLoad_rl);
    cached_channels["wheelLoad_rr"] = to_float_array(c.wheelLoad_rr);

    cached_channels["wheelsPressure_fl"] = to_float_array(c.wheelsPressure_fl);
    cached_channels["wheelsPressure_fr"] = to_float_array(c.wheelsPressure_fr);
    cached_channels["wheelsPressure_rl"] = to_float_array(c.wheelsPressure_rl);
    cached_channels["wheelsPressure_rr"] = to_float_array(c.wheelsPressure_rr);

    cached_channels["wheelAngularSpeed_fl"] = to_float_array_rpm(c.wheelAngularSpeed_fl);
    cached_channels["wheelAngularSpeed_fr"] = to_float_array_rpm(c.wheelAngularSpeed_fr);
    cached_channels["wheelAngularSpeed_rl"] = to_float_array_rpm(c.wheelAngularSpeed_rl);
    cached_channels["wheelAngularSpeed_rr"] = to_float_array_rpm(c.wheelAngularSpeed_rr);

    cached_channels["tyreWear_fl"] = to_float_array(c.tyreWear_fl);
    cached_channels["tyreWear_fr"] = to_float_array(c.tyreWear_fr);
    cached_channels["tyreWear_rl"] = to_float_array(c.tyreWear_rl);
    cached_channels["tyreWear_rr"] = to_float_array(c.tyreWear_rr);

    cached_channels["tyreDirtyLevel_fl"] = to_float_array(c.tyreDirtyLevel_fl);
    cached_channels["tyreDirtyLevel_fr"] = to_float_array(c.tyreDirtyLevel_fr);
    cached_channels["tyreDirtyLevel_rl"] = to_float_array(c.tyreDirtyLevel_rl);
    cached_channels["tyreDirtyLevel_rr"] = to_float_array(c.tyreDirtyLevel_rr);

    cached_channels["tyreCoreTemperature_fl"] = to_float_array(c.tyreCoreTemperature_fl);
    cached_channels["tyreCoreTemperature_fr"] = to_float_array(c.tyreCoreTemperature_fr);
    cached_channels["tyreCoreTemperature_rl"] = to_float_array(c.tyreCoreTemperature_rl);
    cached_channels["tyreCoreTemperature_rr"] = to_float_array(c.tyreCoreTemperature_rr);

    cached_channels["camberRAD_fl"] = to_float_array(c.camberRAD_fl);
    cached_channels["camberRAD_fr"] = to_float_array(c.camberRAD_fr);
    cached_channels["camberRAD_rl"] = to_float_array(c.camberRAD_rl);
    cached_channels["camberRAD_rr"] = to_float_array(c.camberRAD_rr);

    cached_channels["camberDEG_fl"] = to_float_array_deg(c.camberRAD_fl);
    cached_channels["camberDEG_fr"] = to_float_array_deg(c.camberRAD_fr);
    cached_channels["camberDEG_rl"] = to_float_array_deg(c.camberRAD_rl);
    cached_channels["camberDEG_rr"] = to_float_array_deg(c.camberRAD_rr);

    cached_channels["suspensionTravel_fl"] = to_float_array_mm(c.suspensionTravel_fl);
    cached_channels["suspensionTravel_fr"] = to_float_array_mm(c.suspensionTravel_fr);
    cached_channels["suspensionTravel_rl"] = to_float_array_mm(c.suspensionTravel_rl);
    cached_channels["suspensionTravel_rr"] = to_float_array_mm(c.suspensionTravel_rr);

    cached_channels["damperVelocity_fl"] = calc_derivative(c.suspensionTravel_fl, c.iCurrentTime, derivative_smoothing_window, 1000.0f);
    cached_channels["damperVelocity_fr"] = calc_derivative(c.suspensionTravel_fr, c.iCurrentTime, derivative_smoothing_window, 1000.0f);
    cached_channels["damperVelocity_rl"] = calc_derivative(c.suspensionTravel_rl, c.iCurrentTime, derivative_smoothing_window, 1000.0f);
    cached_channels["damperVelocity_rr"] = calc_derivative(c.suspensionTravel_rr, c.iCurrentTime, derivative_smoothing_window, 1000.0f);

    cached_channels["brakeTemp_fl"] = to_float_array(c.brakeTemp_fl);
    cached_channels["brakeTemp_fr"] = to_float_array(c.brakeTemp_fr);
    cached_channels["brakeTemp_rl"] = to_float_array(c.brakeTemp_rl);
    cached_channels["brakeTemp_rr"] = to_float_array(c.brakeTemp_rr);

    cached_channels["tyreTempI_fl"] = to_float_array(c.tyreTempI_fl);
    cached_channels["tyreTempI_fr"] = to_float_array(c.tyreTempI_fr);
    cached_channels["tyreTempI_rl"] = to_float_array(c.tyreTempI_rl);
    cached_channels["tyreTempI_rr"] = to_float_array(c.tyreTempI_rr);

    cached_channels["tyreTempM_fl"] = to_float_array(c.tyreTempM_fl);
    cached_channels["tyreTempM_fr"] = to_float_array(c.tyreTempM_fr);
    cached_channels["tyreTempM_rl"] = to_float_array(c.tyreTempM_rl);
    cached_channels["tyreTempM_rr"] = to_float_array(c.tyreTempM_rr);

    cached_channels["tyreTempO_fl"] = to_float_array(c.tyreTempO_fl);
    cached_channels["tyreTempO_fr"] = to_float_array(c.tyreTempO_fr);
    cached_channels["tyreTempO_rl"] = to_float_array(c.tyreTempO_rl);
    cached_channels["tyreTempO_rr"] = to_float_array(c.tyreTempO_rr);
    
    // tyre contact point
    cached_channels["tyreContactPoint_fl_x"] = to_float_array(c.tyreContactPoint_fl_x);
    cached_channels["tyreContactPoint_fl_y"] = to_float_array(c.tyreContactPoint_fl_y);
    cached_channels["tyreContactPoint_fl_z"] = to_float_array(c.tyreContactPoint_fl_z);
    cached_channels["tyreContactPoint_fr_x"] = to_float_array(c.tyreContactPoint_fr_x);
    cached_channels["tyreContactPoint_fr_y"] = to_float_array(c.tyreContactPoint_fr_y);
    cached_channels["tyreContactPoint_fr_z"] = to_float_array(c.tyreContactPoint_fr_z);
    cached_channels["tyreContactPoint_rl_x"] = to_float_array(c.tyreContactPoint_rl_x);
    cached_channels["tyreContactPoint_rl_y"] = to_float_array(c.tyreContactPoint_rl_y);
    cached_channels["tyreContactPoint_rl_z"] = to_float_array(c.tyreContactPoint_rl_z);
    cached_channels["tyreContactPoint_rr_x"] = to_float_array(c.tyreContactPoint_rr_x);
    cached_channels["tyreContactPoint_rr_y"] = to_float_array(c.tyreContactPoint_rr_y);
    cached_channels["tyreContactPoint_rr_z"] = to_float_array(c.tyreContactPoint_rr_z);

    // tyre contact normal
    cached_channels["tyreContactNormal_fl_x"] = to_float_array(c.tyreContactNormal_fl_x);
    cached_channels["tyreContactNormal_fl_y"] = to_float_array(c.tyreContactNormal_fl_y);
    cached_channels["tyreContactNormal_fl_z"] = to_float_array(c.tyreContactNormal_fl_z);
    cached_channels["tyreContactNormal_fr_x"] = to_float_array(c.tyreContactNormal_fr_x);
    cached_channels["tyreContactNormal_fr_y"] = to_float_array(c.tyreContactNormal_fr_y);
    cached_channels["tyreContactNormal_fr_z"] = to_float_array(c.tyreContactNormal_fr_z);
    cached_channels["tyreContactNormal_rl_x"] = to_float_array(c.tyreContactNormal_rl_x);
    cached_channels["tyreContactNormal_rl_y"] = to_float_array(c.tyreContactNormal_rl_y);
    cached_channels["tyreContactNormal_rl_z"] = to_float_array(c.tyreContactNormal_rl_z);
    cached_channels["tyreContactNormal_rr_x"] = to_float_array(c.tyreContactNormal_rr_x);
    cached_channels["tyreContactNormal_rr_y"] = to_float_array(c.tyreContactNormal_rr_y);
    cached_channels["tyreContactNormal_rr_z"] = to_float_array(c.tyreContactNormal_rr_z);

    // tyre contact heading
    cached_channels["tyreContactHeading_fl_x"] = to_float_array(c.tyreContactHeading_fl_x);
    cached_channels["tyreContactHeading_fl_y"] = to_float_array(c.tyreContactHeading_fl_y);
    cached_channels["tyreContactHeading_fl_z"] = to_float_array(c.tyreContactHeading_fl_z);
    cached_channels["tyreContactHeading_fr_x"] = to_float_array(c.tyreContactHeading_fr_x);
    cached_channels["tyreContactHeading_fr_y"] = to_float_array(c.tyreContactHeading_fr_y);
    cached_channels["tyreContactHeading_fr_z"] = to_float_array(c.tyreContactHeading_fr_z);
    cached_channels["tyreContactHeading_rl_x"] = to_float_array(c.tyreContactHeading_rl_x);
    cached_channels["tyreContactHeading_rl_y"] = to_float_array(c.tyreContactHeading_rl_y);
    cached_channels["tyreContactHeading_rl_z"] = to_float_array(c.tyreContactHeading_rl_z);
    cached_channels["tyreContactHeading_rr_x"] = to_float_array(c.tyreContactHeading_rr_x);
    cached_channels["tyreContactHeading_rr_y"] = to_float_array(c.tyreContactHeading_rr_y);
    cached_channels["tyreContactHeading_rr_z"] = to_float_array(c.tyreContactHeading_rr_z);

    // physics advanced
    cached_channels["drs"] = to_float_array(c.drs);
    cached_channels["tc"] = to_float_array_pct(c.tc);
    cached_channels["heading"] = to_float_array_deg(c.heading);
    cached_channels["pitch"] = to_float_array_deg(c.pitch);
    cached_channels["roll"] = to_float_array_deg(c.roll);
    cached_channels["cgHeight"] = to_float_array_mm(c.cgHeight);
    cached_channels["pitLimiterOn"] = to_int_array(c.pitLimiterOn);
    cached_channels["abs"] = to_float_array_pct(c.abs);
    cached_channels["kersCharge"] = to_float_array_pct(c.kersCharge);
    cached_channels["kersInput"] = to_float_array_pct(c.kersInput);
    cached_channels["autoShifterOn"] = to_int_array(c.autoShifterOn);
    cached_channels["rideHeight_f"] = to_float_array_mm(c.rideHeight_f);
    cached_channels["rideHeight_r"] = to_float_array_mm(c.rideHeight_r);
    cached_channels["turboBoost"] = to_float_array(c.turboBoost);
    cached_channels["ballast"] = to_float_array(c.ballast);
    cached_channels["airDensity"] = to_float_array(c.airDensity);
    cached_channels["airTemp"] = to_float_array(c.airTemp);
    cached_channels["roadTemp"] = to_float_array(c.roadTemp);
    cached_channels["localAngularVel_x"] = to_float_array_deg(c.localAngularVel_x);
    cached_channels["localAngularVel_y"] = to_float_array_deg(c.localAngularVel_y);
    cached_channels["localAngularVel_z"] = to_float_array_deg(c.localAngularVel_z);
    cached_channels["finalFF"] = to_float_array(c.finalFF);
    cached_channels["performanceMeter"] = to_float_array(c.performanceMeter);
    cached_channels["engineBrake"] = to_int_array(c.engineBrake);
    cached_channels["ersRecoveryLevel"] = to_int_array(c.ersRecoveryLevel);
    cached_channels["ersPowerLevel"] = to_int_array(c.ersPowerLevel);
    cached_channels["ersHeatCharging"] = to_int_array(c.ersHeatCharging);
    cached_channels["ersIsCharging"] = to_int_array(c.ersIsCharging);
    cached_channels["kersCurrentKJ"] = to_float_array(c.kersCurrentKJ);
    cached_channels["drsAvailable"] = to_int_array(c.drsAvailable);
    cached_channels["drsEnabled"] = to_int_array(c.drsEnabled);
    cached_channels["clutch"] = to_float_array(c.clutch);
    cached_channels["brakeBias"] = to_float_array_pct(c.brakeBias);
    cached_channels["localVelocity_x"] = to_float_array(c.localVelocity_x);
    cached_channels["localVelocity_y"] = to_float_array(c.localVelocity_y);
    cached_channels["localVelocity_z"] = to_float_array(c.localVelocity_z);
    
    cached_channels["carDamage_0"] = to_float_array(c.carDamage_0);
    cached_channels["carDamage_1"] = to_float_array(c.carDamage_1);
    cached_channels["carDamage_2"] = to_float_array(c.carDamage_2);
    cached_channels["carDamage_3"] = to_float_array(c.carDamage_3);
    cached_channels["carDamage_4"] = to_float_array(c.carDamage_4);
    cached_channels["numberOfTyresOut"] = to_int_array(c.numberOfTyresOut);

    // graphic
    cached_channels["status"] = to_int_array(c.status);
    cached_channels["session"] = to_int_array(c.session);
    cached_channels["position"] = to_int_array(c.position);
    cached_channels["sessionTimeLeft"] = to_float_array(c.sessionTimeLeft);
    cached_channels["isInPit"] = to_int_array(c.isInPit);
    cached_channels["currentSectorIndex"] = to_int_array(c.currentSectorIndex);
    cached_channels["lastSectorTime"] = to_int_array(c.lastSectorTime);
    cached_channels["carCoordinates_x"] = to_float_array(c.carCoordinates_x);
    cached_channels["carCoordinates_y"] = to_float_array(c.carCoordinates_y);
    cached_channels["carCoordinates_z"] = to_float_array(c.carCoordinates_z);
    cached_channels["penaltyTime"] = to_float_array(c.penaltyTime);
    cached_channels["flag"] = to_int_array(c.flag);
    cached_channels["idealLineOn"] = to_int_array(c.idealLineOn);
    cached_channels["isInPitLane"] = to_int_array(c.isInPitLane);
    cached_channels["surfaceGrip"] = to_float_array(c.surfaceGrip);
    cached_channels["mandatoryPitDone"] = to_int_array(c.mandatoryPitDone);
    cached_channels["windSpeed"] = to_float_array(c.windSpeed);
    cached_channels["windDirection"] = to_float_array(c.windDirection);
}

Dictionary GDLapTelemetry::get_channels() const {
    return cached_channels;
}

float GDLapTelemetry::get_channel_average(const StringName &p_channel) const {
    if (!cached_channels.has(p_channel)) return 0.0f;
    Variant v = cached_channels[p_channel];
    
    if (v.get_type() == Variant::PACKED_FLOAT32_ARRAY) {
        PackedFloat32Array arr = v;
        if (arr.is_empty()) return 0.0f;
        const float *ptr = arr.ptr();
        double sum = 0.0;
        for (int i = 0; i < arr.size(); ++i) {
            sum += ptr[i];
        }
        return (float)(sum / arr.size());
    } 
    else if (v.get_type() == Variant::PACKED_INT32_ARRAY) {
        PackedInt32Array arr = v;
        if (arr.is_empty()) return 0.0f;
        const int32_t *ptr = arr.ptr();
        double sum = 0.0;
        for (int i = 0; i < arr.size(); ++i) {
            sum += ptr[i];
        }
        return (float)(sum / arr.size());
    }
    
    return 0.0f;
}