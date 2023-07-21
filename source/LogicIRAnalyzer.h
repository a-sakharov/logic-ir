#ifndef LOGIC_IR_ANALYZER_H
#define LOGIC_IR_ANALYZER_H

#include <Analyzer.h>
#include "LogicIRAnalyzerResults.h"
#include "LogicIRSimulationDataGenerator.h"

class LogicIRAnalyzerSettings;
class ANALYZER_EXPORT LogicIRAnalyzer : public Analyzer2
{
public:
    LogicIRAnalyzer();
    virtual ~LogicIRAnalyzer();
    virtual void WorkerThread();
    virtual void SetupResults();

    virtual U32 GenerateSimulationData(U64 newest_sample_requested,
                                       U32 sample_rate,
                                       SimulationChannelDescriptor **simulation_channels);
    virtual U32 GetMinimumSampleRateHz();

    virtual const char *GetAnalyzerName() const;
    virtual bool NeedsRerun();
#pragma warning( push )
#pragma warning( disable : 4251 ) // warning C4251: 'SerialAnalyzer::<...>' : class <...> needs to have dll-interface to be used by clients of class

private:
    double GetPercent(double value, double percent);
    bool TestActiveSignal(U64& ticks_length);

protected: //vars
    std::unique_ptr< LogicIRAnalyzerSettings > mSettings;
    std::unique_ptr< LogicIRAnalyzerResults > mResults;
    AnalyzerChannelData *mIR_Clannel;

    LogicIRSimulationDataGenerator mSimulationDataGenerator;
    bool mSimulationInitilized;

    double SamplesToUs(U64 samples);
    U64 UsToSamples(double us);

    U32 mSampleRateHz;
#pragma warning( pop )
};

extern "C" ANALYZER_EXPORT const char *__cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer *__cdecl CreateAnalyzer();
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer(Analyzer *analyzer);

#endif
