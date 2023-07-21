#include "LogicIRAnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "LogicIRAnalyzer.h"
#include "LogicIRAnalyzerSettings.h"
#include <fstream>

LogicIRAnalyzerResults::LogicIRAnalyzerResults(LogicIRAnalyzer *analyzer, LogicIRAnalyzerSettings *settings)
    :   AnalyzerResults(),
        mSettings(settings),
        mAnalyzer(analyzer)
{
}

LogicIRAnalyzerResults::~LogicIRAnalyzerResults()
{
}

void LogicIRAnalyzerResults::GenerateBubbleText(U64 frame_index, Channel &channel, DisplayBase display_base)
{
    Frame frame = GetFrame(frame_index);
    ClearResultStrings();

    char number_str[128] = { 0 };
    if (frame.mFlags)
    {
        snprintf(number_str, sizeof(number_str), "Start");
    }
    else
    {
        U64 i;
        for (i = 0; i < frame.mData2; ++i)
        {
            snprintf(number_str + strlen(number_str), sizeof(number_str) - strlen(number_str), "%llu", frame.mData1);
        }
    }

    AddResultString(number_str);   
}

void LogicIRAnalyzerResults::GenerateExportFile(const char *file, DisplayBase display_base, U32 export_type_user_id)
{
    std::ofstream file_stream(file, std::ios::out);

    U64 num_frames = GetNumFrames();
    for (U32 i = 0; i < num_frames; i++) 
    {
        Frame frame = GetFrame(i);

        char number_str[128] = { 0 };
        if (frame.mFlags)
        {
            snprintf(number_str, sizeof(number_str), "\n[Start]");
        }
        else
        {
            U64 i;
            for (i = 0; i < frame.mData2; ++i)
            {
                snprintf(number_str + strlen(number_str), sizeof(number_str) - strlen(number_str), "%llu", frame.mData1);
            }
        }

        file_stream << number_str;

        if (UpdateExportProgressAndCheckForCancel(i, num_frames) == true) 
        {
            file_stream.close();
            return;
        }
    }

    file_stream.close();
}

void LogicIRAnalyzerResults::GenerateFrameTabularText(U64 frame_index, DisplayBase display_base)
{
    Frame frame = GetFrame(frame_index);
    ClearTabularText();

    char number_str[128] = {0};
    
    if (frame.mFlags)
    {
        snprintf(number_str, sizeof(number_str), "\n[Start]");
    }
    else
    {
        U64 i;
        for (i = 0; i < frame.mData2; ++i)
        {
            snprintf(number_str + strlen(number_str), sizeof(number_str) - strlen(number_str), "%llu", frame.mData1);
        }
    }

#ifdef NDEBUG
    AddTabularText(number_str);
#else
    AddTabularText(number_str, "-");
#endif
}

void LogicIRAnalyzerResults::GeneratePacketTabularText(U64 packet_id, DisplayBase display_base)
{
    ClearResultStrings();
    AddResultString("not supported");
}

void LogicIRAnalyzerResults::GenerateTransactionTabularText(U64 transaction_id, DisplayBase display_base)
{
    ClearResultStrings();
    AddResultString("not supported");
}
