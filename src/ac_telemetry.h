#pragma once

#include <godot_cpp/classes/node.hpp>
#include "telemetry_data_structs.h"
#include "telemetry_enums.h"
#include "gd_lap_telemetry.h"
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
        double samples_per_meter = 1.0;
        double last_recorded_meter = -1.0;
        String session_output_file_path = "";
        
        std::vector<LapDataChannels> sessions_data;
        std::vector<LapDataChannels> loaded_session_data;
        SPageStatic loaded_session_static_data;
        double loaded_session_sample_interval = 0.0;
        double loaded_session_samples_per_meter = 0.0;
        int loaded_session_lap_count = -1;
        std::vector<uint64_t> loaded_session_lap_offsets;

        String save_file_signature = "ACTL"; 

        int last_lap_count = 0;
        int last_physics_packet_id = -1;
        int last_i_current_time = 0;
        bool is_connected = false;
        std::atomic<bool> is_logging{false};
        std::atomic<bool> game_disconnected{false};

        std::thread logging_thread;
        std::mutex data_mutex;

        void logging_loop();
        
        String _open_session_file(const String& file_path, std::ifstream& infile, SPageStatic& out_static, double& out_sample_interval, double& out_samples_per_meter, uint64_t& out_lap_count, std::vector<uint64_t>& out_lap_offsets);
        Dictionary _calculate_session_metadata(const SPageStatic& stat, uint64_t count, const std::vector<LapDataChannels>& laps);

    protected:
        static void _bind_methods();

    public:
        ACTelemetry();
        ~ACTelemetry();


        void _process(double delta) override;
        
        bool is_connected_to_ac() const;
        bool is_currently_logging() const;
        int get_ac_status();

        String get_save_file_signature() const;
        void set_save_file_signature(String p_signature);

        double get_sample_interval() const;
        void set_sample_interval(double p_sample_interval);

        double get_samples_per_meter() const;
        void set_samples_per_meter(double p_samples_per_meter);

        String connect_to_ac();
        void disconnect_from_ac();
        
        // output_file_path (required): file where telemetry will be saved if AC is turned off without finish_logging()
        // it must be a valid file path
        String start_logging(String output_file_path);
        // output_file_path (optional): file where telemetry will be saved
        // if it's not a valid file path, the file path where the start_logging() function is called will be used
        String finish_logging(String output_file_path = "");

        Dictionary get_live_static_data();
        Ref<GDLapTelemetry> get_live_snapshot();

        String load_session_data(String file_path);
        Dictionary get_session_metadata(String file_path);
        Dictionary get_loaded_session_metadata();
        Ref<GDLapTelemetry> get_loaded_session_lap_data(int lap_index);
        Dictionary get_loaded_session_lap_stats(int lap_index);
        Dictionary get_loaded_session_static_data();
        int get_loaded_session_lap_count();
        double get_loaded_session_sample_interval();
        double get_loaded_session_samples_per_meter();
        double get_loaded_session_lap_fuel_consumption(int lap_index);
        double get_loaded_session_total_fuel_consumption();
        double get_loaded_session_total_laps();
        void close_loaded_session();
        
        PackedFloat32Array calculate_lap_time_delta(String target_file_path, int target_lap_index = 0, String current_file_path = "", int current_lap_index = 0, PackedFloat32Array reference_positions = PackedFloat32Array());

        static Dictionary _static_to_dict(const SPageStatic &s);
    };

}

VARIANT_ENUM_CAST(ACStatusType);
VARIANT_ENUM_CAST(ACSessionType);
VARIANT_ENUM_CAST(ACFlagType);