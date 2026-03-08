#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include "telemetry_data_structs.h"
#include "telemetry_enums.h"

namespace godot {
    class GDTelemetrySnapshot : public RefCounted {
        GDCLASS(GDTelemetrySnapshot, RefCounted)

    protected:
        static void _bind_methods();

    public:
        GDTelemetrySnapshot();
        ~GDTelemetrySnapshot();

        void fill_from_snapshot(const TelemetrySnapshot &s);

        // variables

        double snapshot_timestamp = 0;
        
        int physics_packetId = 0;
        float physics_gas = 0;
        float physics_brake = 0;
        float physics_fuel = 0;
        int physics_gear = 0;
        int physics_rpms = 0;
        float physics_steerAngle = 0;
        float physics_speedKmh = 0;
        Vector3 physics_velocity;
        Vector3 physics_accG;
        Vector4 physics_wheelSlip;
        Vector4 physics_wheelLoad;
        Vector4 physics_wheelsPressure;
        Vector4 physics_wheelAngularSpeed;
        Vector4 physics_tyreWear;
        Vector4 physics_tyreDirtyLevel;
        Vector4 physics_tyreCoreTemperature;
        Vector4 physics_camberRAD;
        Vector4 physics_suspensionTravel;
        float physics_drs = 0;
        float physics_tc = 0;
        float physics_heading = 0;
        float physics_pitch = 0;
        float physics_roll = 0;
        float physics_cgHeight;
        Vector4 physics_carDamage;
        int physics_numberOfTyresOut = 0;
        int physics_pitLimiterOn = 0;
        float physics_abs = 0;
        float physics_kersCharge = 0;
        float physics_kersInput = 0;
        int physics_autoShifterOn = 0;
        Vector2 physics_rideHeight;
        float physics_turboBoost = 0;
        float physics_ballast = 0;
        float physics_airDensity = 0;

        float physics_airTemp = 0;
        float physics_roadTemp = 0;
        Vector3 physics_localAngularVel;
        float physics_finalFF = 0;
        float physics_performanceMeter = 0;
        int physics_engineBrake = 0;
        int physics_ersRecoveryLevel = 0;
        int physics_ersPowerLevel = 0;
        int physics_ersHeatCharging = 0;
        int physics_ersIsCharging = 0;
        float physics_kersCurrentKJ = 0;
        int physics_drsAvailable = 0;
        int physics_drsEnabled = 0;
        Vector4 physics_brakeTemp;
        float physics_clutch = 0;
        Vector4 physics_tyreTempI;
        Vector4 physics_tyreTempM;
        Vector4 physics_tyreTempO;
        int physics_isAIControlled;
        PackedVector3Array physics_tyreContactPoint;
        PackedVector3Array physics_tyreContactNormal;
        PackedVector3Array physics_tyreContactHeading;
        float physics_brakeBias;
        Vector3 physics_localVelocity;

        int graphic_packetId = 0;
        // ACStatusType graphic_status = ACStatusType::STATUS_OFF;
        ACSessionType graphic_session = ACSessionType::SESSION_UNKNOWN;
        String graphic_currentTime;
        String graphic_lastTime;
        String graphic_bestTime;
        String graphic_split;
        int graphic_completedLaps = 0;
        int graphic_position = 0;
        int graphic_iCurrentTime = 0;
        int graphic_iLastTime = 0;
        int graphic_iBestTime = 0;
        float graphic_sessionTimeLeft = 0;
        float graphic_distanceTraveled = 0;
        int graphic_isInPit = 0;
        int graphic_currentSectorIndex = 0;
        int graphic_lastSectorTime = 0;
        int graphic_numberOfLaps = 0;
        String graphic_tyreCompound;
        // float graphic_replayTimeMultiplier = 0;
        float graphic_normalizedCarPosition = 0;
        Vector3 graphic_carCoordinates;
        float graphic_penaltyTime = 0;
        ACFlagType graphic_flag = ACFlagType::FLAG_NONE;
        // int graphic_idealLineOn = 0;
        int graphic_isInPitLane = 0;
        float graphic_surfaceGrip = 0;

        int graphic_mandatoryPitDone = 0;
        float graphic_windSpeed = 0;
        float graphic_windDirection = 0;

        // getters

        double get_snapshot_timestamp() const;
        
        int get_physics_packetId() const;
        float get_physics_gas() const;
        float get_physics_brake() const;
        float get_physics_fuel() const;
        int get_physics_gear() const;
        int get_physics_rpms() const;
        float get_physics_steerAngle() const;
        float get_physics_speedKmh() const;
        Vector3 get_physics_velocity() const;
        Vector3 get_physics_accG() const;
        Vector4 get_physics_wheelSlip() const;
        Vector4 get_physics_wheelLoad() const;
        Vector4 get_physics_wheelsPressure() const;
        Vector4 get_physics_wheelAngularSpeed() const;
        Vector4 get_physics_tyreWear() const;
        Vector4 get_physics_tyreDirtyLevel() const;
        Vector4 get_physics_tyreCoreTemperature() const;
        Vector4 get_physics_camberRAD() const;
        Vector4 get_physics_suspensionTravel() const;
        float get_physics_drs() const;
        float get_physics_tc() const;
        float get_physics_heading() const;
        float get_physics_pitch() const;
        float get_physics_roll() const;
        float get_physics_cgHeight() const;
        Vector4 get_physics_carDamage() const;
        int get_physics_numberOfTyresOut() const;
        int get_physics_pitLimiterOn() const;
        float get_physics_abs() const;
        float get_physics_kersCharge() const;
        float get_physics_kersInput() const;
        int get_physics_autoShifterOn() const;
        Vector2 get_physics_rideHeight() const;
        float get_physics_turboBoost() const;
        float get_physics_ballast() const;
        float get_physics_airDensity() const;

        float get_physics_airTemp() const;
        float get_physics_roadTemp() const;
        Vector3 get_physics_localAngularVel() const;
        float get_physics_finalFF() const;
        float get_physics_performanceMeter() const;
        int get_physics_engineBrake() const;
        int get_physics_ersRecoveryLevel() const;
        int get_physics_ersPowerLevel() const;
        int get_physics_ersHeatCharging() const;
        int get_physics_ersIsCharging() const;
        float get_physics_kersCurrentKJ() const;
        int get_physics_drsAvailable() const;
        int get_physics_drsEnabled() const;
        Vector4 get_physics_brakeTemp() const;
        float get_physics_clutch() const;
        Vector4 get_physics_tyreTempI() const;
        Vector4 get_physics_tyreTempM() const;
        Vector4 get_physics_tyreTempO() const;
        int get_physics_isAIControlled() const;
        PackedVector3Array get_physics_tyreContactPoint() const;
        PackedVector3Array get_physics_tyreContactNormal() const;
        PackedVector3Array get_physics_tyreContactHeading() const;
        float get_physics_brakeBias() const;
        Vector3 get_physics_localVelocity() const;

        int get_graphic_packetId() const;
        // int get_graphic_status() const;
        int get_graphic_session() const;
        String get_graphic_currentTime() const;
        String get_graphic_lastTime() const;
        String get_graphic_bestTime() const;
        String get_graphic_split() const;
        int get_graphic_completedLaps() const;
        int get_graphic_position() const;
        int get_graphic_iCurrentTime() const;
        int get_graphic_iLastTime() const;
        int get_graphic_iBestTime() const;
        float get_graphic_sessionTimeLeft() const;
        float get_graphic_distanceTraveled() const;
        int get_graphic_isInPit() const;
        int get_graphic_currentSectorIndex() const;
        int get_graphic_lastSectorTime() const;
        int get_graphic_numberOfLaps() const;
        String get_graphic_tyreCompound() const;
        float get_graphic_replayTimeMultiplier() const;
        float get_graphic_normalizedCarPosition() const;
        Vector3 get_graphic_carCoordinates() const;
        float get_graphic_penaltyTime() const;
        int get_graphic_flag() const;
        int get_graphic_idealLineOn() const;
        int get_graphic_isInPitLane() const;
        float get_graphic_surfaceGrip() const;

        int get_graphic_mandatoryPitDone() const;
        float get_graphic_windSpeed() const;
        float get_graphic_windDirection() const;
    };
}
