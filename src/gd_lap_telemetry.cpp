#include "gd_lap_telemetry.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>

using namespace godot;

GDLapTelemetry::GDLapTelemetry() {}
GDLapTelemetry::~GDLapTelemetry() {}

void GDLapTelemetry::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_channels", "p_channels"), &GDLapTelemetry::set_channels);
    ClassDB::bind_method(D_METHOD("get_channels"), &GDLapTelemetry::get_channels);
    ClassDB::bind_method(D_METHOD("get_channel_average", "channel_name"), &GDLapTelemetry::get_channel_average);
}

void GDLapTelemetry::set_channels(const Dictionary &p_channels) {
    cached_channels = p_channels;
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