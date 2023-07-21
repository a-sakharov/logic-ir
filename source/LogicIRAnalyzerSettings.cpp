#include "LogicIRAnalyzerSettings.h"
#include <AnalyzerHelpers.h>


LogicIRAnalyzerSettings::LogicIRAnalyzerSettings()
    :   mInputChannel(UNDEFINED_CHANNEL),
        mActiveFrequencyHighHz(38000),
        mActiveFrequencyLowHz(37000),
        mTimeSlotUs(575),
        mOutputType(OUTPUT_BINARY),
        mSignalStartTimeUs(9000),
        mSignalInverted(false)
{
    mInputChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
    mInputChannelInterface->SetTitleAndTooltip("IR", "IR data analyzing");
    mInputChannelInterface->SetChannel(mInputChannel);

    mActiveFrequencyLowInterface.reset(new AnalyzerSettingInterfaceInteger());
    mActiveFrequencyLowInterface->SetTitleAndTooltip("Active frequency (Hz), min", "Frequency used for signal, hertz");
    mActiveFrequencyLowInterface->SetMax(1000000);
    mActiveFrequencyLowInterface->SetMin(1);
    mActiveFrequencyLowInterface->SetInteger(mActiveFrequencyLowHz);

    mActiveFrequencyHighInterface.reset(new AnalyzerSettingInterfaceInteger());
    mActiveFrequencyHighInterface->SetTitleAndTooltip("Active frequency (Hz), max", "Frequency used for signal, hertz");
    mActiveFrequencyHighInterface->SetMax(1000000);
    mActiveFrequencyHighInterface->SetMin(1);
    mActiveFrequencyHighInterface->SetInteger(mActiveFrequencyHighHz);

    mSlotTimeInterface.reset(new AnalyzerSettingInterfaceInteger());
    mSlotTimeInterface->SetTitleAndTooltip("Slot time (us)", "Time for one bit");
    mSlotTimeInterface->SetMax(1000000);
    mSlotTimeInterface->SetMin(1);
    mSlotTimeInterface->SetInteger(mTimeSlotUs);

    mAnalysisOutputTypeInterface.reset(new AnalyzerSettingInterfaceNumberList());
    mAnalysisOutputTypeInterface->SetTitleAndTooltip("Output type", "How collected data should be formatted?");
    mAnalysisOutputTypeInterface->AddNumber(OUTPUT_BINARY, "Binary", "Ones and zeros");
    //mAnalysisOutputTypeInterface->AddNumber(OUTPUT_U8_LSF, "U8, least significant bit first", "Bytes");
    //mAnalysisOutputTypeInterface->AddNumber(OUTPUT_U8_MSF, "U8, most significant bit first", "Bytes");
    mAnalysisOutputTypeInterface->SetNumber(mOutputType);

    mSignalStartTimeInterface.reset(new AnalyzerSettingInterfaceInteger());
    mSignalStartTimeInterface->SetTitleAndTooltip("Signal start time (us)", "Special transmission at transmission start");
    mSignalStartTimeInterface->SetMax(1000000);
    mSignalStartTimeInterface->SetMin(0);
    mSignalStartTimeInterface->SetInteger(mSignalStartTimeUs);
    
    
    //mSignalInvertedInterface.reset(new AnalyzerSettingInterfaceBool());
    //mSignalInvertedInterface->SetTitleAndTooltip("Input signal inverted", "Invert signal back while processing");
    ////mSignalInvertedInterface->SetCheckBoxText("Invert signal");
    //mSignalInvertedInterface->SetValue(mSignalInverted);
    

    AddInterface(mInputChannelInterface.get());
    AddInterface(mActiveFrequencyLowInterface.get());
    AddInterface(mActiveFrequencyHighInterface.get());
    AddInterface(mSlotTimeInterface.get());
    AddInterface(mAnalysisOutputTypeInterface.get());
    AddInterface(mSignalStartTimeInterface.get());
    //AddInterface(mSignalInvertedInterface.get());

    //AddExportOption(0, "Export as csv file");
    //AddExportExtension(0, "csv", "csv");

    ClearChannels();
    AddChannel(mInputChannel, "IR", false);
}

LogicIRAnalyzerSettings::~LogicIRAnalyzerSettings()
{
}

bool LogicIRAnalyzerSettings::SetSettingsFromInterfaces()
{
    mInputChannel = mInputChannelInterface->GetChannel();
    mActiveFrequencyLowHz = mActiveFrequencyLowInterface->GetInteger();
    mActiveFrequencyHighHz = mActiveFrequencyHighInterface->GetInteger();
    mTimeSlotUs = mSlotTimeInterface->GetInteger();
    mOutputType = (int)mAnalysisOutputTypeInterface->GetNumber();
    mSignalStartTimeUs = mSignalStartTimeInterface->GetInteger();
    //mSignalInverted = mSignalInvertedInterface->GetValue();

    ClearChannels();
    AddChannel(mInputChannel, "IR", true);

    return true;
}

void LogicIRAnalyzerSettings::UpdateInterfacesFromSettings()
{
    mInputChannelInterface->SetChannel(mInputChannel);
    mActiveFrequencyHighInterface->SetInteger(mActiveFrequencyHighHz);
    mActiveFrequencyLowInterface->SetInteger(mActiveFrequencyLowHz);
    mSlotTimeInterface->SetInteger(mTimeSlotUs);
    mAnalysisOutputTypeInterface->SetNumber(mOutputType);
    mSignalStartTimeInterface->SetInteger(mSignalStartTimeUs);
    //mSignalInvertedInterface->SetValue(mSignalInverted);
}

void LogicIRAnalyzerSettings::LoadSettings(const char *settings)
{
    SimpleArchive text_archive;
    text_archive.SetString(settings);

    text_archive >> mInputChannel;
    text_archive >> mActiveFrequencyHighHz;
    text_archive >> mActiveFrequencyLowHz;
    text_archive >> mTimeSlotUs;
    text_archive >> mOutputType;
    text_archive >> mSignalStartTimeUs;
    text_archive >> mSignalInverted;

    ClearChannels();
    AddChannel(mInputChannel, "IR", true);

    UpdateInterfacesFromSettings();
}

const char *LogicIRAnalyzerSettings::SaveSettings()
{
    SimpleArchive text_archive;

    text_archive << mInputChannel;
    text_archive << mActiveFrequencyHighHz;
    text_archive << mActiveFrequencyLowHz;
    text_archive << mTimeSlotUs;
    text_archive << mOutputType;
    text_archive << mSignalStartTimeUs;
    text_archive << mSignalInverted;

    return SetReturnString(text_archive.GetString());
}
