#ifndef LOGIC_IR_ANALYZER_RESULTS
#define LOGIC_IR_ANALYZER_RESULTS

#include <AnalyzerResults.h>

class LogicIRAnalyzer;
class LogicIRAnalyzerSettings;

class LogicIRAnalyzerResults : public AnalyzerResults
{
public:
    LogicIRAnalyzerResults(LogicIRAnalyzer *analyzer, LogicIRAnalyzerSettings *settings);
    virtual ~LogicIRAnalyzerResults();

    virtual void GenerateBubbleText(U64 frame_index, Channel &channel, DisplayBase display_base);
    virtual void GenerateExportFile(const char *file, DisplayBase display_base, U32 export_type_user_id);

    virtual void GenerateFrameTabularText(U64 frame_index, DisplayBase display_base);
    virtual void GeneratePacketTabularText(U64 packet_id, DisplayBase display_base);
    virtual void GenerateTransactionTabularText(U64 transaction_id, DisplayBase display_base);

protected:  //vars
    LogicIRAnalyzerSettings *mSettings;
    LogicIRAnalyzer *mAnalyzer;
};

#endif 
