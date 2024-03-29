
#pragma once

#include "libindi/indidriver.h"
#include "libindi/indiguiderinterface.h"
#include "libindi/inditelescope.h"
#include "libindi/indipropertytext.h"
#include "libindi/indipropertynumber.h"
#include "libindi/inditimer.h"
#include "libindi/indicom.h"
#include "libindi/alignment/DriverCommon.h"
#include "libindi/alignment/AlignmentSubsystemForDrivers.h"
#include "slew_drive.h"
#include <memory>

#define TM_LOG telemetryLogger
#define LOGF_TM(fmt, ...)  DEBUGF(telemetryLogger, (fmt), __VA_ARGS__)
#define LOG_TM(txt)   DEBUG(telemetryLogger, (txt))
#define DEFAULT_TM_TICKS_PER_UPDATE 60
// #include "indielapsedtimer.h"
namespace LFAST
{

    // const double slewspeeds[] = {32.0, 64.0, 128.0, 256.0, 512.0};
    // constexpr unsigned int NUM_SLEW_SPEEDS = sizeof(slewspeeds) / sizeof(double);
    // static constexpr double FAST_SLEW_DEFAULT_DPS = SIDEREAL_RATE_DPS * DEFAULT_SLEW_MULT;
}

enum
{
    AXIS_AZ_VEL = AXIS_AZ + 2,
    AXIS_ALT_VEL = AXIS_ALT + 2
};

class LFAST_Mount : public INDI::Telescope,
                    public INDI::GuiderInterface,
                    public INDI::AlignmentSubsystem::AlignmentSubsystemForDrivers
{
public:
    LFAST_Mount();
    virtual ~LFAST_Mount();

    bool ISNewBLOB(const char *dev, const char *name, int sizes[], int blobsizes[], char *blobs[],
                   char *formats[], char *names[], int n) override;
    virtual bool ISNewNumber(const char *dev, const char *name, double values[], char *names[], int n) override;
    virtual bool ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int n) override;
    virtual bool ISNewText(const char *dev, const char *name, char *texts[], char *names[], int n) override;
    virtual const char *getDefaultName() override;
    virtual void TimerHit() override;

    virtual bool SetCurrentPark() override;
    virtual bool SetDefaultPark() override;
protected:
    /** \brief Called to initialize basic properties required all the time */
    virtual bool initProperties() override;
    /** \brief Called when connected state changes, to add/remove properties */
    virtual bool updateProperties() override;
    virtual void simulationTriggered(bool enable) override;
    ///////////////////////////////////////////////////////////////////////////////
    /// Communication Commands
    ///////////////////////////////////////////////////////////////////////////////
    /**
     * @brief Handshake Attempt communication with the mount.
     * @return true if successful, false otherwise.
     */
    virtual bool Handshake() override;

    /**
     * @brief ReadScopeStatus Query the mount status, coordinate, any status indicators, pier side..etc.
     * @return True if query is successful, false otherwise.
     */
    virtual bool ReadScopeStatus() override;

    bool Connect() override;
    bool Disconnect() override;

    ///////////////////////////////////////////////////////////////////////////////
    /// Pulse Guiding Commands
    ///////////////////////////////////////////////////////////////////////////////
    virtual IPState GuideNorth(uint32_t ms) override;
    virtual IPState GuideSouth(uint32_t ms) override;
    virtual IPState GuideEast(uint32_t ms) override;
    virtual IPState GuideWest(uint32_t ms) override;
    IPState GuideNS(int32_t ms);
    IPState GuideWE(int32_t ms);

    ///////////////////////////////////////////////////////////////////////////////
    /// Motions commands.
    ///////////////////////////////////////////////////////////////////////////////
    bool Abort() override;
    bool MoveNS(INDI_DIR_NS dir, TelescopeMotionCommand command) override;
    bool MoveWE(INDI_DIR_WE dir, TelescopeMotionCommand command) override;

    ///////////////////////////////////////////////////////////////////////////////
    /// GOTO & Sync commands
    ///////////////////////////////////////////////////////////////////////////////
    virtual bool Goto(double RA, double DE) override;
    virtual bool Sync(double RA, double DE) override;

    ///////////////////////////////////////////////////////////////////////////////
    /// Time, Date & Location commands.
    ///////////////////////////////////////////////////////////////////////////////
    virtual bool updateLocation(double latitude, double longitude, double elevation) override;

    ///////////////////////////////////////////////////////////////////////////////
    /// Parking commands
    ///////////////////////////////////////////////////////////////////////////////
    virtual bool Park() override;
    virtual bool UnPark() override;
    // virtual bool SetCurrentPark() override;
    // virtual bool SetDefaultPark() override;

    ///////////////////////////////////////////////////////////////////////////////
    /// Utility Functions
    ///////////////////////////////////////////////////////////////////////////////
    void updateTrackingTarget(double ra, double dec);
    bool SetSlewRate(int index) override;
    void initializeTimers();
    void terminateNSGuide();
    void terminateEWGuide();
    /**
     * @brief hexDump Helper function to print non-string commands to the logger so it is easier to debug
     * @param buf buffer to format the command into hex strings.
     * @param data the command
     * @param size length of the command in bytes.
     * @note This is called internally by sendCommand, no need to call it directly.
     */
    void hexDump(char *buf, const char *data, int size);
    // void logfTelemetry(const char* format, ...);
    void tmLogMountStates();
    virtual bool saveConfigItems(FILE *fp) override;
    virtual void ISGetProperties(const char *dev) override;

private:
    unsigned int TM_LOG{0};

    TelescopeStatus PrevTrackState{SCOPE_IDLE};
    
    std::unique_ptr<SlewDrive> AltitudeAxis;
    std::unique_ptr<SlewDrive> AzimuthAxis;

    // Tracking
    INDI::IEquatorialCoordinates m_SkyTrackingTarget{0, 0};
    INDI::IEquatorialCoordinates m_SkyGuideOffset{0, 0};
    INDI::IEquatorialCoordinates m_EqSkyGuideDelta{0, 0};
    INDI::IEquatorialCoordinates m_SkyCurrentRADE{0, 0};
    INDI::IHorizontalCoordinates m_MountAltAz{0, 0};

    // INDI::IHorizontalCoordinates m_HzSkyGuideRate{0, 0};

    // Tracing in timer tick
    int TraceThisTickCount{0};
    bool TraceThisTick{false};

    bool guideManeuverActive{false};
    bool manualMotionActive{false};

    ///////////////////////////////////////////////////////////////////////////////
    /// Additional Properties
    ///////////////////////////////////////////////////////////////////////////////
    // static constexpr const char *DetailedMountInfoPage { "Detailed Mount Information" };

    INDI::PropertyText ModbusCommPortTP{1};
    // INDI::PropertyText NtpServerTP{1};
    INDI::PropertyNumber AzAltCoordsNP{4};
    // INDI::PropertySwitch MountSlewRateSP{LFAST::NUM_SLEW_SPEEDS};
    INDI::PropertyNumber GuideRateNP{2};
    // bool gotoPending;
    // INDI::PropertyText TrackStateTP{1};

    // INDI::PropertyLight TrackStateLP{5};
    // INDI::PropertySwitch TrackStateSP{6};
    INDI::PropertySwitch HomeSP{1};
    bool homingRoutineActive;
    bool altHomingComplete;
    bool azHomingComplete;

    INDI::PropertyNumber TelemetryDownsampleNP{1};

    enum
    {
        SAVE_POSN_DISABLED,
        SAVE_POSN_ENABLED
    };
    // enum
    // {
    //     FULL_STOP,
    //     SLEWING,
    //     SLEWING_TO,
    //     SLEWING_FORWARD,
    //     HIGH_SPEED,
    //     NOT_INITIALISED
    // };
    // ISwitch AxisOneStateS[6];
    // ISwitchVectorProperty AxisOneStateSP;
    ///////////////////////////////////////////////////////////////////////////////
    /// Class Variables
    ///////////////////////////////////////////////////////////////////////////////
    // IGeographicCoordinates m_GeographicLocation{0, 0};
    INDI::Timer m_NSTimer;
    INDI::Timer m_WETimer;
    // INDI::ElapsedTimer m_TrackingRateTimer;
    // unsigned int DBG_SIMULATOR{0};

    ///////////////////////////////////////////////////////////////////////////////
    /// Helper Functions
    ///////////////////////////////////////////////////////////////////////////////
    INDI::IHorizontalCoordinates getTrackingTargetAltAzPosition();
    INDI::IHorizontalCoordinates getSiderealTargetAltAzRates();
    bool updatePointingCoordinates();
    INDI::IHorizontalCoordinates getHorizontalRates();
    double GetSlewRate();
    bool startHomingRoutine();
};

const std::string getDirString(INDI_DIR_NS dir)
{
    if (dir == DIRECTION_NORTH)
        return "DIRECTION_NORTH";
    else if (dir == DIRECTION_SOUTH)
        return "DIRECTION_SOUTH";
    else
        return "DIRECTION_ERROR";
}

const std::string getDirString(INDI_DIR_WE dir)
{
    if (dir == DIRECTION_WEST)
        return "DIRECTION_WEST";
    else if (dir == DIRECTION_EAST)
        return "DIRECTION_EAST";
    else
        return "DIRECTION_ERROR";
}

inline bool is_file_exist(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}

