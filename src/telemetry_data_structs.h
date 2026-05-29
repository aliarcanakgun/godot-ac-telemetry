#pragma once

#include <vector>
#include <string>
#include <ostream>
#include <istream>
#include <array>

// assetto corsa status and types
typedef int AC_STATUS;
#define AC_OFF 0
#define AC_REPLAY 1
#define AC_LIVE 2
#define AC_PAUSE 3

typedef int AC_SESSION_TYPE;
#define AC_UNKNOWN -1
#define AC_PRACTICE 0
#define AC_QUALIFY 1
#define AC_RACE 2
#define AC_HOTLAP 3
#define AC_TIME_ATTACK 4
#define AC_DRIFT 5
#define AC_DRAG 6

typedef int AC_FLAG_TYPE;
#define AC_NO_FLAG 0
#define AC_BLUE_FLAG 1
#define AC_YELLOW_FLAG 2
#define AC_BLACK_FLAG 3
#define AC_WHITE_FLAG 4
#define AC_CHECKERED_FLAG 5
#define AC_PENALTY_FLAG 6

// ensure 4-byte alignment to match assetto corsa memory layout
#pragma pack(push, 4)

struct SPagePhysics {
    int packetId = 0;
    float gas = 0;
    float brake = 0;
    float fuel = 0;
    int gear = 0;
    int rpms = 0;
    float steerAngle = 0;
    float speedKmh = 0;
    float velocity[3];
    float accG[3];
    float wheelSlip[4];
    float wheelLoad[4];
    float wheelsPressure[4];
    float wheelAngularSpeed[4];
    float tyreWear[4];
    float tyreDirtyLevel[4];
    float tyreCoreTemperature[4];
    float camberRAD[4];
    float suspensionTravel[4];
    float drs = 0;
    float tc = 0;
    float heading = 0;
    float pitch = 0;
    float roll = 0;
    float cgHeight;
    float carDamage[5];
    int numberOfTyresOut = 0;
    int pitLimiterOn = 0;
    float abs = 0;
    float kersCharge = 0;
    float kersInput = 0;
    int autoShifterOn = 0;
    float rideHeight[2];
    float turboBoost = 0;
    float ballast = 0;
    float airDensity = 0;

    float airTemp = 0;
    float roadTemp = 0;
    float localAngularVel[3];
    float finalFF = 0;
    float performanceMeter = 0;
    int engineBrake = 0;
    int ersRecoveryLevel = 0;
    int ersPowerLevel = 0;
    int ersHeatCharging = 0;
    int ersIsCharging = 0;
    float kersCurrentKJ = 0;
    int drsAvailable = 0;
    int drsEnabled = 0;
    float brakeTemp[4];
    float clutch = 0;
    float tyreTempI[4];
    float tyreTempM[4];
    float tyreTempO[4];
    int isAIControlled;
    float tyreContactPoint[4][3];
    float tyreContactNormal[4][3];
    float tyreContactHeading[4][3];
    float brakeBias;
    float localVelocity[3];
};

struct SPageGraphic {
    int packetId = 0;
    AC_STATUS status = AC_OFF;
    AC_SESSION_TYPE session = AC_PRACTICE;
    wchar_t currentTime[15];
    wchar_t lastTime[15];
    wchar_t bestTime[15];
    wchar_t split[15];
    int completedLaps = 0;
    int position = 0;
    int iCurrentTime = 0;
    int iLastTime = 0;
    int iBestTime = 0;
    float sessionTimeLeft = 0;
    float distanceTraveled = 0;
    int isInPit = 0;
    int currentSectorIndex = 0;
    int lastSectorTime = 0;
    int numberOfLaps = 0;
    wchar_t tyreCompound[33];

    float replayTimeMultiplier = 0;
    float normalizedCarPosition = 0;
    float carCoordinates[3];
    float penaltyTime = 0;
    AC_FLAG_TYPE flag = AC_NO_FLAG;
    int idealLineOn = 0;
    int isInPitLane = 0;

    float surfaceGrip = 0;

    int mandatoryPitDone = 0;
    float windSpeed = 0;
    float windDirection = 0;
};

struct SPageStatic {
    wchar_t smVersion[15];
    wchar_t acVersion[15];

    // session static info
    int numberOfSessions = 0;
    int numCars = 0;
    wchar_t carModel[33];
    wchar_t track[33];
    wchar_t playerName[33];
    wchar_t playerSurname[33];
    wchar_t playerNick[33];
    int sectorCount = 0;

    // car static info
    float maxTorque = 0;
    float maxPower = 0;
    int maxRpm = 0;
    float maxFuel = 0;
    float suspensionMaxTravel[4];
    float tyreRadius[4];
    float maxTurboBoost = 0;

    float deprecated_1 = 0;
    float deprecated_2 = 0;

    int penaltiesEnabled = 0;

    float aidFuelRate = 0;
    float aidTireRate = 0;
    float aidMechanicalDamage = 0;
    int aidAllowTyreBlankets = 0;
    float aidStability = 0;
    int aidAutoClutch = 0;
    int aidAutoBlip = 0;

    
    int hasDRS = 0;
    int hasERS = 0;
    int hasKERS = 0;
    float kersMaxJ = 0;
    int engineBrakeSettingsCount = 0;
    int ersPowerControllerCount = 0;
    
    float trackSPlineLength = 0;
    wchar_t trackConfiguration[33];
    
    float ersMaxJ = 0;
    int isTimedRace = 0;
    int hasExtraLap = 0;
    wchar_t carSkin[33];
    int reversedGridPositions = 0;
    int pitWindowStart = 0;
    int pitWindowEnd = 0;
    int isOnline = 0;
};

// channel data for a single lap
struct LapDataChannels {
    // timestamps & positions
    std::vector<double> timestamp;
    
    // graphic basics
    std::vector<int> packetId_graphic;
    std::vector<int32_t> iCurrentTime;
    std::vector<int32_t> iLastTime;
    std::vector<int32_t> iBestTime;
    std::vector<float> normalizedCarPosition;
    std::vector<float> distanceTraveled;
    std::vector<float> replayTimeMultiplier;
    std::vector<int> numberOfLaps;
    std::vector<int> completedLaps;

    // graphic strings
    std::vector<std::array<wchar_t, 15>> currentTime;
    std::vector<std::array<wchar_t, 15>> lastTime;
    std::vector<std::array<wchar_t, 15>> bestTime;
    std::vector<std::array<wchar_t, 15>> split;
    std::vector<std::array<wchar_t, 33>> tyreCompound;

    // physics - basic
    std::vector<int> packetId_physics;
    std::vector<float> gas;
    std::vector<float> brake;
    std::vector<float> fuel;
    std::vector<int> gear;
    std::vector<int> rpms;
    std::vector<float> steerAngle;
    std::vector<float> speedKmh;
    std::vector<int> isAIControlled;

    // physics - vectors/arrays
    // for simplicity and cache locality, store 3d vectors as 3 separate vectors
    std::vector<float> velocity_x;
    std::vector<float> velocity_y;
    std::vector<float> velocity_z;

    std::vector<float> accG_x;
    std::vector<float> accG_y;
    std::vector<float> accG_z;

    // wheel data (FL, FR, RL, RR)
    std::vector<float> wheelSlip_fl, wheelSlip_fr, wheelSlip_rl, wheelSlip_rr;
    std::vector<float> wheelLoad_fl, wheelLoad_fr, wheelLoad_rl, wheelLoad_rr;
    std::vector<float> wheelsPressure_fl, wheelsPressure_fr, wheelsPressure_rl, wheelsPressure_rr;
    std::vector<float> wheelAngularSpeed_fl, wheelAngularSpeed_fr, wheelAngularSpeed_rl, wheelAngularSpeed_rr;
    std::vector<float> tyreWear_fl, tyreWear_fr, tyreWear_rl, tyreWear_rr;
    std::vector<float> tyreDirtyLevel_fl, tyreDirtyLevel_fr, tyreDirtyLevel_rl, tyreDirtyLevel_rr;
    std::vector<float> tyreCoreTemperature_fl, tyreCoreTemperature_fr, tyreCoreTemperature_rl, tyreCoreTemperature_rr;
    std::vector<float> camberRAD_fl, camberRAD_fr, camberRAD_rl, camberRAD_rr;
    std::vector<float> suspensionTravel_fl, suspensionTravel_fr, suspensionTravel_rl, suspensionTravel_rr;
    std::vector<float> brakeTemp_fl, brakeTemp_fr, brakeTemp_rl, brakeTemp_rr;
    std::vector<float> tyreTempI_fl, tyreTempI_fr, tyreTempI_rl, tyreTempI_rr;
    std::vector<float> tyreTempM_fl, tyreTempM_fr, tyreTempM_rl, tyreTempM_rr;
    std::vector<float> tyreTempO_fl, tyreTempO_fr, tyreTempO_rl, tyreTempO_rr;
    
    // tyre contact point
    std::vector<float> tyreContactPoint_fl_x, tyreContactPoint_fl_y, tyreContactPoint_fl_z;
    std::vector<float> tyreContactPoint_fr_x, tyreContactPoint_fr_y, tyreContactPoint_fr_z;
    std::vector<float> tyreContactPoint_rl_x, tyreContactPoint_rl_y, tyreContactPoint_rl_z;
    std::vector<float> tyreContactPoint_rr_x, tyreContactPoint_rr_y, tyreContactPoint_rr_z;

    // tyre contact normal
    std::vector<float> tyreContactNormal_fl_x, tyreContactNormal_fl_y, tyreContactNormal_fl_z;
    std::vector<float> tyreContactNormal_fr_x, tyreContactNormal_fr_y, tyreContactNormal_fr_z;
    std::vector<float> tyreContactNormal_rl_x, tyreContactNormal_rl_y, tyreContactNormal_rl_z;
    std::vector<float> tyreContactNormal_rr_x, tyreContactNormal_rr_y, tyreContactNormal_rr_z;

    // tyre contact heading
    std::vector<float> tyreContactHeading_fl_x, tyreContactHeading_fl_y, tyreContactHeading_fl_z;
    std::vector<float> tyreContactHeading_fr_x, tyreContactHeading_fr_y, tyreContactHeading_fr_z;
    std::vector<float> tyreContactHeading_rl_x, tyreContactHeading_rl_y, tyreContactHeading_rl_z;
    std::vector<float> tyreContactHeading_rr_x, tyreContactHeading_rr_y, tyreContactHeading_rr_z;

    // physics - advanced
    std::vector<float> drs;
    std::vector<float> tc;
    std::vector<float> heading;
    std::vector<float> pitch;
    std::vector<float> roll;
    std::vector<float> cgHeight;
    std::vector<int> pitLimiterOn;
    std::vector<float> abs;
    std::vector<float> kersCharge;
    std::vector<float> kersInput;
    std::vector<int> autoShifterOn;
    std::vector<float> rideHeight_f, rideHeight_r;
    std::vector<float> turboBoost;
    std::vector<float> ballast;
    std::vector<float> airDensity;
    std::vector<float> airTemp;
    std::vector<float> roadTemp;
    std::vector<float> localAngularVel_x, localAngularVel_y, localAngularVel_z;
    std::vector<float> finalFF;
    std::vector<float> performanceMeter;
    std::vector<int> engineBrake;
    std::vector<int> ersRecoveryLevel;
    std::vector<int> ersPowerLevel;
    std::vector<int> ersHeatCharging;
    std::vector<int> ersIsCharging;
    std::vector<float> kersCurrentKJ;
    std::vector<int> drsAvailable;
    std::vector<int> drsEnabled;
    std::vector<float> clutch;
    std::vector<float> brakeBias;
    std::vector<float> localVelocity_x, localVelocity_y, localVelocity_z;
    
    std::vector<float> carDamage_0, carDamage_1, carDamage_2, carDamage_3, carDamage_4;
    std::vector<int> numberOfTyresOut;

    // graphic - specific
    std::vector<AC_STATUS> status;
    std::vector<AC_SESSION_TYPE> session;
    std::vector<int> position;
    std::vector<float> sessionTimeLeft;
    std::vector<int> isInPit;
    std::vector<int> currentSectorIndex;
    std::vector<int> lastSectorTime;
    std::vector<float> carCoordinates_x, carCoordinates_y, carCoordinates_z;
    std::vector<float> penaltyTime;
    std::vector<AC_FLAG_TYPE> flag;
    std::vector<int> idealLineOn;
    std::vector<int> isInPitLane;
    std::vector<float> surfaceGrip;
    std::vector<int> mandatoryPitDone;
    std::vector<float> windSpeed;
    std::vector<float> windDirection;

    // clear all channels
    void clear() {
        timestamp.clear();
        packetId_graphic.clear();
        iCurrentTime.clear();
        iLastTime.clear();
        iBestTime.clear();
        normalizedCarPosition.clear();
        distanceTraveled.clear();
        replayTimeMultiplier.clear();
        numberOfLaps.clear();
        completedLaps.clear();
        
        currentTime.clear();
        lastTime.clear();
        bestTime.clear();
        split.clear();
        tyreCompound.clear();
        
        packetId_physics.clear();
        gas.clear(); brake.clear(); fuel.clear(); gear.clear(); rpms.clear();
        steerAngle.clear(); speedKmh.clear(); isAIControlled.clear();
        velocity_x.clear(); velocity_y.clear(); velocity_z.clear();
        accG_x.clear(); accG_y.clear(); accG_z.clear();
        
        wheelSlip_fl.clear(); wheelSlip_fr.clear(); wheelSlip_rl.clear(); wheelSlip_rr.clear();
        wheelLoad_fl.clear(); wheelLoad_fr.clear(); wheelLoad_rl.clear(); wheelLoad_rr.clear();
        wheelsPressure_fl.clear(); wheelsPressure_fr.clear(); wheelsPressure_rl.clear(); wheelsPressure_rr.clear();
        wheelAngularSpeed_fl.clear(); wheelAngularSpeed_fr.clear(); wheelAngularSpeed_rl.clear(); wheelAngularSpeed_rr.clear();
        tyreWear_fl.clear(); tyreWear_fr.clear(); tyreWear_rl.clear(); tyreWear_rr.clear();
        tyreDirtyLevel_fl.clear(); tyreDirtyLevel_fr.clear(); tyreDirtyLevel_rl.clear(); tyreDirtyLevel_rr.clear();
        tyreCoreTemperature_fl.clear(); tyreCoreTemperature_fr.clear(); tyreCoreTemperature_rl.clear(); tyreCoreTemperature_rr.clear();
        camberRAD_fl.clear(); camberRAD_fr.clear(); camberRAD_rl.clear(); camberRAD_rr.clear();
        suspensionTravel_fl.clear(); suspensionTravel_fr.clear(); suspensionTravel_rl.clear(); suspensionTravel_rr.clear();
        brakeTemp_fl.clear(); brakeTemp_fr.clear(); brakeTemp_rl.clear(); brakeTemp_rr.clear();
        tyreTempI_fl.clear(); tyreTempI_fr.clear(); tyreTempI_rl.clear(); tyreTempI_rr.clear();
        tyreTempM_fl.clear(); tyreTempM_fr.clear(); tyreTempM_rl.clear(); tyreTempM_rr.clear();
        tyreTempO_fl.clear(); tyreTempO_fr.clear(); tyreTempO_rl.clear(); tyreTempO_rr.clear();
        
        tyreContactPoint_fl_x.clear(); tyreContactPoint_fl_y.clear(); tyreContactPoint_fl_z.clear();
        tyreContactPoint_fr_x.clear(); tyreContactPoint_fr_y.clear(); tyreContactPoint_fr_z.clear();
        tyreContactPoint_rl_x.clear(); tyreContactPoint_rl_y.clear(); tyreContactPoint_rl_z.clear();
        tyreContactPoint_rr_x.clear(); tyreContactPoint_rr_y.clear(); tyreContactPoint_rr_z.clear();
        tyreContactNormal_fl_x.clear(); tyreContactNormal_fl_y.clear(); tyreContactNormal_fl_z.clear();
        tyreContactNormal_fr_x.clear(); tyreContactNormal_fr_y.clear(); tyreContactNormal_fr_z.clear();
        tyreContactNormal_rl_x.clear(); tyreContactNormal_rl_y.clear(); tyreContactNormal_rl_z.clear();
        tyreContactNormal_rr_x.clear(); tyreContactNormal_rr_y.clear(); tyreContactNormal_rr_z.clear();
        tyreContactHeading_fl_x.clear(); tyreContactHeading_fl_y.clear(); tyreContactHeading_fl_z.clear();
        tyreContactHeading_fr_x.clear(); tyreContactHeading_fr_y.clear(); tyreContactHeading_fr_z.clear();
        tyreContactHeading_rl_x.clear(); tyreContactHeading_rl_y.clear(); tyreContactHeading_rl_z.clear();
        tyreContactHeading_rr_x.clear(); tyreContactHeading_rr_y.clear(); tyreContactHeading_rr_z.clear();
        
        drs.clear(); tc.clear(); heading.clear(); pitch.clear(); roll.clear(); cgHeight.clear();
        pitLimiterOn.clear(); abs.clear(); kersCharge.clear(); kersInput.clear(); autoShifterOn.clear();
        rideHeight_f.clear(); rideHeight_r.clear(); turboBoost.clear(); ballast.clear(); airDensity.clear();
        airTemp.clear(); roadTemp.clear();
        localAngularVel_x.clear(); localAngularVel_y.clear(); localAngularVel_z.clear();
        finalFF.clear(); performanceMeter.clear(); engineBrake.clear(); ersRecoveryLevel.clear();
        ersPowerLevel.clear(); ersHeatCharging.clear(); ersIsCharging.clear(); kersCurrentKJ.clear();
        drsAvailable.clear(); drsEnabled.clear(); clutch.clear(); brakeBias.clear();
        localVelocity_x.clear(); localVelocity_y.clear(); localVelocity_z.clear();
        
        carDamage_0.clear(); carDamage_1.clear(); carDamage_2.clear(); carDamage_3.clear(); carDamage_4.clear();
        numberOfTyresOut.clear();
        
        status.clear(); session.clear(); position.clear(); sessionTimeLeft.clear(); isInPit.clear();
        currentSectorIndex.clear(); lastSectorTime.clear();
        carCoordinates_x.clear(); carCoordinates_y.clear(); carCoordinates_z.clear();
        penaltyTime.clear(); flag.clear(); idealLineOn.clear(); isInPitLane.clear();
        surfaceGrip.clear(); mandatoryPitDone.clear(); windSpeed.clear(); windDirection.clear();
    }

    void write_to_stream(std::ostream& out) const {
        uint64_t lap_size = timestamp.size();
        out.write(reinterpret_cast<const char*>(&lap_size), sizeof(lap_size));
        if (lap_size == 0) return;

        #define WRITE_CHAN(vec) out.write(reinterpret_cast<const char*>(vec.data()), lap_size * sizeof(vec[0]))
        WRITE_CHAN(timestamp); WRITE_CHAN(packetId_graphic); WRITE_CHAN(iCurrentTime); WRITE_CHAN(iLastTime); WRITE_CHAN(iBestTime); WRITE_CHAN(normalizedCarPosition); WRITE_CHAN(distanceTraveled);
        WRITE_CHAN(replayTimeMultiplier); WRITE_CHAN(numberOfLaps); WRITE_CHAN(completedLaps);
        WRITE_CHAN(currentTime); WRITE_CHAN(lastTime); WRITE_CHAN(bestTime); WRITE_CHAN(split); WRITE_CHAN(tyreCompound);
        WRITE_CHAN(packetId_physics); WRITE_CHAN(gas); WRITE_CHAN(brake); WRITE_CHAN(fuel); WRITE_CHAN(gear); WRITE_CHAN(rpms);
        WRITE_CHAN(steerAngle); WRITE_CHAN(speedKmh); WRITE_CHAN(isAIControlled);
        WRITE_CHAN(velocity_x); WRITE_CHAN(velocity_y); WRITE_CHAN(velocity_z);
        WRITE_CHAN(accG_x); WRITE_CHAN(accG_y); WRITE_CHAN(accG_z);
        
        WRITE_CHAN(wheelSlip_fl); WRITE_CHAN(wheelSlip_fr); WRITE_CHAN(wheelSlip_rl); WRITE_CHAN(wheelSlip_rr);
        WRITE_CHAN(wheelLoad_fl); WRITE_CHAN(wheelLoad_fr); WRITE_CHAN(wheelLoad_rl); WRITE_CHAN(wheelLoad_rr);
        WRITE_CHAN(wheelsPressure_fl); WRITE_CHAN(wheelsPressure_fr); WRITE_CHAN(wheelsPressure_rl); WRITE_CHAN(wheelsPressure_rr);
        WRITE_CHAN(wheelAngularSpeed_fl); WRITE_CHAN(wheelAngularSpeed_fr); WRITE_CHAN(wheelAngularSpeed_rl); WRITE_CHAN(wheelAngularSpeed_rr);
        WRITE_CHAN(tyreWear_fl); WRITE_CHAN(tyreWear_fr); WRITE_CHAN(tyreWear_rl); WRITE_CHAN(tyreWear_rr);
        WRITE_CHAN(tyreDirtyLevel_fl); WRITE_CHAN(tyreDirtyLevel_fr); WRITE_CHAN(tyreDirtyLevel_rl); WRITE_CHAN(tyreDirtyLevel_rr);
        WRITE_CHAN(tyreCoreTemperature_fl); WRITE_CHAN(tyreCoreTemperature_fr); WRITE_CHAN(tyreCoreTemperature_rl); WRITE_CHAN(tyreCoreTemperature_rr);
        WRITE_CHAN(camberRAD_fl); WRITE_CHAN(camberRAD_fr); WRITE_CHAN(camberRAD_rl); WRITE_CHAN(camberRAD_rr);
        WRITE_CHAN(suspensionTravel_fl); WRITE_CHAN(suspensionTravel_fr); WRITE_CHAN(suspensionTravel_rl); WRITE_CHAN(suspensionTravel_rr);
        WRITE_CHAN(brakeTemp_fl); WRITE_CHAN(brakeTemp_fr); WRITE_CHAN(brakeTemp_rl); WRITE_CHAN(brakeTemp_rr);
        WRITE_CHAN(tyreTempI_fl); WRITE_CHAN(tyreTempI_fr); WRITE_CHAN(tyreTempI_rl); WRITE_CHAN(tyreTempI_rr);
        WRITE_CHAN(tyreTempM_fl); WRITE_CHAN(tyreTempM_fr); WRITE_CHAN(tyreTempM_rl); WRITE_CHAN(tyreTempM_rr);
        WRITE_CHAN(tyreTempO_fl); WRITE_CHAN(tyreTempO_fr); WRITE_CHAN(tyreTempO_rl); WRITE_CHAN(tyreTempO_rr);
        
        WRITE_CHAN(tyreContactPoint_fl_x); WRITE_CHAN(tyreContactPoint_fl_y); WRITE_CHAN(tyreContactPoint_fl_z);
        WRITE_CHAN(tyreContactPoint_fr_x); WRITE_CHAN(tyreContactPoint_fr_y); WRITE_CHAN(tyreContactPoint_fr_z);
        WRITE_CHAN(tyreContactPoint_rl_x); WRITE_CHAN(tyreContactPoint_rl_y); WRITE_CHAN(tyreContactPoint_rl_z);
        WRITE_CHAN(tyreContactPoint_rr_x); WRITE_CHAN(tyreContactPoint_rr_y); WRITE_CHAN(tyreContactPoint_rr_z);
        
        WRITE_CHAN(tyreContactNormal_fl_x); WRITE_CHAN(tyreContactNormal_fl_y); WRITE_CHAN(tyreContactNormal_fl_z);
        WRITE_CHAN(tyreContactNormal_fr_x); WRITE_CHAN(tyreContactNormal_fr_y); WRITE_CHAN(tyreContactNormal_fr_z);
        WRITE_CHAN(tyreContactNormal_rl_x); WRITE_CHAN(tyreContactNormal_rl_y); WRITE_CHAN(tyreContactNormal_rl_z);
        WRITE_CHAN(tyreContactNormal_rr_x); WRITE_CHAN(tyreContactNormal_rr_y); WRITE_CHAN(tyreContactNormal_rr_z);
        
        WRITE_CHAN(tyreContactHeading_fl_x); WRITE_CHAN(tyreContactHeading_fl_y); WRITE_CHAN(tyreContactHeading_fl_z);
        WRITE_CHAN(tyreContactHeading_fr_x); WRITE_CHAN(tyreContactHeading_fr_y); WRITE_CHAN(tyreContactHeading_fr_z);
        WRITE_CHAN(tyreContactHeading_rl_x); WRITE_CHAN(tyreContactHeading_rl_y); WRITE_CHAN(tyreContactHeading_rl_z);
        WRITE_CHAN(tyreContactHeading_rr_x); WRITE_CHAN(tyreContactHeading_rr_y); WRITE_CHAN(tyreContactHeading_rr_z);
        
        WRITE_CHAN(drs); WRITE_CHAN(tc); WRITE_CHAN(heading); WRITE_CHAN(pitch); WRITE_CHAN(roll); WRITE_CHAN(cgHeight);
        WRITE_CHAN(pitLimiterOn); WRITE_CHAN(abs); WRITE_CHAN(kersCharge); WRITE_CHAN(kersInput); WRITE_CHAN(autoShifterOn);
        WRITE_CHAN(rideHeight_f); WRITE_CHAN(rideHeight_r); WRITE_CHAN(turboBoost); WRITE_CHAN(ballast); WRITE_CHAN(airDensity);
        WRITE_CHAN(airTemp); WRITE_CHAN(roadTemp);
        WRITE_CHAN(localAngularVel_x); WRITE_CHAN(localAngularVel_y); WRITE_CHAN(localAngularVel_z);
        WRITE_CHAN(finalFF); WRITE_CHAN(performanceMeter); WRITE_CHAN(engineBrake); WRITE_CHAN(ersRecoveryLevel);
        WRITE_CHAN(ersPowerLevel); WRITE_CHAN(ersHeatCharging); WRITE_CHAN(ersIsCharging); WRITE_CHAN(kersCurrentKJ);
        WRITE_CHAN(drsAvailable); WRITE_CHAN(drsEnabled); WRITE_CHAN(clutch); WRITE_CHAN(brakeBias);
        WRITE_CHAN(localVelocity_x); WRITE_CHAN(localVelocity_y); WRITE_CHAN(localVelocity_z);
        
        WRITE_CHAN(carDamage_0); WRITE_CHAN(carDamage_1); WRITE_CHAN(carDamage_2); WRITE_CHAN(carDamage_3); WRITE_CHAN(carDamage_4);
        WRITE_CHAN(numberOfTyresOut);
        
        WRITE_CHAN(status); WRITE_CHAN(session); WRITE_CHAN(position); WRITE_CHAN(sessionTimeLeft); WRITE_CHAN(isInPit);
        WRITE_CHAN(currentSectorIndex); WRITE_CHAN(lastSectorTime);
        WRITE_CHAN(carCoordinates_x); WRITE_CHAN(carCoordinates_y); WRITE_CHAN(carCoordinates_z);
        WRITE_CHAN(penaltyTime); WRITE_CHAN(flag); WRITE_CHAN(idealLineOn); WRITE_CHAN(isInPitLane);
        WRITE_CHAN(surfaceGrip); WRITE_CHAN(mandatoryPitDone); WRITE_CHAN(windSpeed); WRITE_CHAN(windDirection);
        #undef WRITE_CHAN
    }

    void read_from_stream(std::istream& in) {
        clear();
        uint64_t lap_size = 0;
        in.read(reinterpret_cast<char*>(&lap_size), sizeof(lap_size));
        if (lap_size == 0) return;

        #define READ_CHAN(vec) vec.resize(lap_size); in.read(reinterpret_cast<char*>(vec.data()), lap_size * sizeof(vec[0]))
        READ_CHAN(timestamp); READ_CHAN(packetId_graphic); READ_CHAN(iCurrentTime); READ_CHAN(iLastTime); READ_CHAN(iBestTime); READ_CHAN(normalizedCarPosition); READ_CHAN(distanceTraveled);
        READ_CHAN(replayTimeMultiplier); READ_CHAN(numberOfLaps); READ_CHAN(completedLaps);
        READ_CHAN(currentTime); READ_CHAN(lastTime); READ_CHAN(bestTime); READ_CHAN(split); READ_CHAN(tyreCompound);
        READ_CHAN(packetId_physics); READ_CHAN(gas); READ_CHAN(brake); READ_CHAN(fuel); READ_CHAN(gear); READ_CHAN(rpms);
        READ_CHAN(steerAngle); READ_CHAN(speedKmh); READ_CHAN(isAIControlled);
        READ_CHAN(velocity_x); READ_CHAN(velocity_y); READ_CHAN(velocity_z);
        READ_CHAN(accG_x); READ_CHAN(accG_y); READ_CHAN(accG_z);
        
        READ_CHAN(wheelSlip_fl); READ_CHAN(wheelSlip_fr); READ_CHAN(wheelSlip_rl); READ_CHAN(wheelSlip_rr);
        READ_CHAN(wheelLoad_fl); READ_CHAN(wheelLoad_fr); READ_CHAN(wheelLoad_rl); READ_CHAN(wheelLoad_rr);
        READ_CHAN(wheelsPressure_fl); READ_CHAN(wheelsPressure_fr); READ_CHAN(wheelsPressure_rl); READ_CHAN(wheelsPressure_rr);
        READ_CHAN(wheelAngularSpeed_fl); READ_CHAN(wheelAngularSpeed_fr); READ_CHAN(wheelAngularSpeed_rl); READ_CHAN(wheelAngularSpeed_rr);
        READ_CHAN(tyreWear_fl); READ_CHAN(tyreWear_fr); READ_CHAN(tyreWear_rl); READ_CHAN(tyreWear_rr);
        READ_CHAN(tyreDirtyLevel_fl); READ_CHAN(tyreDirtyLevel_fr); READ_CHAN(tyreDirtyLevel_rl); READ_CHAN(tyreDirtyLevel_rr);
        READ_CHAN(tyreCoreTemperature_fl); READ_CHAN(tyreCoreTemperature_fr); READ_CHAN(tyreCoreTemperature_rl); READ_CHAN(tyreCoreTemperature_rr);
        READ_CHAN(camberRAD_fl); READ_CHAN(camberRAD_fr); READ_CHAN(camberRAD_rl); READ_CHAN(camberRAD_rr);
        READ_CHAN(suspensionTravel_fl); READ_CHAN(suspensionTravel_fr); READ_CHAN(suspensionTravel_rl); READ_CHAN(suspensionTravel_rr);
        READ_CHAN(brakeTemp_fl); READ_CHAN(brakeTemp_fr); READ_CHAN(brakeTemp_rl); READ_CHAN(brakeTemp_rr);
        READ_CHAN(tyreTempI_fl); READ_CHAN(tyreTempI_fr); READ_CHAN(tyreTempI_rl); READ_CHAN(tyreTempI_rr);
        READ_CHAN(tyreTempM_fl); READ_CHAN(tyreTempM_fr); READ_CHAN(tyreTempM_rl); READ_CHAN(tyreTempM_rr);
        READ_CHAN(tyreTempO_fl); READ_CHAN(tyreTempO_fr); READ_CHAN(tyreTempO_rl); READ_CHAN(tyreTempO_rr);
        
        READ_CHAN(tyreContactPoint_fl_x); READ_CHAN(tyreContactPoint_fl_y); READ_CHAN(tyreContactPoint_fl_z);
        READ_CHAN(tyreContactPoint_fr_x); READ_CHAN(tyreContactPoint_fr_y); READ_CHAN(tyreContactPoint_fr_z);
        READ_CHAN(tyreContactPoint_rl_x); READ_CHAN(tyreContactPoint_rl_y); READ_CHAN(tyreContactPoint_rl_z);
        READ_CHAN(tyreContactPoint_rr_x); READ_CHAN(tyreContactPoint_rr_y); READ_CHAN(tyreContactPoint_rr_z);
        
        READ_CHAN(tyreContactNormal_fl_x); READ_CHAN(tyreContactNormal_fl_y); READ_CHAN(tyreContactNormal_fl_z);
        READ_CHAN(tyreContactNormal_fr_x); READ_CHAN(tyreContactNormal_fr_y); READ_CHAN(tyreContactNormal_fr_z);
        READ_CHAN(tyreContactNormal_rl_x); READ_CHAN(tyreContactNormal_rl_y); READ_CHAN(tyreContactNormal_rl_z);
        READ_CHAN(tyreContactNormal_rr_x); READ_CHAN(tyreContactNormal_rr_y); READ_CHAN(tyreContactNormal_rr_z);
        
        READ_CHAN(tyreContactHeading_fl_x); READ_CHAN(tyreContactHeading_fl_y); READ_CHAN(tyreContactHeading_fl_z);
        READ_CHAN(tyreContactHeading_fr_x); READ_CHAN(tyreContactHeading_fr_y); READ_CHAN(tyreContactHeading_fr_z);
        READ_CHAN(tyreContactHeading_rl_x); READ_CHAN(tyreContactHeading_rl_y); READ_CHAN(tyreContactHeading_rl_z);
        READ_CHAN(tyreContactHeading_rr_x); READ_CHAN(tyreContactHeading_rr_y); READ_CHAN(tyreContactHeading_rr_z);
        
        READ_CHAN(drs); READ_CHAN(tc); READ_CHAN(heading); READ_CHAN(pitch); READ_CHAN(roll); READ_CHAN(cgHeight);
        READ_CHAN(pitLimiterOn); READ_CHAN(abs); READ_CHAN(kersCharge); READ_CHAN(kersInput); READ_CHAN(autoShifterOn);
        READ_CHAN(rideHeight_f); READ_CHAN(rideHeight_r); READ_CHAN(turboBoost); READ_CHAN(ballast); READ_CHAN(airDensity);
        READ_CHAN(airTemp); READ_CHAN(roadTemp);
        READ_CHAN(localAngularVel_x); READ_CHAN(localAngularVel_y); READ_CHAN(localAngularVel_z);
        READ_CHAN(finalFF); READ_CHAN(performanceMeter); READ_CHAN(engineBrake); READ_CHAN(ersRecoveryLevel);
        READ_CHAN(ersPowerLevel); READ_CHAN(ersHeatCharging); READ_CHAN(ersIsCharging); READ_CHAN(kersCurrentKJ);
        READ_CHAN(drsAvailable); READ_CHAN(drsEnabled); READ_CHAN(clutch); READ_CHAN(brakeBias);
        READ_CHAN(localVelocity_x); READ_CHAN(localVelocity_y); READ_CHAN(localVelocity_z);
        
        READ_CHAN(carDamage_0); READ_CHAN(carDamage_1); READ_CHAN(carDamage_2); READ_CHAN(carDamage_3); READ_CHAN(carDamage_4);
        READ_CHAN(numberOfTyresOut);
        
        READ_CHAN(status); READ_CHAN(session); READ_CHAN(position); READ_CHAN(sessionTimeLeft); READ_CHAN(isInPit);
        READ_CHAN(currentSectorIndex); READ_CHAN(lastSectorTime);
        READ_CHAN(carCoordinates_x); READ_CHAN(carCoordinates_y); READ_CHAN(carCoordinates_z);
        READ_CHAN(penaltyTime); READ_CHAN(flag); READ_CHAN(idealLineOn); READ_CHAN(isInPitLane);
        READ_CHAN(surfaceGrip); READ_CHAN(mandatoryPitDone); READ_CHAN(windSpeed); READ_CHAN(windDirection);
        #undef READ_CHAN
    }
};

#pragma pack(pop)
