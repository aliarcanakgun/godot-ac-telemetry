#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include "telemetry_data_structs.h"

namespace godot {
    class GDLapTelemetry : public RefCounted {
        GDCLASS(GDLapTelemetry, RefCounted)

    private:
        Dictionary cached_channels;

    protected:
        static void _bind_methods();

    public:
        GDLapTelemetry();
        ~GDLapTelemetry();

        void fill_from_channels(const LapDataChannels &channels);

        Dictionary get_channels() const;
    };
}
