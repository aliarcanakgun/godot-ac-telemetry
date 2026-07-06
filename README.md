# Godot AC Telemetry Extension

### Requirements:
* Godot Engine 4.5+
* Compatible with Windows only

### How to compile:
* First, you need SCons installed on your PC (you can install it using `python -m pip install --user scons`)
* Then, clone the repository using:
```bash
git clone --recurse-submodules https://github.com/aliarcanakgun/godot-ac-telemetry.git
cd godot-ac-telemetry
```
Before building the extension, you must compile the godot-cpp library. Navigate to the submodule folder and run SCons:
```bash
cd godot-cpp
scons platform=windows target=template_debug # or template_release
cd ..
```
* Finally, in the root folder, use the commands below to build the extension:
```bash
# debug build
scons platform=windows target=template_debug

# release build
scons platform=windows target=template_release
```

### Features:
* **Thread-Safe Background Logging:** Telemetry is polled safely in a background thread, preventing data loss during stutters and avoiding performance hits on the main Godot thread.
* **Channel-Based Data Architecture:** Data is organized into channels (arrays of values over time or distance), making it drastically easier to plot graphs or run analytics in GDScript.
* **Distance-Based & Time-Based Sampling:** Accurate distance-normalized telemetry alongside time-based intervals.
* **Advanced Analytics & Stats:** Calculate lap deltas, track fuel consumption, and retrieve extensive session metadata.
* **Live Snapshots & Session Handling:** Retrieve real-time dashboards via `get_live_snapshot()`, and safely save sessions on unexpected disconnects.

### Basic Usage Example (GDScript):
```gdscript
extends Node

var telemetry: ACTelemetry

func _ready():
    telemetry = ACTelemetry.new()
    add_child(telemetry)
    
    var err = telemetry.connect_to_ac()
    if err == "":
        print("Connected to Assetto Corsa!")
        # a valid output path is required as a fallback in case of a crash/disconnect
        telemetry.start_logging("user://telemetry_backup.actl")
    else:
        print("Failed to connect: ", err)

func _process(delta):
    if telemetry.is_connected_to_ac():
        # get live data for dashboards
        var live_data = telemetry.get_live_snapshot()
        var speed_channel = live_data.get_channels().get("physics_speedKmh", [])
        if speed_channel.size() > 0:
            print("Current Speed: ", speed_channel[-1], " km/h")

func _exit_tree():
    if telemetry.is_currently_logging():
        var save_path = telemetry.finish_logging("user://telemetry_session.actl")
        print("Session saved to: ", save_path)
    telemetry.disconnect_from_ac()
```

### Public functions & properties:
* `connect_to_ac() -> String`: Establishes a connection to Assetto Corsa via shared memory. Returns `""` on success or an error string on failure.
* `disconnect_from_ac() -> void`: Closes all shared memory connections.
* `start_logging(output_file_path: String) -> String`: Starts background telemetry logging. `output_file_path` is required as a fallback path in case of crash/disconnect. Returns `""` on success or an error string.
* `finish_logging(output_file_path: String = "") -> String`: Stops logging and saves the telemetry session. If `output_file_path` is left empty, the fallback path provided in `start_logging` is used. Returns the saved file path or an error string.
* `is_connected_to_ac() -> bool`: Returns whether shared-memory mappings are currently active.
* `is_currently_logging() -> bool`: Returns whether the background logging thread is running.
* `get_live_static_data() -> Dictionary`: Returns static track and car info for the live session.
* `get_live_snapshot() -> GDLapTelemetry`: Returns the most recent telemetry snapshot as a `GDLapTelemetry` object (contains single-point arrays in channels).
* `load_session_data(file_path: String) -> String`: Loads a previously saved telemetry session. Returns `""` on success or an error string.
* `get_session_metadata(file_path: String) -> Dictionary`: Parses session metadata (lap count, session static data, sample rates) without loading the entire telemetry data into memory.
* `get_loaded_session_lap_data(lap_index: int) -> GDLapTelemetry`: Returns all channel data for the specified lap as a `GDLapTelemetry` object.
* `get_loaded_session_lap_stats(lap_index: int) -> Dictionary`: Returns key statistics for a lap (top speed, min speed, sector times, average values).
* `get_loaded_session_static_data() -> Dictionary`: Returns static data of the loaded session.
* `get_loaded_session_lap_count() -> int`: Returns the total number of laps recorded in the loaded session.
* `get_loaded_session_sample_interval() -> float`: Returns the time-based sample interval used for the session.
* `get_loaded_session_samples_per_meter() -> float`: Returns the distance-based sampling rate used for the session.
* `get_loaded_session_lap_fuel_consumption(lap_index: int) -> float`: Calculates fuel consumed (in liters) during the specified lap.
* `get_loaded_session_total_fuel_consumption() -> float`: Calculates total fuel consumed (in liters) across the loaded session.
* `get_loaded_session_total_laps() -> float`: Returns the exact float value of total laps completed (including the incomplete final lap).
* `close_loaded_session() -> void`: Closes the loaded session and frees its memory.
* `calculate_lap_time_delta(target_file_path: String, target_lap_index: int = 0, current_file_path: String = "", current_lap_index: int = 0, reference_positions: PackedFloat32Array = PackedFloat32Array()) -> PackedFloat32Array`: Calculates the time delta (in seconds) between two laps across distance.
* `sample_interval: float`: Property for the time interval between telemetry samples (seconds).
* `samples_per_meter: float`: Property for the distance interval between telemetry samples (meters). Overrides `sample_interval` if > 0.
* `save_file_signature: String`: Property for the signature string written at the beginning of the binary save file (default `"ACTL"`).
* Signal `connection_lost`: Emitted when the shared memory connection is unexpectedly lost.

### Data & file notes:
* **Channel-Based Architecture:** Telemetry is now organized into `GDLapTelemetry` objects containing channels (arrays of values over time/distance) instead of raw struct snapshots. This makes the data drastically easier to graph and analyze within GDScript.
* Output file: Contains serialized telemetry channels organized by laps. The entire session is flushed to the binary file only when logging is finished or connection is lost.
* Supported Sims: Telemetry is developed for original Assetto Corsa. Multi-sim support is planned for the future (in a different branch).
* Platform: Current implementation uses Win32 APIs (OpenFileMapping, MapViewOfFile, FormatMessage, etc.) and is Windows-only.
