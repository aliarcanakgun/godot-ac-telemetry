#pragma once

#include "ac_data_structs.h"
#include <vector>
#include <string>
#include <ostream>
#include <istream>
#include <array>
#include <cstdint>

// ensure 4-byte alignment to match assetto corsa memory layout
#pragma pack(push, 4)

typedef int ACC_FLAG_TYPE;
#define ACC_NO_FLAG 0
#define ACC_BLUE_FLAG 1
#define ACC_YELLOW_FLAG 2
#define ACC_BLACK_FLAG 3
#define ACC_WHITE_FLAG 4
#define ACC_CHECKERED_FLAG 5
#define ACC_PENALTY_FLAG 6
#define ACC_GREEN_FLAG 7
#define ACC_ORANGE_FLAG 8

typedef int ACC_PENALTY_TYPE;
#define ACC_None 0
#define ACC_DriveThrough_Cutting 1
#define ACC_StopAndGo_10_Cutting 2
#define ACC_StopAndGo_20_Cutting 3
#define ACC_StopAndGo_30_Cutting 4
#define ACC_Disqualified_Cutting 5
#define ACC_RemoveBestLaptime_Cutting 6
#define ACC_DriveThrough_PitSpeeding 7
#define ACC_StopAndGo_10_PitSpeeding 8
#define ACC_StopAndGo_20_PitSpeeding 9
#define ACC_StopAndGo_30_PitSpeeding 10
#define ACC_Disqualified_PitSpeeding 11
#define ACC_RemoveBestLaptime_PitSpeeding 12
#define ACC_Disqualified_IgnoredMandatoryPit 13
#define ACC_PostRaceTime 14
#define ACC_Disqualified_Trolling 15
#define ACC_Disqualified_PitEntry 16
#define ACC_Disqualified_PitExit 17
#define ACC_Disqualified_Wrongway 18
#define ACC_DriveThrough_IgnoredDriverStint 19
#define ACC_Disqualified_IgnoredDriverStint 20
#define ACC_Disqualified_ExceededDriverStintLimit 21

typedef int ACC_SESSION_TYPE;
#define ACC_UNKNOWN -1
#define ACC_PRACTICE 0
#define ACC_QUALIFY 1
#define ACC_RACE 2
#define ACC_HOTLAP 3
#define ACC_TIMEATTACK 4
#define ACC_DRIFT 5
#define ACC_DRAG 6
#define ACC_HOTSTINT 7
#define ACC_HOTSTINTSUPERPOLE 8

typedef int ACC_STATUS;
#define ACC_OFF 0
#define ACC_REPLAY 1
#define ACC_LIVE 2
#define ACC_PAUSE 3

typedef int ACC_WHEELS_TYPE;
#define ACC_FrontLeft 0
#define ACC_FrontRight 1
#define ACC_RearLeft 2
#define ACC_RearRight 3

typedef int ACC_TRACK_GRIP_STATUS;
#define ACC_GREEN 0
#define ACC_FAST 1
#define ACC_OPTIMUM 2
#define ACC_GREASY 3
#define ACC_DAMP 4
#define ACC_WET 5
#define ACC_FLOODED 6

typedef int ACC_RAIN_INTENSITY;
#define ACC_NO_RAIN 0
#define ACC_DRIZZLE 1
#define ACC_LIGHT_RAIN 2
#define ACC_MEDIUM_RAIN 3
#define ACC_HEAVY_RAIN 4
#define ACC_THUNDERSTORM 5

struct ACC_SPagePhysics : public AC_SPagePhysics {
    int P2PActivation;
    int P2PStatus;
    float currentMaxRpm;
    float mz[4];
    float fx[4];
    float fy[4];
    float slipRatio[4];
    float slipAngle[4];
    int tcinAction;
    int absInAction;
    float suspensionDamage[4];
    float tyreTemp[4];
    float waterTemp;
    float brakePressure[4];
    int frontBrakeCompound;
    int rearBrakeCompound;
    float padLife[4];
    float discLife[4];
    int ignitionOn;
    int starterEngineOn;
    int isEngineRunning;
    float kerbVibration;
    float slipVibrations;
    float gVibrations;
    float absVibrations;
};

struct ACC_SPageGraphic {
    int packetId;
    ACC_STATUS status;
    ACC_SESSION_TYPE session;
    wchar_t currentTime[15];
    wchar_t lastTime[15];
    wchar_t bestTime[15];
    wchar_t split[15];
    int completedLaps;
    int position;
    int iCurrentTime;
    int iLastTime;
    int iBestTime;
    float sessionTimeLeft;
    float distanceTraveled;
    int isInPit;
    int currentSectorIndex;
    int lastSectorTime;
    int numberOfLaps;
    wchar_t tyreCompound[33];
    float replayTimeMultiplier;
    float normalizedCarPosition;
    int activeCars;
    float carCoordinates[60][3];
    int carID[60];
    int playerCarID;
    float penaltyTime;
    ACC_FLAG_TYPE flag;
    ACC_PENALTY_TYPE penalty;
    int idealLineOn;
    int isInPitLane;
    float surfaceGrip;
    int mandatoryPitDone;
    float windSpeed;
    float windDirection;

    int isSetupMenuVisible;
    int mainDisplayIndex;
    int secondaryDisplyIndex;
    int TC;
    int TCCUT;
    int EngineMap;
    int ABS;
    float fuelXLap;
    int rainLights;
    int flashingLights;
    int lightsStage;
    float exhaustTemperature;
    int wiperLV;
    int driverStintTotalTimeLeft;
    int driverStintTimeLeft;
    int rainTyres;
    int sessionIndex;
    float usedFuel;
    wchar_t deltaLapTime[15];
    int iDeltaLapTime;
    wchar_t estimatedLapTime[15];
    int iEstimatedLapTime;
    int isDeltaPositive;
    int iSplit;
    int isValidLap;
    float fuelEstimatedLaps;
    wchar_t trackStatus[33];
    int missingMandatoryPits;
    float Clock;
    int directionLightsLeft;
    int directionLightsRight;
    int GlobalYellow;
    int GlobalYellow1;
    int GlobalYellow2;
    int GlobalYellow3;
    int GlobalWhite;
    int GlobalGreen;
    int GlobalChequered;
    int GlobalRed;
    int mfdTyreSet;
    float mfdFuelToAdd;
    float mfdTyrePressureLF;
    float mfdTyrePressureRF;
    float mfdTyrePressureLR;
    float mfdTyrePressureRR;
    ACC_TRACK_GRIP_STATUS trackGripStatus;
    ACC_RAIN_INTENSITY rainIntensity;
    ACC_RAIN_INTENSITY rainIntensityIn10min;
    ACC_RAIN_INTENSITY rainIntensityIn30min;
    int currentTyreSet;
    int strategyTyreSet;
    int gapAhead;
    int gapBehind;
};

struct ACC_SPageStatic : public AC_SPageStatic {
    wchar_t dryTyresName[33];
    wchar_t wetTyresName[33];
};

// channel data for a single lap
struct ACC_LapDataChannels : public AC_LapDataChannels {
    std::vector<float> mz_FL, mz_FR, mz_RL, mz_RR;
    std::vector<float> fx_FL, fx_FR, fx_RL, fx_RR;
    std::vector<float> fy_FL, fy_FR, fy_RL, fy_RR;
    std::vector<float> slipRatio_FL, slipRatio_FR, slipRatio_RL, slipRatio_RR;
    std::vector<float> slipAngle_FL, slipAngle_FR, slipAngle_RL, slipAngle_RR;
    
    std::vector<int> tcinAction;
    std::vector<int> absInAction;
    
    std::vector<float> suspensionDamage_FL, suspensionDamage_FR, suspensionDamage_RL, suspensionDamage_RR;
    std::vector<float> tyreTemp_FL, tyreTemp_FR, tyreTemp_RL, tyreTemp_RR;
    std::vector<float> waterTemp;
    
    std::vector<float> brakePressure_FL, brakePressure_FR, brakePressure_RL, brakePressure_RR;
    std::vector<float> padLife_FL, padLife_FR, padLife_RL, padLife_RR;
    std::vector<float> discLife_FL, discLife_FR, discLife_RL, discLife_RR;
    
    std::vector<float> kerbVibration;
    std::vector<float> slipVibrations;
    std::vector<float> gVibrations;
    std::vector<float> absVibrations;

    std::vector<int> TC;
    std::vector<int> TCCUT;
    std::vector<int> EngineMap;
    std::vector<int> ABS;
    std::vector<float> exhaustTemperature;

    std::vector<std::array<std::array<float, 3>, 60>> carCoordinates;
    std::vector<std::array<int, 60>> carID;
    std::vector<int> activeCars;
    std::vector<int> playerCarID;
    std::vector<ACC_PENALTY_TYPE> penalty;
    
    std::vector<int> isSetupMenuVisible;
    std::vector<int> mainDisplayIndex;
    std::vector<int> secondaryDisplyIndex;
    std::vector<float> fuelXLap;
    std::vector<int> rainLights;
    std::vector<int> flashingLights;
    std::vector<int> lightsStage;
    std::vector<int> wiperLV;
    std::vector<int> driverStintTotalTimeLeft;
    std::vector<int> driverStintTimeLeft;
    std::vector<int> rainTyres;
    std::vector<int> sessionIndex;
    std::vector<float> usedFuel;
    std::vector<std::array<wchar_t, 15>> deltaLapTime;
    std::vector<int> iDeltaLapTime;
    std::vector<std::array<wchar_t, 15>> estimatedLapTime;
    std::vector<int> iEstimatedLapTime;
    std::vector<int> isDeltaPositive;
    std::vector<int> iSplit;
    std::vector<int> isValidLap;
    std::vector<float> fuelEstimatedLaps;
    std::vector<std::array<wchar_t, 33>> trackStatus;
    std::vector<int> missingMandatoryPits;
    std::vector<float> Clock;
    std::vector<int> directionLightsLeft;
    std::vector<int> directionLightsRight;
    std::vector<int> GlobalYellow;
    std::vector<int> GlobalYellow1;
    std::vector<int> GlobalYellow2;
    std::vector<int> GlobalYellow3;
    std::vector<int> GlobalWhite;
    std::vector<int> GlobalGreen;
    std::vector<int> GlobalChequered;
    std::vector<int> GlobalRed;
    std::vector<int> mfdTyreSet;
    std::vector<float> mfdFuelToAdd;
    std::vector<float> mfdTyrePressureLF;
    std::vector<float> mfdTyrePressureRF;
    std::vector<float> mfdTyrePressureLR;
    std::vector<float> mfdTyrePressureRR;
    std::vector<ACC_TRACK_GRIP_STATUS> trackGripStatus;
    std::vector<ACC_RAIN_INTENSITY> rainIntensity;
    std::vector<ACC_RAIN_INTENSITY> rainIntensityIn10min;
    std::vector<ACC_RAIN_INTENSITY> rainIntensityIn30min;
    std::vector<int> currentTyreSet;
    std::vector<int> strategyTyreSet;
    std::vector<int> gapAhead;
    std::vector<int> gapBehind;

    // clear all channels
    void clear() {
        AC_LapDataChannels::clear();
        
        mz_FL.clear(); mz_FR.clear(); mz_RL.clear(); mz_RR.clear();
        fx_FL.clear(); fx_FR.clear(); fx_RL.clear(); fx_RR.clear();
        fy_FL.clear(); fy_FR.clear(); fy_RL.clear(); fy_RR.clear();
        slipRatio_FL.clear(); slipRatio_FR.clear(); slipRatio_RL.clear(); slipRatio_RR.clear();
        slipAngle_FL.clear(); slipAngle_FR.clear(); slipAngle_RL.clear(); slipAngle_RR.clear();
        
        tcinAction.clear(); absInAction.clear();
        
        suspensionDamage_FL.clear(); suspensionDamage_FR.clear(); suspensionDamage_RL.clear(); suspensionDamage_RR.clear();
        tyreTemp_FL.clear(); tyreTemp_FR.clear(); tyreTemp_RL.clear(); tyreTemp_RR.clear();
        waterTemp.clear();
        
        brakePressure_FL.clear(); brakePressure_FR.clear(); brakePressure_RL.clear(); brakePressure_RR.clear();
        padLife_FL.clear(); padLife_FR.clear(); padLife_RL.clear(); padLife_RR.clear();
        discLife_FL.clear(); discLife_FR.clear(); discLife_RL.clear(); discLife_RR.clear();
        
        kerbVibration.clear(); slipVibrations.clear(); gVibrations.clear(); absVibrations.clear();
        
        status.clear(); session.clear();
        position.clear();
        sessionTimeLeft.clear(); isInPit.clear(); currentSectorIndex.clear(); lastSectorTime.clear();
        carCoordinates.clear(); carID.clear(); penaltyTime.clear(); flag.clear(); idealLineOn.clear(); isInPitLane.clear();
        surfaceGrip.clear(); mandatoryPitDone.clear(); windSpeed.clear(); windDirection.clear();
        
        TC.clear(); TCCUT.clear(); EngineMap.clear(); ABS.clear(); exhaustTemperature.clear();
        activeCars.clear(); playerCarID.clear(); penalty.clear();
        isSetupMenuVisible.clear(); mainDisplayIndex.clear(); secondaryDisplyIndex.clear();
        fuelXLap.clear(); rainLights.clear(); flashingLights.clear(); lightsStage.clear();
        wiperLV.clear(); driverStintTotalTimeLeft.clear(); driverStintTimeLeft.clear(); rainTyres.clear();
        sessionIndex.clear(); usedFuel.clear(); deltaLapTime.clear(); iDeltaLapTime.clear();
        estimatedLapTime.clear(); iEstimatedLapTime.clear(); isDeltaPositive.clear(); iSplit.clear();
        isValidLap.clear(); fuelEstimatedLaps.clear(); trackStatus.clear(); missingMandatoryPits.clear();
        Clock.clear(); directionLightsLeft.clear(); directionLightsRight.clear();
        GlobalYellow.clear(); GlobalYellow1.clear(); GlobalYellow2.clear(); GlobalYellow3.clear();
        GlobalWhite.clear(); GlobalGreen.clear(); GlobalChequered.clear(); GlobalRed.clear();
        mfdTyreSet.clear(); mfdFuelToAdd.clear(); mfdTyrePressureLF.clear(); mfdTyrePressureRF.clear();
        mfdTyrePressureLR.clear(); mfdTyrePressureRR.clear(); trackGripStatus.clear();
        rainIntensity.clear(); rainIntensityIn10min.clear(); rainIntensityIn30min.clear();
        currentTyreSet.clear(); strategyTyreSet.clear(); gapAhead.clear(); gapBehind.clear();
    }

    void write_to_stream(std::ostream& out) const {
        AC_LapDataChannels::write_to_stream(out);
        
        uint64_t lap_size = timestamp.size();
        if (lap_size == 0) return;

        #define WRITE_CHAN(vec) out.write(reinterpret_cast<const char*>(vec.data()), lap_size * sizeof(vec[0]))
        WRITE_CHAN(mz_FL); WRITE_CHAN(mz_FR); WRITE_CHAN(mz_RL); WRITE_CHAN(mz_RR);
        WRITE_CHAN(fx_FL); WRITE_CHAN(fx_FR); WRITE_CHAN(fx_RL); WRITE_CHAN(fx_RR);
        WRITE_CHAN(fy_FL); WRITE_CHAN(fy_FR); WRITE_CHAN(fy_RL); WRITE_CHAN(fy_RR);
        WRITE_CHAN(slipRatio_FL); WRITE_CHAN(slipRatio_FR); WRITE_CHAN(slipRatio_RL); WRITE_CHAN(slipRatio_RR);
        WRITE_CHAN(slipAngle_FL); WRITE_CHAN(slipAngle_FR); WRITE_CHAN(slipAngle_RL); WRITE_CHAN(slipAngle_RR);
        
        WRITE_CHAN(tcinAction); WRITE_CHAN(absInAction);
        
        WRITE_CHAN(suspensionDamage_FL); WRITE_CHAN(suspensionDamage_FR); WRITE_CHAN(suspensionDamage_RL); WRITE_CHAN(suspensionDamage_RR);
        WRITE_CHAN(tyreTemp_FL); WRITE_CHAN(tyreTemp_FR); WRITE_CHAN(tyreTemp_RL); WRITE_CHAN(tyreTemp_RR);
        WRITE_CHAN(waterTemp);
        
        WRITE_CHAN(brakePressure_FL); WRITE_CHAN(brakePressure_FR); WRITE_CHAN(brakePressure_RL); WRITE_CHAN(brakePressure_RR);
        WRITE_CHAN(padLife_FL); WRITE_CHAN(padLife_FR); WRITE_CHAN(padLife_RL); WRITE_CHAN(padLife_RR);
        WRITE_CHAN(discLife_FL); WRITE_CHAN(discLife_FR); WRITE_CHAN(discLife_RL); WRITE_CHAN(discLife_RR);
        
        WRITE_CHAN(kerbVibration); WRITE_CHAN(slipVibrations); WRITE_CHAN(gVibrations); WRITE_CHAN(absVibrations);
        
        WRITE_CHAN(status); WRITE_CHAN(session); 
        WRITE_CHAN(position); 
        WRITE_CHAN(sessionTimeLeft); WRITE_CHAN(isInPit); WRITE_CHAN(currentSectorIndex); WRITE_CHAN(lastSectorTime);
        WRITE_CHAN(carCoordinates); WRITE_CHAN(carID); WRITE_CHAN(penaltyTime); WRITE_CHAN(flag); WRITE_CHAN(idealLineOn); WRITE_CHAN(isInPitLane);
        WRITE_CHAN(surfaceGrip); WRITE_CHAN(mandatoryPitDone); WRITE_CHAN(windSpeed); WRITE_CHAN(windDirection);

        WRITE_CHAN(TC); WRITE_CHAN(TCCUT); WRITE_CHAN(EngineMap); WRITE_CHAN(ABS); WRITE_CHAN(exhaustTemperature);
        WRITE_CHAN(activeCars); WRITE_CHAN(playerCarID); WRITE_CHAN(penalty);
        WRITE_CHAN(isSetupMenuVisible); WRITE_CHAN(mainDisplayIndex); WRITE_CHAN(secondaryDisplyIndex);
        WRITE_CHAN(fuelXLap); WRITE_CHAN(rainLights); WRITE_CHAN(flashingLights); WRITE_CHAN(lightsStage);
        WRITE_CHAN(wiperLV); WRITE_CHAN(driverStintTotalTimeLeft); WRITE_CHAN(driverStintTimeLeft); WRITE_CHAN(rainTyres);
        WRITE_CHAN(sessionIndex); WRITE_CHAN(usedFuel); WRITE_CHAN(deltaLapTime); WRITE_CHAN(iDeltaLapTime);
        WRITE_CHAN(estimatedLapTime); WRITE_CHAN(iEstimatedLapTime); WRITE_CHAN(isDeltaPositive); WRITE_CHAN(iSplit);
        WRITE_CHAN(isValidLap); WRITE_CHAN(fuelEstimatedLaps); WRITE_CHAN(trackStatus); WRITE_CHAN(missingMandatoryPits);
        WRITE_CHAN(Clock); WRITE_CHAN(directionLightsLeft); WRITE_CHAN(directionLightsRight);
        WRITE_CHAN(GlobalYellow); WRITE_CHAN(GlobalYellow1); WRITE_CHAN(GlobalYellow2); WRITE_CHAN(GlobalYellow3);
        WRITE_CHAN(GlobalWhite); WRITE_CHAN(GlobalGreen); WRITE_CHAN(GlobalChequered); WRITE_CHAN(GlobalRed);
        WRITE_CHAN(mfdTyreSet); WRITE_CHAN(mfdFuelToAdd); WRITE_CHAN(mfdTyrePressureLF); WRITE_CHAN(mfdTyrePressureRF);
        WRITE_CHAN(mfdTyrePressureLR); WRITE_CHAN(mfdTyrePressureRR); WRITE_CHAN(trackGripStatus);
        WRITE_CHAN(rainIntensity); WRITE_CHAN(rainIntensityIn10min); WRITE_CHAN(rainIntensityIn30min);
        WRITE_CHAN(currentTyreSet); WRITE_CHAN(strategyTyreSet); WRITE_CHAN(gapAhead); WRITE_CHAN(gapBehind);
        #undef WRITE_CHAN
    }

    void read_from_stream(std::istream& in) {
        AC_LapDataChannels::read_from_stream(in);
        
        uint64_t lap_size = timestamp.size();
        if (lap_size == 0) return;

        #define READ_CHAN(vec) vec.resize(lap_size); in.read(reinterpret_cast<char*>(vec.data()), lap_size * sizeof(vec[0]))
        READ_CHAN(mz_FL); READ_CHAN(mz_FR); READ_CHAN(mz_RL); READ_CHAN(mz_RR);
        READ_CHAN(fx_FL); READ_CHAN(fx_FR); READ_CHAN(fx_RL); READ_CHAN(fx_RR);
        READ_CHAN(fy_FL); READ_CHAN(fy_FR); READ_CHAN(fy_RL); READ_CHAN(fy_RR);
        READ_CHAN(slipRatio_FL); READ_CHAN(slipRatio_FR); READ_CHAN(slipRatio_RL); READ_CHAN(slipRatio_RR);
        READ_CHAN(slipAngle_FL); READ_CHAN(slipAngle_FR); READ_CHAN(slipAngle_RL); READ_CHAN(slipAngle_RR);
        
        READ_CHAN(tcinAction); READ_CHAN(absInAction);
        
        READ_CHAN(suspensionDamage_FL); READ_CHAN(suspensionDamage_FR); READ_CHAN(suspensionDamage_RL); READ_CHAN(suspensionDamage_RR);
        READ_CHAN(tyreTemp_FL); READ_CHAN(tyreTemp_FR); READ_CHAN(tyreTemp_RL); READ_CHAN(tyreTemp_RR);
        READ_CHAN(waterTemp);
        
        READ_CHAN(brakePressure_FL); READ_CHAN(brakePressure_FR); READ_CHAN(brakePressure_RL); READ_CHAN(brakePressure_RR);
        READ_CHAN(padLife_FL); READ_CHAN(padLife_FR); READ_CHAN(padLife_RL); READ_CHAN(padLife_RR);
        READ_CHAN(discLife_FL); READ_CHAN(discLife_FR); READ_CHAN(discLife_RL); READ_CHAN(discLife_RR);
        
        READ_CHAN(kerbVibration); READ_CHAN(slipVibrations); READ_CHAN(gVibrations); READ_CHAN(absVibrations);
        
        READ_CHAN(status); READ_CHAN(session); 
        READ_CHAN(position); 
        READ_CHAN(sessionTimeLeft); READ_CHAN(isInPit); READ_CHAN(currentSectorIndex); READ_CHAN(lastSectorTime);
        READ_CHAN(carCoordinates); READ_CHAN(carID); READ_CHAN(penaltyTime); READ_CHAN(flag); READ_CHAN(idealLineOn); READ_CHAN(isInPitLane);
        READ_CHAN(surfaceGrip); READ_CHAN(mandatoryPitDone); READ_CHAN(windSpeed); READ_CHAN(windDirection);

        READ_CHAN(TC); READ_CHAN(TCCUT); READ_CHAN(EngineMap); READ_CHAN(ABS); READ_CHAN(exhaustTemperature);
        READ_CHAN(activeCars); READ_CHAN(playerCarID); READ_CHAN(penalty);
        READ_CHAN(isSetupMenuVisible); READ_CHAN(mainDisplayIndex); READ_CHAN(secondaryDisplyIndex);
        READ_CHAN(fuelXLap); READ_CHAN(rainLights); READ_CHAN(flashingLights); READ_CHAN(lightsStage);
        READ_CHAN(wiperLV); READ_CHAN(driverStintTotalTimeLeft); READ_CHAN(driverStintTimeLeft); READ_CHAN(rainTyres);
        READ_CHAN(sessionIndex); READ_CHAN(usedFuel); READ_CHAN(deltaLapTime); READ_CHAN(iDeltaLapTime);
        READ_CHAN(estimatedLapTime); READ_CHAN(iEstimatedLapTime); READ_CHAN(isDeltaPositive); READ_CHAN(iSplit);
        READ_CHAN(isValidLap); READ_CHAN(fuelEstimatedLaps); READ_CHAN(trackStatus); READ_CHAN(missingMandatoryPits);
        READ_CHAN(Clock); READ_CHAN(directionLightsLeft); READ_CHAN(directionLightsRight);
        READ_CHAN(GlobalYellow); READ_CHAN(GlobalYellow1); READ_CHAN(GlobalYellow2); READ_CHAN(GlobalYellow3);
        READ_CHAN(GlobalWhite); READ_CHAN(GlobalGreen); READ_CHAN(GlobalChequered); READ_CHAN(GlobalRed);
        READ_CHAN(mfdTyreSet); READ_CHAN(mfdFuelToAdd); READ_CHAN(mfdTyrePressureLF); READ_CHAN(mfdTyrePressureRF);
        READ_CHAN(mfdTyrePressureLR); READ_CHAN(mfdTyrePressureRR); READ_CHAN(trackGripStatus);
        READ_CHAN(rainIntensity); READ_CHAN(rainIntensityIn10min); READ_CHAN(rainIntensityIn30min);
        READ_CHAN(currentTyreSet); READ_CHAN(strategyTyreSet); READ_CHAN(gapAhead); READ_CHAN(gapBehind);
        #undef READ_CHAN
    }

    void read_metadata_from_stream(std::istream& in) {
        AC_LapDataChannels::read_metadata_from_stream(in);
        
        uint64_t lap_size = timestamp.size();
        if (lap_size == 0) return;
        
        #define SKIP_CHAN(vec) in.seekg(lap_size * sizeof(vec[0]), std::ios::cur)
        SKIP_CHAN(mz_FL); SKIP_CHAN(mz_FR); SKIP_CHAN(mz_RL); SKIP_CHAN(mz_RR);
        SKIP_CHAN(fx_FL); SKIP_CHAN(fx_FR); SKIP_CHAN(fx_RL); SKIP_CHAN(fx_RR);
        SKIP_CHAN(fy_FL); SKIP_CHAN(fy_FR); SKIP_CHAN(fy_RL); SKIP_CHAN(fy_RR);
        SKIP_CHAN(slipRatio_FL); SKIP_CHAN(slipRatio_FR); SKIP_CHAN(slipRatio_RL); SKIP_CHAN(slipRatio_RR);
        SKIP_CHAN(slipAngle_FL); SKIP_CHAN(slipAngle_FR); SKIP_CHAN(slipAngle_RL); SKIP_CHAN(slipAngle_RR);
        
        SKIP_CHAN(tcinAction); SKIP_CHAN(absInAction);
        
        SKIP_CHAN(suspensionDamage_FL); SKIP_CHAN(suspensionDamage_FR); SKIP_CHAN(suspensionDamage_RL); SKIP_CHAN(suspensionDamage_RR);
        SKIP_CHAN(tyreTemp_FL); SKIP_CHAN(tyreTemp_FR); SKIP_CHAN(tyreTemp_RL); SKIP_CHAN(tyreTemp_RR);
        SKIP_CHAN(waterTemp);
        
        SKIP_CHAN(brakePressure_FL); SKIP_CHAN(brakePressure_FR); SKIP_CHAN(brakePressure_RL); SKIP_CHAN(brakePressure_RR);
        SKIP_CHAN(padLife_FL); SKIP_CHAN(padLife_FR); SKIP_CHAN(padLife_RL); SKIP_CHAN(padLife_RR);
        SKIP_CHAN(discLife_FL); SKIP_CHAN(discLife_FR); SKIP_CHAN(discLife_RL); SKIP_CHAN(discLife_RR);
        
        SKIP_CHAN(kerbVibration); SKIP_CHAN(slipVibrations); SKIP_CHAN(gVibrations); SKIP_CHAN(absVibrations);
        
        SKIP_CHAN(status); SKIP_CHAN(session); 
        SKIP_CHAN(position); 
        SKIP_CHAN(sessionTimeLeft); SKIP_CHAN(isInPit); SKIP_CHAN(currentSectorIndex); SKIP_CHAN(lastSectorTime);
        SKIP_CHAN(carCoordinates); SKIP_CHAN(carID); SKIP_CHAN(penaltyTime); SKIP_CHAN(flag); SKIP_CHAN(idealLineOn); SKIP_CHAN(isInPitLane);
        SKIP_CHAN(surfaceGrip); SKIP_CHAN(mandatoryPitDone); SKIP_CHAN(windSpeed); SKIP_CHAN(windDirection);

        SKIP_CHAN(TC); SKIP_CHAN(TCCUT); SKIP_CHAN(EngineMap); SKIP_CHAN(ABS); SKIP_CHAN(exhaustTemperature);
        SKIP_CHAN(activeCars); SKIP_CHAN(playerCarID); SKIP_CHAN(penalty);
        SKIP_CHAN(isSetupMenuVisible); SKIP_CHAN(mainDisplayIndex); SKIP_CHAN(secondaryDisplyIndex);
        SKIP_CHAN(fuelXLap); SKIP_CHAN(rainLights); SKIP_CHAN(flashingLights); SKIP_CHAN(lightsStage);
        SKIP_CHAN(wiperLV); SKIP_CHAN(driverStintTotalTimeLeft); SKIP_CHAN(driverStintTimeLeft); SKIP_CHAN(rainTyres);
        SKIP_CHAN(sessionIndex); SKIP_CHAN(usedFuel); SKIP_CHAN(deltaLapTime); SKIP_CHAN(iDeltaLapTime);
        SKIP_CHAN(estimatedLapTime); SKIP_CHAN(iEstimatedLapTime); SKIP_CHAN(isDeltaPositive); SKIP_CHAN(iSplit);
        SKIP_CHAN(isValidLap); SKIP_CHAN(fuelEstimatedLaps); SKIP_CHAN(trackStatus); SKIP_CHAN(missingMandatoryPits);
        SKIP_CHAN(Clock); SKIP_CHAN(directionLightsLeft); SKIP_CHAN(directionLightsRight);
        SKIP_CHAN(GlobalYellow); SKIP_CHAN(GlobalYellow1); SKIP_CHAN(GlobalYellow2); SKIP_CHAN(GlobalYellow3);
        SKIP_CHAN(GlobalWhite); SKIP_CHAN(GlobalGreen); SKIP_CHAN(GlobalChequered); SKIP_CHAN(GlobalRed);
        SKIP_CHAN(mfdTyreSet); SKIP_CHAN(mfdFuelToAdd); SKIP_CHAN(mfdTyrePressureLF); SKIP_CHAN(mfdTyrePressureRF);
        SKIP_CHAN(mfdTyrePressureLR); SKIP_CHAN(mfdTyrePressureRR); SKIP_CHAN(trackGripStatus);
        SKIP_CHAN(rainIntensity); SKIP_CHAN(rainIntensityIn10min); SKIP_CHAN(rainIntensityIn30min);
        SKIP_CHAN(currentTyreSet); SKIP_CHAN(strategyTyreSet); SKIP_CHAN(gapAhead); SKIP_CHAN(gapBehind);
        #undef SKIP_CHAN
    }
};

#pragma pack(pop)
