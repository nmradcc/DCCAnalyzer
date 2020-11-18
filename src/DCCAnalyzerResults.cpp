#include "DCCAnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "DCCAnalyzer.h"
#include "DCCAnalyzerSettings.h"
#include <iostream>
#include <sstream>
#include <stdio.h>

DCCAnalyzerResults::DCCAnalyzerResults(DCCAnalyzer *analyzer, DCCAnalyzerSettings *settings)
    :   AnalyzerResults(),
        mSettings(settings),
        mAnalyzer(analyzer)
{
}

DCCAnalyzerResults::~DCCAnalyzerResults()
{
}


void DCCAnalyzerResults::GenerateBubbleText(U64 frame_index, Channel & /*channel*/, DisplayBase display_base)   //unrefereced vars commented out to remove warnings.
{
    //we only need to pay attention to 'channel' if we're making bubbles for more than one channel (as set by AddChannelBubblesWillAppearOn)
    ClearResultStrings();
    Frame frame = GetFrame(frame_index);

	bool framing_error = ((frame.mFlags & FRAMING_ERROR_FLAG) != 0);
	bool packet_error = ((frame.mFlags & PACKET_ERROR_FLAG) != 0);

    char result_str[128];
	switch ((eFrameType)frame.mType)
	{
	case FRAME_PREAMBLE:
		AddResultString("P");
		AddResultString("PREAMBLE");
		snprintf(result_str, sizeof(result_str), "%llu Preamble Bits ", frame.mData1);
        AddResultString(result_str, framing_error ? "f" : "", packet_error ? "x" : "");
		break;
	case FRAME_PSBIT:
        AddResultString("S");
		AddResultString("PSBIT");
        snprintf(result_str, sizeof(result_str), "Packet Start Bit ");
        AddResultString(result_str, framing_error ? "f" : "", packet_error ? "x" : "");
		break;
    case FRAME_ADBYTE:
        AddResultString("A");
        AddResultString("ADBYTE");
        snprintf(result_str, sizeof(result_str), "Address Data Byte: %#02llx ", frame.mData1);
        AddResultString(result_str, framing_error ? "f" : "", packet_error ? "x" : "");
        break;
    case FRAME_DSBIT:
        AddResultString("S");
        AddResultString("DSBIT");
        snprintf(result_str, sizeof(result_str), "Data Start Bit ");
        AddResultString(result_str, framing_error ? "f" : "", packet_error ? "x" : "");
        break;
	case FRAME_DBYTE:
		AddResultString("D");
		AddResultString("DBYTE");
		snprintf(result_str, sizeof(result_str), "Data Byte: %#02llx ", frame.mData1);
		AddResultString(result_str, framing_error ? "f" : "", packet_error ? "x" : "");
		break;
	case FRAME_EDBYTE:
		AddResultString("E");
		AddResultString("EDBYTE");
		snprintf(result_str, sizeof(result_str), "Error Detection Byte: %#02llx ", frame.mData1);
		AddResultString(result_str, framing_error ? "f" : "", packet_error ? "x" : "");
		break;
    case FRAME_PEBIT:
        AddResultString("N");
        AddResultString("PEBIT");
        snprintf(result_str, sizeof(result_str), "Packet End Bit ");
        AddResultString(result_str, framing_error ? "f" : "", packet_error ? "x" : "");
        break;
    case FRAME_END_ERR:
        packet_error = true;
	default:
        AddResultString("X");
        AddResultString("ERROR");
        snprintf(result_str, sizeof(result_str), "Error Detected ");
        AddResultString(result_str, framing_error ? "f" : "", packet_error ? "x" : "");
        break;
	}
}

void DCCAnalyzerResults::GenerateExportFile(const char *file, DisplayBase display_base, U32 /*export_type_user_id*/)
{

	//export_type_user_id is only important if we have more than one export type.
    std::stringstream ss;

    U64 trigger_sample = mAnalyzer->GetTriggerSample();
    U32 sample_rate = mAnalyzer->GetSampleRate();
    U64 num_frames = GetNumFrames();

    void *f = AnalyzerHelpers::StartFile(file);

    ss << "Time [s], Data" << std::endl;

    for (U32 i = 0; i < num_frames; i++) {
        Frame frame = GetFrame(i);

        U64 packet_id = GetPacketContainingFrameSequential(i);

        char time_str[128];
        AnalyzerHelpers::GetTimeString(frame.mStartingSampleInclusive, trigger_sample, sample_rate, time_str, 128);

        char number_str[128];
        AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 8, number_str, 128);

        if (packet_id == INVALID_RESULT_INDEX) {
            ss << time_str << "," << "" << ",";
        } else {
            ss << time_str << "," << number_str << ",";
        }

        if ((frame.mFlags & FRAMING_ERROR_FLAG) != 0) {
            ss << "Error";
        }

        ss << std::endl;

        AnalyzerHelpers::AppendToFile((U8 *)ss.str().c_str(), ss.str().length(), f);
        ss.str(std::string());

        if (UpdateExportProgressAndCheckForCancel(i, num_frames) == true) {
            AnalyzerHelpers::EndFile(f);
            return;
        }
    }

    UpdateExportProgressAndCheckForCancel(num_frames, num_frames);
    AnalyzerHelpers::EndFile(f);

}

void DCCAnalyzerResults::GenerateFrameTabularText(U64 frame_index, DisplayBase display_base)
{

    char result_str[128];
    U8 ft;

    ClearTabularText();
    Frame frame = GetFrame(frame_index);
    bool framing_error = ((frame.mFlags & FRAMING_ERROR_FLAG) != 0);
    bool packet_error = ((frame.mFlags & PACKET_ERROR_FLAG) != 0);
    ft = frame.mType;

    switch (ft)
    {
    case FRAME_PREAMBLE:
        snprintf(result_str, sizeof(result_str), "%llu Preamble Bits", frame.mData1);
        break;
    case FRAME_PSBIT:
        snprintf(result_str, sizeof(result_str), "PSBit");
        break;
    case FRAME_ADBYTE:
        snprintf(result_str, sizeof(result_str), "Address Data Byte: %#02llx", frame.mData1);
        break;
    case FRAME_DSBIT:
        snprintf(result_str, sizeof(result_str), "DSBit");
        break;
    case FRAME_DBYTE:
        snprintf(result_str, sizeof(result_str), "Instruction Data Byte: %#02llx", frame.mData1);
        break;
    case FRAME_EDBYTE:
        snprintf(result_str, sizeof(result_str), "Error Detection Byte: %#02llx", frame.mData1);
        break;
    case FRAME_PEBIT:
        snprintf(result_str, sizeof(result_str), "PEBit");
        break;
    default:
		snprintf(result_str, sizeof(result_str), "Error");
        break;
    }
    AddTabularText(result_str);
    snprintf(result_str, sizeof(result_str), " %s %s", framing_error ? "f" : "", packet_error ? "x" : "");
    AddTabularText(result_str);

}

void DCCAnalyzerResults::GeneratePacketTabularText(U64 /*packet_id*/, DisplayBase /*display_base*/)    //unrefereced vars commented out to remove warnings.
{
/*
    ClearResultStrings();
    AddResultString("not supported");
*/
}

void DCCAnalyzerResults::GenerateTransactionTabularText(U64 /*transaction_id*/, DisplayBase /*display_base*/)    //unrefereced vars commented out to remove warnings.
{
/*
	ClearResultStrings();
    AddResultString("not supported");
*/
}
