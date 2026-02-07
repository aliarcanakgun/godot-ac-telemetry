# Godot AC Telemetry Extension (WIP)

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

### Known issues:
* **This extension is not production-ready yet,** but the core system is implemented.

### Public functions & properties:
* **This list will be updated soon...**
* `connect_to_ac()`: Attempts to open and map Assetto Corsa's shared memory pages. Returns "" on success or an error string on failure.
* `disconnect_from_ac()`: Unmaps and closes any shared-memory handles and marks the node disconnected.
* `start_logging()`: Clears previous session data and begins collecting telemetry snapshots into in-memory per-lap vectors.
* `finish_logging()`: Stops logging and writes all collected lap snapshots into last_session_data.bin (binary). Returns the output filename or error string.
* `get_speed()`: Convenience accessor returning the latest speedKmh from the physics page (or 0.0 if not available).
* `is_connected_to_ac()`: Property/getter — returns whether shared-memory mappings are currently active.
* `is_currently_logging()`: Property/getter — returns whether the node is collecting telemetry snapshots.
* `sample_interval`: Property for the polling interval (default 0.02 seconds). Exposed as the sample_interval property in the Godot inspector.
* Signal `connection_lost`: Emitted when a previously valid mapping becomes invalid (used to notify the game that telemetry source was disconnected).

### Data & file notes:

* Telemetry structures:: `SPagePhysics`, `SPageGraphic` and `SPageStatic` match the memory layout expected from Assetto Corsa (packed with 4-byte alignment).
* Output file: Contains serialized `TelemetrySnapshot` sequences organized by laps. Each snapshot captures `SPagePhysics` and `SPageGraphic` data at every `sample_interval`. Snapshots are buffered into a lap-specific vector, which are then stored in a session-wide "main" vector. The entire session is flushed to the binary file only when logging is finished. An example is in the demo project.
* Platform:: Current implementation uses Win32 APIs (OpenFileMapping, MapViewOfFile, FormatMessage, etc.) and is Windows-only. For cross-platform usage, the IPC layer must be refactored.
