#include "LogicIRSimulationDataGenerator.h"
#include "LogicIRAnalyzerSettings.h"

#include <AnalyzerHelpers.h>

LogicIRSimulationDataGenerator::LogicIRSimulationDataGenerator()
{
    srand((unsigned int)time(NULL));
}

LogicIRSimulationDataGenerator::~LogicIRSimulationDataGenerator()
{
}

void LogicIRSimulationDataGenerator::Initialize(U32 simulation_sample_rate,
                                            LogicIRAnalyzerSettings *settings)
{
    mSimulationSampleRateHz = simulation_sample_rate;
    mSettings = settings;

    mLogicIRSimulationData.SetChannel(mSettings->mInputChannel);
    mLogicIRSimulationData.SetSampleRate(simulation_sample_rate);
    mLogicIRSimulationData.SetInitialBitState(BIT_LOW);
}

U32 LogicIRSimulationDataGenerator::GenerateSimulationData(U64 largest_sample_requested,
                                                       U32 sample_rate,
                                                       SimulationChannelDescriptor **simulation_channel)
{
    U64 adjusted_largest_sample_requested = AnalyzerHelpers::AdjustSimulationTargetSample(largest_sample_requested, sample_rate, mSimulationSampleRateHz);

    U32 clocks_used = 0;
    int i;
    int payload_length;
    U32 slot_clocks = mSettings->mTimeSlotUs * (mSimulationSampleRateHz / 1000000);

    while (clocks_used < adjusted_largest_sample_requested)
    {
        clocks_used += GenerateStart();

        payload_length = (rand() % 10) * 8;
        for (i=0; i< payload_length; ++i)
        {
            if (rand() & 1)
            {
                clocks_used += GenerateActive(slot_clocks);
                clocks_used += GeneratePassive(slot_clocks);
            }
            else
            {
                clocks_used += GeneratePassive(slot_clocks);
            }
        }

        clocks_used += GeneratePassive((mSimulationSampleRateHz / 100) * (rand() % 50));//pause
    }

    *simulation_channel = &mLogicIRSimulationData;
    return 1;
}

U32 LogicIRSimulationDataGenerator::GenerateStart()
{
    return GenerateActive(mSettings->mSignalStartTimeUs * (mSimulationSampleRateHz / 1000000) + 100);
}

U32 LogicIRSimulationDataGenerator::GenerateActive(U32 clocks)
{
    U32 clocks_used = 0;
    U32 full_phase_clocks = mSimulationSampleRateHz / mSettings->mActiveFrequencyLowHz;
    U32 hight_clocks = full_phase_clocks/2;
    U32 low_clocks = full_phase_clocks - hight_clocks;

    while (clocks > clocks_used)
    {
        mLogicIRSimulationData.TransitionIfNeeded(BIT_HIGH);
        mLogicIRSimulationData.Advance(hight_clocks); clocks_used += hight_clocks;

        mLogicIRSimulationData.TransitionIfNeeded(BIT_LOW);
        mLogicIRSimulationData.Advance(low_clocks); clocks_used += low_clocks;
    }

    mLogicIRSimulationData.TransitionIfNeeded(BIT_LOW);

    return clocks_used;
}

U32 LogicIRSimulationDataGenerator::GeneratePassive(U32 clocks)
{
    mLogicIRSimulationData.Advance(clocks);

    return clocks;
}
