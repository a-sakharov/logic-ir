#include <cmath>

#include "LogicIRAnalyzer.h"
#include "LogicIRAnalyzerSettings.h"
#include <AnalyzerChannelData.h>

LogicIRAnalyzer::LogicIRAnalyzer()
    :   Analyzer2(),
        mSettings(new LogicIRAnalyzerSettings()),
        mSimulationInitilized(false),
        mSampleRateHz(0)
{
    SetAnalyzerSettings(mSettings.get());
}

LogicIRAnalyzer::~LogicIRAnalyzer()
{
    KillThread();
}

void LogicIRAnalyzer::SetupResults()
{
    mResults.reset(new LogicIRAnalyzerResults(this, mSettings.get()));
    SetAnalyzerResults(mResults.get());
    mResults->AddChannelBubblesWillAppearOn(mSettings->mInputChannel);
}

void LogicIRAnalyzer::WorkerThread()
{
    mSampleRateHz = this->GetSampleRate();
    mIR_Clannel = GetAnalyzerChannelData(mSettings->mInputChannel);

    // Find the next full pulse for a clean start.
    mIR_Clannel->AdvanceToNextEdge();

    bool signal_decoding = false;

    U64 start_ticks = UsToSamples(mSettings->mSignalStartTimeUs);

    for (;;)
    {
        U64 start = mIR_Clannel->GetSampleNumber();
        U64 next = mIR_Clannel->GetSampleOfNextEdge();
        U64 length = next - start;
        BitState state = mIR_Clannel->GetBitState();
        double length_us = SamplesToUs(length);


        if (length_us > GetPercent(mSettings->mTimeSlotUs, 95)) // if signal stable for more than 1 slot
        {
            if (state == BIT_LOW)
            {
                //process low
#ifndef NDEBUG
                printf("Signal low for %f us [%llu -> %llu]\n", length_us, start, next);
#else
                printf("FuckFuckFuckFuckFuckFuckFuckFuckFuck\n");
#endif

                U64 slots = (U64)round(length_us / ((double)mSettings->mTimeSlotUs));

                if (slots < 10)//probably not a signal
                {
                    Frame frame;
                    frame.mStartingSampleInclusive = start;
                    frame.mEndingSampleInclusive = start + length - 1;
                    frame.mData1 = 0;
                    frame.mData2 = slots;
                    frame.mFlags = 0;

                    mResults->AddFrame(frame);
                    mResults->CommitResults();
                    ReportProgress(frame.mEndingSampleInclusive);
                }
            }
            else
            {
                //signal ended
#ifndef NDEBUG
                printf("Signal error state, long and high\n");
#endif
                signal_decoding = false;
            }

            mIR_Clannel->AdvanceToNextEdge();
        }
        else //signal less then 1 slot
        {
            U64 length_ticks;

            if (TestActiveSignal(length_ticks))//valid signal
            {

#ifndef NDEBUG
                printf("Signal active for %f us [%llu -> %llu]\n", length_us, start, start + length_ticks);
#endif

                if (start_ticks && start_ticks <= length_ticks)
                {
                    Frame frame;
                    frame.mStartingSampleInclusive = start;
                    frame.mEndingSampleInclusive = start + start_ticks - 1;
                    frame.mData1 = 0;
                    frame.mData2 = 0;
                    frame.mFlags = 1;

                    mResults->AddFrame(frame);
                    mResults->CommitResults();
                    ReportProgress(frame.mEndingSampleInclusive);

                    start += start_ticks;
                    length_ticks -= start_ticks;
                }

                length_us = SamplesToUs(length_ticks);

                if (length_us >= mSettings->mTimeSlotUs)
                {

                    Frame frame;
                    frame.mStartingSampleInclusive = start;
                    frame.mEndingSampleInclusive = start + length_ticks - 1;
                    frame.mData1 = 1;
                    frame.mData2 = (U64)round(length_us / ((double)mSettings->mTimeSlotUs));
                    frame.mFlags = 0;

                    mResults->AddFrame(frame);
                    mResults->CommitResults();
                    ReportProgress(frame.mEndingSampleInclusive);
                }
            }
            else//invalid signal
            {
                signal_decoding = false;
#ifndef NDEBUG
                printf("Signal error state, short and not active\n");
#endif
                mIR_Clannel->AdvanceToNextEdge();
            }
        }
    }
}

bool LogicIRAnalyzer::TestActiveSignal(U64 &ticks_length)
{
    U64 start = mIR_Clannel->GetSampleNumber();
    BitState state = mIR_Clannel->GetBitState();
    
    U64 length = mIR_Clannel->GetSampleOfNextEdge() - mIR_Clannel->GetSampleNumber();
    
    mIR_Clannel->AdvanceToNextEdge();
    length += mIR_Clannel->GetSampleOfNextEdge() - mIR_Clannel->GetSampleNumber();
    mIR_Clannel->AdvanceToAbsPosition(mIR_Clannel->GetSampleNumber() - 1);

    double length_us = SamplesToUs(length);
    double period_us_max = 1000000.0 / mSettings->mActiveFrequencyLowHz;
    double period_us_min = 1000000.0 / mSettings->mActiveFrequencyHighHz;


    if ((length_us < period_us_min) || (length_us > period_us_max))
    {
#ifndef NDEBUG
        printf("length_us too short in TestActiveSignal - %f\n", length_us);
#endif
        return false;
    }

    while ((length_us >= period_us_min) && (length_us <= period_us_max))
    {
        mIR_Clannel->AdvanceToNextEdge();
        length = mIR_Clannel->GetSampleOfNextEdge() - mIR_Clannel->GetSampleNumber();

        mIR_Clannel->AdvanceToNextEdge();
        length += mIR_Clannel->GetSampleOfNextEdge() - mIR_Clannel->GetSampleNumber();
        mIR_Clannel->AdvanceToAbsPosition(mIR_Clannel->GetSampleNumber() - 1);

        length_us = SamplesToUs(length);
    }

    ticks_length = mIR_Clannel->GetSampleNumber() - start;

    return true;
}

double LogicIRAnalyzer::GetPercent(double value, double percent)
{
    return (percent / 100.) * value;
}

bool LogicIRAnalyzer::NeedsRerun()
{
    return false;
}

U32 LogicIRAnalyzer::GenerateSimulationData(U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor **simulation_channels)
{
    if (mSimulationInitilized == false) 
    {
        mSimulationDataGenerator.Initialize(GetSimulationSampleRate(), mSettings.get());
        mSimulationInitilized = true;
    }

    return mSimulationDataGenerator.GenerateSimulationData(minimum_sample_index, device_sample_rate, simulation_channels);
}

U32 LogicIRAnalyzer::GetMinimumSampleRateHz()
{
    return 1000000;
}

const char *LogicIRAnalyzer::GetAnalyzerName() const
{
    return "IR";
}

const char *GetAnalyzerName()
{
    return "IR";
}

Analyzer *CreateAnalyzer()
{
    return new LogicIRAnalyzer();
}

void DestroyAnalyzer(Analyzer *analyzer)
{
    delete analyzer;
}

double LogicIRAnalyzer::SamplesToUs(U64 samples)
{
    return ((double)samples * 1000000.0) / (double)mSampleRateHz;
}

U64 LogicIRAnalyzer::UsToSamples(double us)
{
    return (U64)((us * (double)mSampleRateHz) / 1000000.0);
}
