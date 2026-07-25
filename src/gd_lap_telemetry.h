#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include "telemetry_data_structs.h"

namespace godot {
    class GDLapTelemetry : public RefCounted {
        GDCLASS(GDLapTelemetry, RefCounted)

    private:
        Dictionary cached_channels;
        float samples_per_meter = 1.0f;

    protected:
        static void _bind_methods();

    public:
        GDLapTelemetry();
        ~GDLapTelemetry();

        void set_samples_per_meter(float p_spm);
        void fill_from_channels(const LapDataChannels &channels);

        Dictionary get_channels() const;
        float get_channel_average(const StringName &p_channel) const;
    };
}
