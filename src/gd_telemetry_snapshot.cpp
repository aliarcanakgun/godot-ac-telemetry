#include "gd_telemetry_snapshot.h"
#include "telemetry_data_structs.h"
#include "helper.h"

using namespace godot;

GDTelemetrySnapshot::GDTelemetrySnapshot() {

}

GDTelemetrySnapshot::~GDTelemetrySnapshot() {
    
}

void GDTelemetrySnapshot::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_snapshot_timestamp"), &GDTelemetrySnapshot::get_snapshot_timestamp);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "snapshot_timestamp"), "", "get_snapshot_timestamp");

    ClassDB::bind_method(D_METHOD("get_physics_packetId"), &GDTelemetrySnapshot::get_physics_packetId);
    ClassDB::bind_method(D_METHOD("get_physics_gas"), &GDTelemetrySnapshot::get_physics_gas);
    ClassDB::bind_method(D_METHOD("get_physics_brake"), &GDTelemetrySnapshot::get_physics_brake);
    ClassDB::bind_method(D_METHOD("get_physics_fuel"), &GDTelemetrySnapshot::get_physics_fuel);
    ClassDB::bind_method(D_METHOD("get_physics_gear"), &GDTelemetrySnapshot::get_physics_gear);
    ClassDB::bind_method(D_METHOD("get_physics_rpms"), &GDTelemetrySnapshot::get_physics_rpms);
    ClassDB::bind_method(D_METHOD("get_physics_steerAngle"), &GDTelemetrySnapshot::get_physics_steerAngle);
    ClassDB::bind_method(D_METHOD("get_physics_speedKmh"), &GDTelemetrySnapshot::get_physics_speedKmh);
    ClassDB::bind_method(D_METHOD("get_physics_velocity"), &GDTelemetrySnapshot::get_physics_velocity);
    ClassDB::bind_method(D_METHOD("get_physics_accG"), &GDTelemetrySnapshot::get_physics_accG);
    ClassDB::bind_method(D_METHOD("get_physics_wheelSlip"), &GDTelemetrySnapshot::get_physics_wheelSlip);
    ClassDB::bind_method(D_METHOD("get_physics_wheelLoad"), &GDTelemetrySnapshot::get_physics_wheelLoad);
    ClassDB::bind_method(D_METHOD("get_physics_wheelsPressure"), &GDTelemetrySnapshot::get_physics_wheelsPressure);
    ClassDB::bind_method(D_METHOD("get_physics_wheelAngularSpeed"), &GDTelemetrySnapshot::get_physics_wheelAngularSpeed);
    ClassDB::bind_method(D_METHOD("get_physics_tyreWear"), &GDTelemetrySnapshot::get_physics_tyreWear);
    ClassDB::bind_method(D_METHOD("get_physics_tyreDirtyLevel"), &GDTelemetrySnapshot::get_physics_tyreDirtyLevel);
    ClassDB::bind_method(D_METHOD("get_physics_tyreCoreTemperature"), &GDTelemetrySnapshot::get_physics_tyreCoreTemperature);
    ClassDB::bind_method(D_METHOD("get_physics_camberRAD"), &GDTelemetrySnapshot::get_physics_camberRAD);
    ClassDB::bind_method(D_METHOD("get_physics_suspensionTravel"), &GDTelemetrySnapshot::get_physics_suspensionTravel);
    ClassDB::bind_method(D_METHOD("get_physics_drs"), &GDTelemetrySnapshot::get_physics_drs);
    ClassDB::bind_method(D_METHOD("get_physics_tc"), &GDTelemetrySnapshot::get_physics_tc);
    ClassDB::bind_method(D_METHOD("get_physics_heading"), &GDTelemetrySnapshot::get_physics_heading);
    ClassDB::bind_method(D_METHOD("get_physics_pitch"), &GDTelemetrySnapshot::get_physics_pitch);
    ClassDB::bind_method(D_METHOD("get_physics_roll"), &GDTelemetrySnapshot::get_physics_roll);
    ClassDB::bind_method(D_METHOD("get_physics_cgHeight"), &GDTelemetrySnapshot::get_physics_cgHeight);
    ClassDB::bind_method(D_METHOD("get_physics_carDamage"), &GDTelemetrySnapshot::get_physics_carDamage);
    ClassDB::bind_method(D_METHOD("get_physics_numberOfTyresOut"), &GDTelemetrySnapshot::get_physics_numberOfTyresOut);
    ClassDB::bind_method(D_METHOD("get_physics_pitLimiterOn"), &GDTelemetrySnapshot::get_physics_pitLimiterOn);
    ClassDB::bind_method(D_METHOD("get_physics_abs"), &GDTelemetrySnapshot::get_physics_abs);
    ClassDB::bind_method(D_METHOD("get_physics_kersCharge"), &GDTelemetrySnapshot::get_physics_kersCharge);
    ClassDB::bind_method(D_METHOD("get_physics_kersInput"), &GDTelemetrySnapshot::get_physics_kersInput);
    ClassDB::bind_method(D_METHOD("get_physics_autoShifterOn"), &GDTelemetrySnapshot::get_physics_autoShifterOn);
    ClassDB::bind_method(D_METHOD("get_physics_rideHeight"), &GDTelemetrySnapshot::get_physics_rideHeight);
    ClassDB::bind_method(D_METHOD("get_physics_turboBoost"), &GDTelemetrySnapshot::get_physics_turboBoost);
    ClassDB::bind_method(D_METHOD("get_physics_ballast"), &GDTelemetrySnapshot::get_physics_ballast);
    ClassDB::bind_method(D_METHOD("get_physics_airDensity"), &GDTelemetrySnapshot::get_physics_airDensity);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "physics_packetId"), "", "get_physics_packetId");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "physics_gas"), "", "get_physics_gas");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "physics_brake"), "", "get_physics_brake");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "physics_fuel"), "", "get_physics_fuel");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "physics_gear"), "", "get_physics_gear");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "physics_rpms"), "", "get_physics_rpms");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "physics_steerAngle"), "", "get_physics_steerAngle");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "physics_speedKmh"), "", "get_physics_speedKmh");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "physics_velocity"), "", "get_physics_velocity");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "physics_accG"), "", "get_physics_accG");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR4, "physics_wheelSlip"), "", "get_physics_wheelSlip");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR4, "physics_wheelLoad"), "", "get_physics_wheelLoad");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR4, "physics_wheelsPressure"), "", "get_physics_wheelsPressure");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR4, "physics_wheelAngularSpeed"), "", "get_physics_wheelAngularSpeed");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR4, "physics_tyreWear"), "", "get_physics_tyreWear");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR4, "physics_tyreDirtyLevel"), "", "get_physics_tyreDirtyLevel");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR4, "physics_tyreCoreTemperature"), "", "get_physics_tyreCoreTemperature");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR4, "physics_camberRAD"), "", "get_physics_camberRAD");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR4, "physics_suspensionTravel"), "", "get_physics_suspensionTravel");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "physics_drs"), "", "get_physics_drs");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "physics_tc"), "", "get_physics_tc");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "physics_heading"), "", "get_physics_heading");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "physics_pitch"), "", "get_physics_pitch");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "physics_roll"), "", "get_physics_roll");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "physics_cgHeight"), "", "get_physics_cgHeight");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR4, "physics_carDamage"), "", "get_physics_carDamage");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "physics_numberOfTyresOut"), "", "get_physics_numberOfTyresOut");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "physics_pitLimiterOn"), "", "get_physics_pitLimiterOn");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "physics_abs"), "", "get_physics_abs");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "physics_kersCharge"), "", "get_physics_kersCharge");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "physics_kersInput"), "", "get_physics_kersInput");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "physics_autoShifterOn"), "", "get_physics_autoShifterOn");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "physics_rideHeight"), "", "get_physics_rideHeight");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "physics_turboBoost"), "", "get_physics_turboBoost");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "physics_ballast"), "", "get_physics_ballast");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "physics_airDensity"), "", "get_physics_airDensity");



    ClassDB::bind_method(D_METHOD("get_graphic_packetId"), &GDTelemetrySnapshot::get_graphic_packetId);
    // ClassDB::bind_method(D_METHOD("get_graphic_status"), &GDTelemetrySnapshot::get_graphic_status);
    // ClassDB::bind_method(D_METHOD("get_graphic_session"), &GDTelemetrySnapshot::get_graphic_session);
    ClassDB::bind_method(D_METHOD("get_graphic_currentTime"), &GDTelemetrySnapshot::get_graphic_currentTime);
    ClassDB::bind_method(D_METHOD("get_graphic_lastTime"), &GDTelemetrySnapshot::get_graphic_lastTime);
    ClassDB::bind_method(D_METHOD("get_graphic_bestTime"), &GDTelemetrySnapshot::get_graphic_bestTime);
    ClassDB::bind_method(D_METHOD("get_graphic_split"), &GDTelemetrySnapshot::get_graphic_split);
    ClassDB::bind_method(D_METHOD("get_graphic_completedLaps"), &GDTelemetrySnapshot::get_graphic_completedLaps);
    ClassDB::bind_method(D_METHOD("get_graphic_position"), &GDTelemetrySnapshot::get_graphic_position);
    ClassDB::bind_method(D_METHOD("get_graphic_iCurrentTime"), &GDTelemetrySnapshot::get_graphic_iCurrentTime);
    ClassDB::bind_method(D_METHOD("get_graphic_iLastTime"), &GDTelemetrySnapshot::get_graphic_iLastTime);
    ClassDB::bind_method(D_METHOD("get_graphic_iBestTime"), &GDTelemetrySnapshot::get_graphic_iBestTime);
    ClassDB::bind_method(D_METHOD("get_graphic_sessionTimeLeft"), &GDTelemetrySnapshot::get_graphic_sessionTimeLeft);
    ClassDB::bind_method(D_METHOD("get_graphic_distanceTraveled"), &GDTelemetrySnapshot::get_graphic_distanceTraveled);
    ClassDB::bind_method(D_METHOD("get_graphic_isInPit"), &GDTelemetrySnapshot::get_graphic_isInPit);
    ClassDB::bind_method(D_METHOD("get_graphic_currentSectorIndex"), &GDTelemetrySnapshot::get_graphic_currentSectorIndex);
    ClassDB::bind_method(D_METHOD("get_graphic_lastSectorTime"), &GDTelemetrySnapshot::get_graphic_lastSectorTime);
    ClassDB::bind_method(D_METHOD("get_graphic_numberOfLaps"), &GDTelemetrySnapshot::get_graphic_numberOfLaps);
    ClassDB::bind_method(D_METHOD("get_graphic_tyreCompound"), &GDTelemetrySnapshot::get_graphic_tyreCompound);
    // ClassDB::bind_method(D_METHOD("get_graphic_replayTimeMultiplier"), &GDTelemetrySnapshot::get_graphic_replayTimeMultiplier);
    ClassDB::bind_method(D_METHOD("get_graphic_normalizedCarPosition"), &GDTelemetrySnapshot::get_graphic_normalizedCarPosition);
    ClassDB::bind_method(D_METHOD("get_graphic_carCoordinates"), &GDTelemetrySnapshot::get_graphic_carCoordinates);
    ClassDB::bind_method(D_METHOD("get_graphic_penaltyTime"), &GDTelemetrySnapshot::get_graphic_penaltyTime);
    ClassDB::bind_method(D_METHOD("get_graphic_flag"), &GDTelemetrySnapshot::get_graphic_flag);
    // ClassDB::bind_method(D_METHOD("get_graphic_idealLineOn"), &GDTelemetrySnapshot::get_graphic_idealLineOn);
    ClassDB::bind_method(D_METHOD("get_graphic_isInPitLane"), &GDTelemetrySnapshot::get_graphic_isInPitLane);
    ClassDB::bind_method(D_METHOD("get_graphic_surfaceGrip"), &GDTelemetrySnapshot::get_graphic_surfaceGrip);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "graphic_packetId"), "", "get_graphic_packetId");
    // ADD_PROPERTY(PropertyInfo(Variant::INT, "graphic_status"), "", "get_graphic_status");
    // ADD_PROPERTY(PropertyInfo(Variant::INT, "graphic_session"), "", "get_graphic_session");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "graphic_currentTime"), "", "get_graphic_currentTime");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "graphic_lastTime"), "", "get_graphic_lastTime");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "graphic_bestTime"), "", "get_graphic_bestTime");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "graphic_split"), "", "get_graphic_split");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "graphic_completedLaps"), "", "get_graphic_completedLaps");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "graphic_position"), "", "get_graphic_position");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "graphic_iCurrentTime"), "", "get_graphic_iCurrentTime");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "graphic_iLastTime"), "", "get_graphic_iLastTime");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "graphic_iBestTime"), "", "get_graphic_iBestTime");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "graphic_sessionTimeLeft"), "", "get_graphic_sessionTimeLeft");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "graphic_distanceTraveled"), "", "get_graphic_distanceTraveled");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "graphic_isInPit"), "", "get_graphic_isInPit");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "graphic_currentSectorIndex"), "", "get_graphic_currentSectorIndex");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "graphic_lastSectorTime"), "", "get_graphic_lastSectorTime");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "graphic_numberOfLaps"), "", "get_graphic_numberOfLaps");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "graphic_tyreCompound"), "", "get_graphic_tyreCompound");
    // ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "graphic_replayTimeMultiplier"), "", "get_graphic_replayTimeMultiplier");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "graphic_normalizedCarPosition"), "", "get_graphic_normalizedCarPosition");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "graphic_carCoordinates"), "", "get_graphic_carCoordinates");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "graphic_penaltyTime"), "", "get_graphic_penaltyTime");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "graphic_flag"), "", "get_graphic_flag");
    // ADD_PROPERTY(PropertyInfo(Variant::INT, "graphic_idealLineOn"), "", "get_graphic_idealLineOn");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "graphic_isInPitLane"), "", "get_graphic_isInPitLane");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "graphic_surfaceGrip"), "", "get_graphic_surfaceGrip");
}

void GDTelemetrySnapshot::fill_from_snapshot(const TelemetrySnapshot &s) {
    snapshot_timestamp = s.timestamp;

    // physics
    physics_packetId = s.physics.packetId;
    physics_gas = s.physics.gas;
    physics_brake = s.physics.brake;
    physics_fuel = s.physics.fuel;
    physics_gear = s.physics.gear;
    physics_rpms = s.physics.rpms;
    physics_steerAngle = s.physics.steerAngle;
    physics_speedKmh = s.physics.speedKmh;
    physics_velocity = Vector3(s.physics.velocity[0], s.physics.velocity[1], s.physics.velocity[2]);
    physics_accG = Vector3(s.physics.accG[0], s.physics.accG[1], s.physics.accG[2]);
    physics_wheelSlip = Vector4(s.physics.wheelSlip[0], s.physics.wheelSlip[1], s.physics.wheelSlip[2], s.physics.wheelSlip[3]);
    physics_wheelLoad = Vector4(s.physics.wheelLoad[0], s.physics.wheelLoad[1], s.physics.wheelLoad[2], s.physics.wheelLoad[3]);
    physics_wheelsPressure = Vector4(s.physics.wheelsPressure[0], s.physics.wheelsPressure[1], s.physics.wheelsPressure[2], s.physics.wheelsPressure[3]);
    physics_wheelAngularSpeed = Vector4(s.physics.wheelAngularSpeed[0], s.physics.wheelAngularSpeed[1], s.physics.wheelAngularSpeed[2], s.physics.wheelAngularSpeed[3]);
    physics_tyreWear = Vector4(s.physics.tyreWear[0], s.physics.tyreWear[1], s.physics.tyreWear[2], s.physics.tyreWear[3]);
    physics_tyreDirtyLevel = Vector4(s.physics.tyreDirtyLevel[0], s.physics.tyreDirtyLevel[1], s.physics.tyreDirtyLevel[2], s.physics.tyreDirtyLevel[3]);
    physics_tyreCoreTemperature = Vector4(s.physics.tyreCoreTemperature[0], s.physics.tyreCoreTemperature[1], s.physics.tyreCoreTemperature[2], s.physics.tyreCoreTemperature[3]);
    physics_camberRAD = Vector4(s.physics.camberRAD[0], s.physics.camberRAD[1], s.physics.camberRAD[2], s.physics.camberRAD[3]);
    physics_suspensionTravel = Vector4(s.physics.suspensionTravel[0], s.physics.suspensionTravel[1], s.physics.suspensionTravel[2], s.physics.suspensionTravel[3]);
    physics_drs = s.physics.drs;
    physics_tc = s.physics.tc;
    physics_heading = s.physics.heading;
    physics_pitch = s.physics.pitch;
    physics_roll = s.physics.roll;
    physics_cgHeight = s.physics.cgHeight;
    physics_carDamage = Vector4(s.physics.carDamage[0], s.physics.carDamage[1], s.physics.carDamage[2], s.physics.carDamage[3]); // actually it's a float[5] but first 4 is valid (according to net)
    physics_numberOfTyresOut = s.physics.numberOfTyresOut;
    physics_pitLimiterOn = s.physics.pitLimiterOn;
    physics_abs = s.physics.abs;
    physics_kersCharge = s.physics.kersCharge;
    physics_kersInput = s.physics.kersInput;
    physics_autoShifterOn = s.physics.autoShifterOn;
    physics_rideHeight = Vector2(s.physics.rideHeight[0], s.physics.rideHeight[1]);
    physics_turboBoost = s.physics.turboBoost;
    physics_ballast = s.physics.ballast;
    physics_airDensity = s.physics.airDensity;

    // graphic
    graphic_packetId = s.graphic.packetId;
    
    // graphic_status = (int)s.graphic.status;
    // graphic_session = (int)s.graphic.session;

    graphic_currentTime = wchar_to_gdstring(s.graphic.currentTime, 15);
    graphic_lastTime = wchar_to_gdstring(s.graphic.lastTime, 15);
    graphic_bestTime = wchar_to_gdstring(s.graphic.bestTime, 15);
    graphic_split = wchar_to_gdstring(s.graphic.split, 15);
    graphic_completedLaps = s.graphic.completedLaps;
    graphic_position = s.graphic.position;
    graphic_iCurrentTime = s.graphic.iCurrentTime;
    graphic_iLastTime = s.graphic.iLastTime;
    graphic_iBestTime = s.graphic.iBestTime;
    graphic_sessionTimeLeft = s.graphic.sessionTimeLeft;
    graphic_distanceTraveled = s.graphic.distanceTraveled;
    graphic_isInPit = s.graphic.isInPit;
    graphic_currentSectorIndex = s.graphic.currentSectorIndex;
    graphic_lastSectorTime = s.graphic.lastSectorTime;
    graphic_numberOfLaps = s.graphic.numberOfLaps;
    graphic_tyreCompound = wchar_to_gdstring(s.graphic.tyreCompound, 33);

    // graphic_replayTimeMultiplier = s.graphic.replayTimeMultiplier;

    graphic_normalizedCarPosition = s.graphic.normalizedCarPosition;
    graphic_carCoordinates = Vector3(s.graphic.carCoordinates[0], s.graphic.carCoordinates[1], s.graphic.carCoordinates[2]);
    graphic_penaltyTime = s.graphic.penaltyTime;
    graphic_flag = (int)s.graphic.flag;
    
    //graphic_idealLineOn = s.graphic.idealLineOn;

    graphic_isInPitLane = s.graphic.isInPitLane;
    graphic_surfaceGrip = s.graphic.surfaceGrip;
}


int GDTelemetrySnapshot::get_snapshot_timestamp() const { return snapshot_timestamp; }

int GDTelemetrySnapshot::get_physics_packetId() const { return physics_packetId; }
float GDTelemetrySnapshot::get_physics_gas() const { return physics_gas; }
float GDTelemetrySnapshot::get_physics_brake() const { return physics_brake; }
float GDTelemetrySnapshot::get_physics_fuel() const { return physics_fuel; }
int GDTelemetrySnapshot::get_physics_gear() const { return physics_gear; }
int GDTelemetrySnapshot::get_physics_rpms() const { return physics_rpms; }
float GDTelemetrySnapshot::get_physics_steerAngle() const { return physics_steerAngle; }
float GDTelemetrySnapshot::get_physics_speedKmh() const { return physics_speedKmh; }
Vector3 GDTelemetrySnapshot::get_physics_velocity() const { return physics_velocity; }
Vector3 GDTelemetrySnapshot::get_physics_accG() const { return physics_accG; }
Vector4 GDTelemetrySnapshot::get_physics_wheelSlip() const { return physics_wheelSlip; }
Vector4 GDTelemetrySnapshot::get_physics_wheelLoad() const { return physics_wheelLoad; }
Vector4 GDTelemetrySnapshot::get_physics_wheelsPressure() const { return physics_wheelsPressure; }
Vector4 GDTelemetrySnapshot::get_physics_wheelAngularSpeed() const { return physics_wheelAngularSpeed; }
Vector4 GDTelemetrySnapshot::get_physics_tyreWear() const { return physics_tyreWear; }
Vector4 GDTelemetrySnapshot::get_physics_tyreDirtyLevel() const { return physics_tyreDirtyLevel; }
Vector4 GDTelemetrySnapshot::get_physics_tyreCoreTemperature() const { return physics_tyreCoreTemperature; }
Vector4 GDTelemetrySnapshot::get_physics_camberRAD() const { return physics_camberRAD; }
Vector4 GDTelemetrySnapshot::get_physics_suspensionTravel() const { return physics_suspensionTravel; }
float GDTelemetrySnapshot::get_physics_drs() const { return physics_drs; }
float GDTelemetrySnapshot::get_physics_tc() const { return physics_tc; }
float GDTelemetrySnapshot::get_physics_heading() const { return physics_heading; }
float GDTelemetrySnapshot::get_physics_pitch() const { return physics_pitch; }
float GDTelemetrySnapshot::get_physics_roll() const { return physics_roll; }
float GDTelemetrySnapshot::get_physics_cgHeight() const { return physics_cgHeight; }
Vector4 GDTelemetrySnapshot::get_physics_carDamage() const { return physics_carDamage; }
int GDTelemetrySnapshot::get_physics_numberOfTyresOut() const { return physics_numberOfTyresOut; }
int GDTelemetrySnapshot::get_physics_pitLimiterOn() const { return physics_pitLimiterOn; }
float GDTelemetrySnapshot::get_physics_abs() const { return physics_abs; }
float GDTelemetrySnapshot::get_physics_kersCharge() const { return physics_kersCharge; }
float GDTelemetrySnapshot::get_physics_kersInput() const { return physics_kersInput; }
int GDTelemetrySnapshot::get_physics_autoShifterOn() const { return physics_autoShifterOn; }
Vector2 GDTelemetrySnapshot::get_physics_rideHeight() const { return physics_rideHeight; }
float GDTelemetrySnapshot::get_physics_turboBoost() const { return physics_turboBoost; }
float GDTelemetrySnapshot::get_physics_ballast() const { return physics_ballast; }
float GDTelemetrySnapshot::get_physics_airDensity() const { return physics_airDensity; }

int GDTelemetrySnapshot::get_graphic_packetId() const { return graphic_packetId; }
// int GDTelemetrySnapshot::get_graphic_status() const { return graphic_status; }
// int GDTelemetrySnapshot::get_graphic_session() const { return graphic_session; }
String GDTelemetrySnapshot::get_graphic_currentTime() const { return graphic_currentTime; }
String GDTelemetrySnapshot::get_graphic_lastTime() const { return graphic_lastTime; }
String GDTelemetrySnapshot::get_graphic_bestTime() const { return graphic_bestTime; }
String GDTelemetrySnapshot::get_graphic_split() const { return graphic_split; }
int GDTelemetrySnapshot::get_graphic_completedLaps() const { return graphic_completedLaps; }
int GDTelemetrySnapshot::get_graphic_position() const { return graphic_position; }
int GDTelemetrySnapshot::get_graphic_iCurrentTime() const { return graphic_iCurrentTime; }
int GDTelemetrySnapshot::get_graphic_iLastTime() const { return graphic_iLastTime; }
int GDTelemetrySnapshot::get_graphic_iBestTime() const { return graphic_iBestTime; }
float GDTelemetrySnapshot::get_graphic_sessionTimeLeft() const { return graphic_sessionTimeLeft; }
float GDTelemetrySnapshot::get_graphic_distanceTraveled() const { return graphic_distanceTraveled; }
int GDTelemetrySnapshot::get_graphic_isInPit() const { return graphic_isInPit; }
int GDTelemetrySnapshot::get_graphic_currentSectorIndex() const { return graphic_currentSectorIndex; }
int GDTelemetrySnapshot::get_graphic_lastSectorTime() const { return graphic_lastSectorTime; }
int GDTelemetrySnapshot::get_graphic_numberOfLaps() const { return graphic_numberOfLaps; }
String GDTelemetrySnapshot::get_graphic_tyreCompound() const { return graphic_tyreCompound; }
// float GDTelemetrySnapshot::get_graphic_replayTimeMultiplier() const { return graphic_replayTimeMultiplier; }
float GDTelemetrySnapshot::get_graphic_normalizedCarPosition() const { return graphic_normalizedCarPosition; }
Vector3 GDTelemetrySnapshot::get_graphic_carCoordinates() const { return graphic_carCoordinates; }
float GDTelemetrySnapshot::get_graphic_penaltyTime() const { return graphic_penaltyTime; }
int GDTelemetrySnapshot::get_graphic_flag() const { return graphic_flag; }
// int GDTelemetrySnapshot::get_graphic_idealLineOn() const { return graphic_idealLineOn; }
int GDTelemetrySnapshot::get_graphic_isInPitLane() const { return graphic_isInPitLane; }
float GDTelemetrySnapshot::get_graphic_surfaceGrip() const { return graphic_surfaceGrip; }
