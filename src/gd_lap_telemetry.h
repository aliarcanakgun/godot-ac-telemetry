#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/dictionary.hpp>

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

        void set_channels(const Dictionary &p_channels);

        Dictionary get_channels() const;
        float get_channel_average(const StringName &p_channel) const;
    };
}
