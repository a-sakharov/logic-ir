#ifndef LOGIC_IR_SIMULATION_DATA_GENERATOR
#define LOGIC_IR_SIMULATION_DATA_GENERATOR

#include <random>

#include <AnalyzerHelpers.h>

#include <SimulationChannelDescriptor.h>
class LogicIRAnalyzerSettings;

class LogicIRSimulationDataGenerator
{
public:
    LogicIRSimulationDataGenerator();
    ~LogicIRSimulationDataGenerator();

    void Initialize(U32 simulation_sample_rate, LogicIRAnalyzerSettings *settings);
    U32 GenerateSimulationData(U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor **simulation_channel);

private:
    U32 GenerateStart();
    U32 GenerateActive(U32 clocks);
    U32 GeneratePassive(U32 clocks);

protected:
    LogicIRAnalyzerSettings *mSettings;
    U32 mSimulationSampleRateHz;

protected:
    SimulationChannelDescriptor mLogicIRSimulationData;
    ClockGenerator mClockGenerator;

};
#endif
