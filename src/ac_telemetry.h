#pragma once

#include <godot_cpp/classes/node.hpp>
#include "telemetry_data_structs.h"
#include "gd_telemetry_snapshot.h"
#include <windows.h>
#include <vector>

namespace godot {
    class ACTelemetry : public Node {
        GDCLASS(ACTelemetry, Node)

    private:
        // shared memory handles
        HANDLE hMapPhysics;
        HANDLE hMapGraphic;
        HANDLE hMapStatic;

        // shared memory pointers
        SPagePhysics* dataPhysics;
        SPageGraphic* dataGraphic;
        SPageStatic* dataStatic;

        // logging logic
        double sample_interval = 0.02; // 50 hz
        double session_time = 0.0;
        double accum = 0.0;
        
        std::vector<std::vector<TelemetrySnapshot>> sessions_data;
        std::vector<std::vector<TelemetrySnapshot>> loaded_session_data;
        SPageStatic loaded_session_static_data;
        double loaded_session_sample_interval = 0.0;
        int loaded_session_lap_count = -1;

        String save_file_signature = "ACTL"; 

        int last_lap_count = 0;
        bool is_connected = false;
        bool is_logging = false;

    protected:
        static void _bind_methods();

    public:
        ACTelemetry();
        ~ACTelemetry();

        void _process(double delta) override;
        
        bool is_connected_to_ac() const; // getter
        bool is_currently_logging() const; // getter

        String get_save_file_signature() const; // getter
        void set_save_file_signature(String p_signature); // setter

        double get_sample_interval() const; // getter
        void set_sample_interval(double p_sample_interval); // setter

        String connect_to_ac();
        void disconnect_from_ac();
        
        void start_logging();
        String finish_logging(String output_file_path);

        String load_session_data(String file_path);
        TypedArray<GDTelemetrySnapshot> get_loaded_session_lap_data(int lap_index);
        Dictionary get_loaded_session_static_data();
        int get_loaded_session_lap_count();
        double get_loaded_session_sample_interval();
        void close_loaded_session();

        static Dictionary _snapshot_to_dict(const TelemetrySnapshot &s);
        static Dictionary _physics_to_dict(const SPagePhysics &p);
        static Dictionary _graphic_to_dict(const SPageGraphic &g);
        static Dictionary _static_to_dict(const SPageStatic &s);
    };

}