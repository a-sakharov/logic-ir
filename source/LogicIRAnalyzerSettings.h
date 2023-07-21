#ifndef LOGIC_IR_ANALYZER_SETTINGS
#define LOGIC_IR_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

#define ANALYSIS_WIDTH -666-1488
#define OUTPUT_BINARY  1
#define OUTPUT_U8_LSF  2
#define OUTPUT_U8_MSF  3

class LogicIRAnalyzerSettings : public AnalyzerSettings
{
public:
    LogicIRAnalyzerSettings();
    virtual ~LogicIRAnalyzerSettings();

    virtual bool SetSettingsFromInterfaces();
    void UpdateInterfacesFromSettings();
    virtual void LoadSettings(const char *settings);
    virtual const char *SaveSettings();


    Channel mInputChannel;
    int mActiveFrequencyHighHz;
    int mActiveFrequencyLowHz;
    int mTimeSlotUs;
    int mOutputType;
    int mSignalStartTimeUs;
    bool mSignalInverted;

protected:
    std::unique_ptr< AnalyzerSettingInterfaceChannel >    mInputChannelInterface; 
    std::unique_ptr< AnalyzerSettingInterfaceInteger >    mActiveFrequencyHighInterface;
    std::unique_ptr< AnalyzerSettingInterfaceInteger >    mActiveFrequencyLowInterface;
    std::unique_ptr< AnalyzerSettingInterfaceInteger >    mSignalStartTimeInterface;
    std::unique_ptr< AnalyzerSettingInterfaceInteger >    mSlotTimeInterface;
    std::unique_ptr< AnalyzerSettingInterfaceNumberList > mAnalysisOutputTypeInterface;
    //std::unique_ptr< AnalyzerSettingInterfaceBool > mSignalInvertedInterface;
};

#endif
