#pragma once

#include <godot_cpp/classes/node.hpp>
#include "telemetry_data_structs.h"
#include "telemetry_enums.h"
#include "gd_telemetry_snapshot.h"
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>

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
        String session_output_file_path = "";
        
        std::vector<std::vector<TelemetrySnapshot>> sessions_data;
        std::vector<std::vector<TelemetrySnapshot>> loaded_session_data;
        SPageStatic loaded_session_static_data;
        double loaded_session_sample_interval = 0.0;
        int loaded_session_lap_count = -1;

        String save_file_signature = "ACTL"; 

        int last_lap_count = 0;
        int last_physics_packet_id = -1;
        int last_i_current_time = 0;
        bool is_connected = false;
        std::atomic<bool> is_logging{false};

        std::thread logging_thread;
        std::mutex data_mutex;

        void logging_loop();

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
        
        // output_file_path (required): file where telemetry will be saved if AC is turned off without finish_logging()
        // it must be a valid file path
        String start_logging(String output_file_path);
        // output_file_path (optional): file where telemetry will be saved
        // if it's not a valid file path, the file path where the start_logging() function is called will be used
        String finish_logging(String output_file_path = "");

        Dictionary get_live_static_data();
        Ref<GDTelemetrySnapshot> get_live_snapshot();

        String load_session_data(String file_path);
        TypedArray<GDTelemetrySnapshot> get_loaded_session_lap_data(int lap_index);
        Dictionary get_loaded_session_lap_stats(int lap_index);
        Dictionary get_loaded_session_static_data();
        int get_loaded_session_lap_count();
        double get_loaded_session_sample_interval();
        void close_loaded_session();
        
        static Dictionary _static_to_dict(const SPageStatic &s);
    };

}

VARIANT_ENUM_CAST(ACStatusType);
VARIANT_ENUM_CAST(ACSessionType);
VARIANT_ENUM_CAST(ACFlagType);