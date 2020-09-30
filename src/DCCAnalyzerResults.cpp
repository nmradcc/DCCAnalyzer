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


char * DCCAnalyzerResults::ParseServiceMode(U8 cCmd, bool bLongPacket = true)
{
	switch ((cCmd >> 2) & 0x03)
	{
	case 0:
	case 1:
		sprintf(sParseBuf, "Verify AH:%x", cCmd & 0x03);
		break;
	case 2:
		sprintf(sParseBuf, "Bits AH:%x", cCmd & 0x03);
		break;
	case 3:
		sprintf(sParseBuf, "Write AH:%x", cCmd & 0x03);
		break;
	}

	return sParseBuf;
}

char * DCCAnalyzerResults::ParseCommand(U8 cCmd)
{
	U8 cSpeed = 0;
	char sSpeedTxt[10] = "";
	switch (cCmd >> 4)
	{
	case 0: // Decoder control
		switch (cCmd & 0x0F)
		{
		case 0: // reset
			sprintf(sParseBuf, "Reset");
			break;
		case 1: //
			sprintf(sParseBuf, "Hard Reset");
			break;
		case 2:
		case 3:
			sprintf(sParseBuf, "Factory Test");
			break;
		case 6:
		case 7:
			sprintf(sParseBuf, "Set Flags");
			break;
		case 10:
		case 11:
			sprintf(sParseBuf, "Set Adv Adr");
			break;
		case 15:  
			sprintf(sParseBuf, "Req Ack");
			break;
		default:
			sprintf(sParseBuf, "RESERVED");
			break;
		}
		break;
	case 1:  // consist control
		switch (cCmd & 0x0F)
		{
		case 2:
			sprintf(sParseBuf, "Set Consist FWD");
			break;
		case 3:
			sprintf(sParseBuf, "Set Consist REV");
			break;
		default:
			sprintf(sParseBuf, "RESERVED");
			break;
		}
		break;
	case 2:
		sprintf(sParseBuf, "RESERVED");
		break;
	case 3:
		switch (cCmd & 0x0F)
		{
		case 15:
			sprintf(sParseBuf, "Speed 128");
			break;
		case 14:
			sprintf(sParseBuf, "Restricted Speed");
			break;
		case 13:
			sprintf(sParseBuf, "Analog Function");
			break;
		default:
			sprintf(sParseBuf, "RESERVED");
			break;
		}
		break;

	case 4:
	case 5:
		cSpeed = ((cCmd & 0x0F) << 1) | ((cCmd >> 4) & 0x01);
		switch (cSpeed)
		{
		case 0:
		case 1:
			sprintf(sSpeedTxt, "STOP");
			break;
		case 2:
		case 3:
			sprintf(sSpeedTxt, "ESTOP");
			break;
		default:
			sprintf(sSpeedTxt, "%d", cSpeed - DCC_BASELINE_PACKET_SPEED_OFFSET);
		}
		sprintf(sParseBuf, "Speed REV %s", sSpeedTxt);
		break;
	case 6:
	case 7:
		cSpeed = ((cCmd & 0x0F) << 1) | ((cCmd >> 4) & 0x01);
		switch (cSpeed)
		{
		case 0:
		case 1:
			sprintf(sSpeedTxt, "STOP");
			break;
		case 2:
		case 3:
			sprintf(sSpeedTxt, "ESTOP");
			break;
		default:
			sprintf(sSpeedTxt, "%d", cSpeed - DCC_BASELINE_PACKET_SPEED_OFFSET);
		}
		sprintf(sParseBuf, "Speed FWD %s", sSpeedTxt);
		break;
	case 8:
	case 9:
		sprintf(sParseBuf, "Func grp 1 %s %0x", (cCmd & 0x10) ? "ON" : "OFF", (cCmd & 0x0F));
		break;
	case 10:
	case 11:
		sprintf(sParseBuf, "Func grp 2 %s %0x", (cCmd & 0x10) ? "L" : "H", (cCmd & 0x0F));
		break;
	case 12: // future expansion
	case 13:
		switch (cCmd & 0x1F)
		{
		case 0:
			sprintf(sParseBuf, "Binary State Long");
			break;
		case 0x1D:
			sprintf(sParseBuf, "Binary State Short");
			break;
		case 0x1E:
			sprintf(sParseBuf, "F13-F20 Control");
			break;
		case 0x1F:
			sprintf(sParseBuf, "F21-F28 Control");
			break;
		default:
			sprintf(sParseBuf, "RESERVED");
		}
		break;
	case 14: //CV access long form
		switch ((cCmd >> 2) & 0x03)
		{
		case 0:
			sprintf(sParseBuf, "CV Long RESERVED");
			break;
		case 1:
			sprintf(sParseBuf, "CV Long Verify %x",(cCmd & 0x03));
			break;
		case 2:
			sprintf(sParseBuf, "CV Long BITS %x", (cCmd & 0x03));
			break;
		case 3:
			sprintf(sParseBuf, "CV Long Write %x", (cCmd & 0x03));
		}
		break;
	case 15: //CV access short form
		switch (cCmd & 0x0F)
		{
		case 0:
			sprintf(sParseBuf, "CV Short N/A");
			break;
		case 2:
			sprintf(sParseBuf, "CV Short Accelerate");
			break;
		case 3:
			sprintf(sParseBuf, "CV Short Decelerate");
			break;
		case 9:
			sprintf(sParseBuf, "Decoder Lock");
			break;
		default:
			sprintf(sParseBuf, "CV Short RESERVED");
		}
		break;
	default:
		sprintf(sParseBuf, "------");
		break;

	}
	return sParseBuf;
}

char * DCCAnalyzerResults::ParseAccessory(U8 cCmd)
{
	if (cCmd & 0x80) {
		sprintf(sParseBuf, "AddrH %d Out %d %s", (~(cCmd >> 4)) & 0x07, (cCmd & 0x07), (cCmd & 0x08) ? "ON" : "OFF");
	} else {
		sprintf(sParseBuf, "AddrH %0x", cCmd ^ 0x70);
	}
	return sParseBuf;
}

void DCCAnalyzerResults::GenerateBubbleText(U64 frame_index, Channel & /*channel*/, DisplayBase display_base)   //unrefereced vars commented out to remove warnings.
{
    //we only need to pay attention to 'channel' if we're making bubbles for more than one channel (as set by AddChannelBubblesWillAppearOn)
    ClearResultStrings();
    Frame frame = GetFrame(frame_index);

	bool framing_error = ((frame.mFlags & FRAMING_ERROR_FLAG) != 0);
	bool checksum_error = ((frame.mFlags & CHECKSUM_ERROR_FLAG) != 0);

    char result_str[128];
	switch ((eFrameType)frame.mType)
	{
	case FRAME_PREAMBLE:
		AddResultString("P");
		AddResultString("Preamble");
		snprintf(result_str, sizeof(result_str), "Preamble bits: %llu ", frame.mData1);
		AddResultString(result_str,framing_error?"f":"",checksum_error?"x":"");
		break;
//	case FRAME_SBIT:
//		AddResultString("S");
//		AddResultString("S ", framing_error ? "f" : "", checksum_error ? "x" : "");
//		break;
	case FRAME_ADDR:
		AddResultString("A");
		AddResultString("Addr");
		switch (frame.mData1)
		{
		case 0:
			sprintf(result_str, "Address: Broadcast "); 
			break;
		case 0xFF:
			sprintf(result_str, "Address: Idle ");
			break;
		default:
			sprintf(result_str, "Address: %#02llx ", frame.mData1);
		}
		AddResultString(result_str, framing_error ? "f" : "", checksum_error ? "x" : "");
		break;
	case FRAME_EADDR:
		AddResultString("L");
		AddResultString("LAddr");
		snprintf(result_str, sizeof(result_str), "LAddress: %#02llx ", frame.mData1);
		AddResultString(result_str, framing_error ? "f" : "", checksum_error ? "x" : "");
		break;
	case FRAME_CMD:
		AddResultString("C");
		AddResultString("Cmd");
		snprintf(result_str, sizeof(result_str), "Command: %#02llx %s", frame.mData1, ParseCommand(frame.mData1));
		AddResultString(result_str, framing_error ? "f" : "", checksum_error ? "x" : "");
		break;
	case FRAME_ACC:
		AddResultString("K");
		AddResultString("Acc");
		snprintf(result_str, sizeof(result_str), "Accessory: %#02llx %s", frame.mData1, ParseAccessory(frame.mData1));
		AddResultString(result_str, framing_error ? "f" : "", checksum_error ? "x" : "");
		break;
	case FRAME_SVC:
		AddResultString("T");
		AddResultString("Svc");
		snprintf(result_str, sizeof(result_str), "Service: %s", ParseServiceMode(frame.mData1));
		AddResultString(result_str, framing_error ? "f" : "", checksum_error ? "x" : "");
		break;
	case FRAME_DATA:
		AddResultString("D");
		AddResultString("Data");
		snprintf(result_str, sizeof(result_str), "Data: %#02llx ", frame.mData1);
		AddResultString(result_str, framing_error ? "f" : "", checksum_error ? "x" : "");
		break;
	case FRAME_CHECKSUM:
		AddResultString("X");
		AddResultString("Chk");
		snprintf(result_str, sizeof(result_str), "Checksum: %#02llx ", frame.mData1);
		AddResultString(result_str, framing_error ? "f" : "", checksum_error ? "x" : "");
		break;
	default:
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
    bool checksum_error = ((frame.mFlags & CHECKSUM_ERROR_FLAG) != 0);
    ft = frame.mType;

    switch (ft)
    {
    case FRAME_PREAMBLE:
        snprintf(result_str, sizeof(result_str), "Preamble bits: %llu", frame.mData1);
        break;
//    case FRAME_SBIT:
//        snprintf(result_str, sizeof(result_str), "S");
//        break;
    case FRAME_ADDR:
        switch (frame.mData1)
        {
        case 0:
            snprintf(result_str, sizeof(result_str), "Address: Broadcast"); 
            break;
        case 0xFF:
            snprintf(result_str, sizeof(result_str), "Address: Idle");
            break;
        default:
            snprintf(result_str, sizeof(result_str), "Address: %#02llx", frame.mData1);
        }
        break;
    case FRAME_EADDR:
        snprintf(result_str, sizeof(result_str), "LAddress: %#02llx", frame.mData1);
        break;
    case FRAME_CMD:
        snprintf(result_str, sizeof(result_str), "Command: %#02llx %s", frame.mData1, ParseCommand(frame.mData1));
        break;
    case FRAME_ACC:
        snprintf(result_str, sizeof(result_str), "Accessory: %#02llx %s", frame.mData1, ParseAccessory(frame.mData1));
        break;
    case FRAME_SVC:
        snprintf(result_str, sizeof(result_str), "Service: %s", ParseServiceMode(frame.mData1));
        break;
    case FRAME_DATA:
        snprintf(result_str, sizeof(result_str), "Data: %#02llx", frame.mData1);
        break;
    case FRAME_CHECKSUM:
        snprintf(result_str, sizeof(result_str), "Checksum: %#02llx", frame.mData1);
        break;
    default:
		snprintf(result_str, sizeof(result_str), "Invalid Frame Type: %u", ft);
        break;
    }
    AddTabularText(result_str);
    snprintf(result_str, sizeof(result_str), " %s %s", framing_error ? "f" : "", checksum_error ? "x" : "");
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
