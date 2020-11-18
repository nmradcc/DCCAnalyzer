#include "DCCAnalyzer.h"
#include "DCCAnalyzerSettings.h"
#include <AnalyzerChannelData.h>
#include <math.h>

#include <stdio.h>


DCCAnalyzer::DCCAnalyzer()
    : Analyzer2(),
      mSettings(new DCCAnalyzerSettings()),
      mSimulationInitilized(false)
{
    SetAnalyzerSettings(mSettings.get());
}

DCCAnalyzer::~DCCAnalyzer()
{
    KillThread();
}

void DCCAnalyzer::SetupResults()
{
    //Unlike the worker thread, this function is called from the GUI thread
    //we need to reset the Results object here because it is exposed for direct access by the GUI, and it can't be deleted from the WorkerThread

    mResults.reset(new DCCAnalyzerResults(this, mSettings.get()));
    SetAnalyzerResults(mResults.get());
    mResults->AddChannelBubblesWillAppearOn(mSettings->mInputChannel);
}

UINT DCCAnalyzer::LookaheadNextHBit(U64 *nSample)
{
	UINT nHBitLen = (UINT)(mDCC->GetSampleOfNextEdge() - *nSample);
	*nSample = mDCC->GetSampleOfNextEdge();
    if (nHBitLen >= mMin1hbit && nHBitLen <= mMax1hbit)
		return 1;
	else if (nHBitLen >= mMin0hbit && nHBitLen <= mMax0hbit)
		return 0;
	else
		return BIT_ERROR_FLAG; // bit error
}

bool DCCAnalyzer::CheckPacketEndHold(U64 *nSample)
{
	UINT nHBitLen = (UINT)(mDCC->GetSampleOfNextEdge() - *nSample);
	*nSample = mDCC->GetSampleOfNextEdge();
    if (nHBitLen >= mMinPEHold)
		return true;
	else
        return false;
}

UINT DCCAnalyzer::GetNextHBit(U64 *nSample)
{
	U64 nSampNumber = *nSample;
	mDCC->AdvanceToNextEdge();
	*nSample = mDCC->GetSampleNumber();
	UINT	nHBitLen = (UINT)(mDCC->GetSampleNumber() - nSampNumber);
    if (nHBitLen >= mMin1hbit && nHBitLen <= mMax1hbit)
		return 1;
    else if (nHBitLen >= mMin0hbit && nHBitLen <= mMax0hbit)
		return 0;
	else 
        return BIT_ERROR_FLAG; // bit error
}

//--------------------------------------------------------------
// Get the next bit (0 or 1).
// Alignment error: return 3;
// Framing and other error return 2;
UINT DCCAnalyzer::GetNextBit(U64 *nSample)
{
	U64 nTemp = *nSample;
	UINT nHBit1 = GetNextHBit(nSample);
	UINT nHBit2 = GetNextHBit(nSample);
    if ((nHBit1 > 1) || (nHBit2 > 1) || ((UINT)(*nSample - nTemp) > mMaxBitLen))
		return BIT_ERROR_FLAG;      // bit error
	else if (nHBit1 != nHBit2)
		return FRAMING_ERROR_FLAG;  // frame error
	else 
        return nHBit1;
}

void DCCAnalyzer::PostFrame(U64 nStartSample, U64 nEndSample, eFrameType ft, U8 Flags, U64 Data1 = 0, U64 Data2 =0 )
{
	Frame frame;
	frame.mStartingSampleInclusive = nStartSample;
	frame.mEndingSampleInclusive = nEndSample;
	frame.mData1 = Data1;
	frame.mType = ft;
	frame.mFlags = Flags;
	mResults->AddFrame(frame);

// This is a new feature for the Saleae Analyzer that enables the Python debugging interface

    FrameV2 framev2;

    switch (ft)
    {
    case FRAME_PREAMBLE:    // 0,           nHBitVal/2
        framev2.AddString("type", "preamble");                        
        framev2.AddByte("data", (U8)Data1);
        mResults->AddFrameV2( framev2, "preamble", nStartSample, nEndSample );
        break;
    case FRAME_PSBIT:        // 0
        framev2.AddString("type", "psbit");                        
        mResults->AddFrameV2( framev2, "psbit", nStartSample, nEndSample );
        break;
    case FRAME_ADBYTE:        // 0,           nVal
        framev2.AddString("type", "adbyte");                        
        framev2.AddByte("data", (U8)Data1);
        mResults->AddFrameV2( framev2, "adbyte", nStartSample, nEndSample );
        break;
    case FRAME_DSBIT:        // 0
        framev2.AddString("type", "dsbit");                        
        mResults->AddFrameV2( framev2, "dsbit", nStartSample, nEndSample );
        break;
    case FRAME_DBYTE:        // 0,           nVal
        framev2.AddString("type", "dbyte");                        
        framev2.AddByte("data", (U8)Data1);
        mResults->AddFrameV2( framev2, "dbyte", nStartSample, nEndSample );
        break;
    case FRAME_EDBYTE:    // nCurSample,  nVal
        framev2.AddString("type", "edbyte");                        
        framev2.AddByte("data", (U8)Data1);
        mResults->AddFrameV2( framev2, "edbyte", nStartSample, nEndSample );
        break;
    case FRAME_PEBIT:         // End of packet bit
        framev2.AddString("type", "pebit");                        
        mResults->AddFrameV2( framev2, "pebit", nStartSample, nEndSample );
        break;
    case FRAME_END_ERR:
        framev2.AddString("type", "error");                        
        mResults->AddFrameV2( framev2, "packet end error", nStartSample, nEndSample );
        break;
    default:
        framev2.AddString("type", "error");                        
        mResults->AddFrameV2( framev2, "error", nStartSample, nEndSample );
        break;
    }

    mResults->CommitResults();

}

void DCCAnalyzer::Setup()
{
    //
    // Sample Rate
    //
    mSampleRateHz = GetSampleRate();
    double dSamplesPerMicrosecond = (mSampleRateHz / 1000000.0);
    //
    // Use the mCalPPM setting to adjust the resolution of the measurements
    //
	double dMaxCorrection = 1.0 + mSettings->mCalPPM / 1000000.0;
	double dMinCorrection = 1.0 - mSettings->mCalPPM / 1000000.0;
    //
    // The following measurements are in microseconds
    //
	mMaxBitLen = round(12000.0 * dSamplesPerMicrosecond * dMaxCorrection);
    mMinPEHold = round(   26.0 * dSamplesPerMicrosecond * dMinCorrection);
	switch (mSettings->mMode)
	{
	case DCCAnalyzerEnums::MODE_CS:
		mMin1hbit  = round(  55.0  * dSamplesPerMicrosecond * dMinCorrection);
		mMax1hbit  = round(  61.0  * dSamplesPerMicrosecond * dMaxCorrection);
		mMin0hbit  = round(  95.0  * dSamplesPerMicrosecond * dMinCorrection);
		mMax0hbit  = round(9900.0  * dSamplesPerMicrosecond * dMaxCorrection);
		break;
	case DCCAnalyzerEnums::MODE_DECODER:
	case DCCAnalyzerEnums::MODE_SERVICE:
	default:
		mMin1hbit  = round(   52.0 * dSamplesPerMicrosecond * dMinCorrection);
		mMax1hbit  = round(   64.0 * dSamplesPerMicrosecond * dMaxCorrection);
		mMin0hbit  = round(   90.0 * dSamplesPerMicrosecond * dMinCorrection);
		mMax0hbit  = round(10000.0 * dSamplesPerMicrosecond * dMaxCorrection);
		break;
	}

	mDCC = GetAnalyzerChannelData(mSettings->mInputChannel);
}

void DCCAnalyzer::WorkerThread()
{
	Setup();
	U32 nHBitCnt = 0;
	U8  nHBitVal = 0;
	U32 nBits = 0;
	U8	nVal = 0;
	U8	nChecksum = 0;
	eFrameState ef = FSTATE_INIT;
	U64	nFrameStart = mDCC->GetSampleNumber();
	U64 nCurSample = nFrameStart;
    U64 nPreambleStart = 0;     // capture the actual start of the preamble
	U64 nBitStartSample = 0;
    U64 nCutoutStart = 0;
	U64 nTemp = nCurSample;
	for (;;) {
		nBitStartSample = nCurSample;
		switch (ef)
		{
        case FSTATE_INIT:
			nHBitVal = GetNextHBit(&nCurSample); // get next hbit
			switch (nHBitVal)
			{
            case 1:
                ++nHBitCnt;
                if (nHBitCnt == (mSettings->mPreambleBits * 2)) {
                    ef = FSTATE_PREAMBLE;
                }
				break;
            default: //anything not another 1 is just non-dcc
                nHBitCnt = 0;
                nFrameStart = nCurSample + 1;
                nPreambleStart = nFrameStart;
                break;
            }
            break;

//          case 0: // 0 HBit causes a reset in preamble bit count
//              nHBitCnt = 0;
//              nFrameStart = nCurSample + 1;
//              nPreambleStart = nFrameStart;
//  			break;
//  		default: // error frame
//  			nHBitCnt = 0;
//              nFrameStart = nCurSample + 1;
//  			PostFrame(nBitStartSample, nCurSample, FRAME_ERR, nHBitVal);
//  			mResults->AddMarker(nBitStartSample, AnalyzerResults::ErrorSquare, mSettings->mInputChannel);
//  			mResults->AddMarker(nCurSample, AnalyzerResults::ErrorX, mSettings->mInputChannel);
//  			ReportProgress(nCurSample);
		case FSTATE_PREAMBLE:
			nTemp = nCurSample;
			nHBitVal = LookaheadNextHBit(&nTemp); // get next hbit
			switch(nHBitVal) {
			case 0: // 0 HBit ends the preamble, send frame
				PostFrame(nPreambleStart, nCurSample, FRAME_PREAMBLE, 0, nHBitCnt / 2);
				ReportProgress(nCurSample);
				nFrameStart = nCurSample + 1;
				ef = FSTATE_PSBIT;
				break;
			case 1:
				nHBitVal = GetNextHBit(&nCurSample); // get next hbit
				++nHBitCnt;
				break;
			default:
				PostFrame(nBitStartSample, nCurSample, FRAME_ERR, nHBitVal);
				mResults->AddMarker(nBitStartSample, AnalyzerResults::ErrorSquare, mSettings->mInputChannel);
				mResults->AddMarker(nCurSample, AnalyzerResults::ErrorX, mSettings->mInputChannel);
				ReportProgress(nCurSample);
				nHBitCnt = 0;
				ef = FSTATE_INIT;
			}
			break;
		case FSTATE_PSBIT:
			nHBitVal = GetNextBit(&nCurSample); // get next hbit
            nHBitCnt = 0;   // resets the preamble bit count
			if (nHBitVal == 0) { // this is the start bit, now we are in sync
				PostFrame(nFrameStart, nCurSample, FRAME_PSBIT, 0);
				mResults->AddMarker(nFrameStart, AnalyzerResults::Start, mSettings->mInputChannel);
				ReportProgress(nCurSample);
				nBits = nVal = 0;
				nFrameStart = nCurSample + 1;
				ef = FSTATE_ADBYTE;
			} else {
				PostFrame(nBitStartSample, nCurSample, FRAME_ERR, FRAMING_ERROR_FLAG);
				mResults->AddMarker(nBitStartSample, AnalyzerResults::ErrorDot, mSettings->mInputChannel);
				ReportProgress(nCurSample);
				nHBitCnt = 0;
				ef = FSTATE_INIT;
			}
			break;
		case FSTATE_ADBYTE:
			switch (nHBitVal = GetNextBit(&nCurSample))
			{
			case 0:
			case 1:
				nVal <<= 1;
				nVal |= nHBitVal;
				nBits++;
				if (nBits == 8)
				{
					nChecksum ^= nVal;
                    ef = FSTATE_DSBIT;
                    PostFrame(nFrameStart, nCurSample, FRAME_ADBYTE, 0, nVal);
					ReportProgress(nCurSample);
					nFrameStart = nCurSample + 1;
				}
				break;
			default:
				PostFrame(nBitStartSample, nCurSample, FRAME_ERR, nHBitVal);
				mResults->AddMarker(nBitStartSample, AnalyzerResults::ErrorSquare, mSettings->mInputChannel);
				mResults->AddMarker(nCurSample, AnalyzerResults::ErrorX, mSettings->mInputChannel);
				ReportProgress(nCurSample);
				nHBitCnt = 0;
				ef = FSTATE_INIT;
			}
			break;
        case FSTATE_DSBIT:
            nHBitVal = GetNextBit(&nCurSample); // get next bit
            if (nHBitVal == 0) { // start bit
                PostFrame(nFrameStart, nCurSample, FRAME_DSBIT, 0);
                mResults->AddMarker(nFrameStart, AnalyzerResults::Start, mSettings->mInputChannel);
                ReportProgress(nCurSample);
                nBits = nVal = 0;
                nFrameStart = nCurSample + 1;
                ef = FSTATE_DBYTE;
            }
            else {
                PostFrame(nBitStartSample, nCurSample, FRAME_ERR, FRAMING_ERROR_FLAG);
                mResults->AddMarker(nBitStartSample, AnalyzerResults::ErrorDot, mSettings->mInputChannel);
                ReportProgress(nCurSample);
                nHBitCnt = 0;
                ef = FSTATE_INIT;
            }
            break;
        case FSTATE_DBYTE:
            switch (nHBitVal = GetNextBit(&nCurSample))
            {
            case 0:
            case 1:
                nVal <<= 1;
                nVal |= nHBitVal;
                nBits++;
                if (nBits == 8)
                {
                    nTemp = nCurSample;
                    nChecksum ^= nVal;
                    UINT nLABit = LookaheadNextHBit(&nTemp);
                    if (nLABit == 0) { //look for end of packet
                        PostFrame(nFrameStart, nCurSample, FRAME_DBYTE, 0, nVal);
                        ef = FSTATE_DSBIT;
                    } else if (nLABit == 1) {
                        nTemp = (nChecksum == 0) ? 0 : PACKET_ERROR_FLAG;
                        PostFrame(nFrameStart, nCurSample, FRAME_EDBYTE, nTemp, nVal);
                        if (nTemp != 0){    // mark the error but pass on the packet.
                            mResults->AddMarker(nFrameStart, AnalyzerResults::ErrorX, mSettings->mInputChannel);
                            mResults->AddMarker(nCurSample, AnalyzerResults::ErrorSquare, mSettings->mInputChannel);
                        }
                        ef = FSTATE_PEBIT;
                        nHBitCnt = 0;
                    } else {
                        PostFrame(nBitStartSample, nCurSample, FRAME_ERR, FRAMING_ERROR_FLAG);
                        mResults->AddMarker(nBitStartSample, AnalyzerResults::ErrorDot, mSettings->mInputChannel);
                        ReportProgress(nCurSample);
                        nHBitCnt = 0;
                        ef = FSTATE_INIT;
                    }
                    ReportProgress(nCurSample);
                    nFrameStart = nCurSample + 1;
                }
                break;
            default:
                PostFrame(nBitStartSample, nCurSample, FRAME_ERR, nHBitVal);
                mResults->AddMarker(nBitStartSample, AnalyzerResults::ErrorSquare, mSettings->mInputChannel);
                mResults->AddMarker(nCurSample, AnalyzerResults::ErrorX, mSettings->mInputChannel);
                ReportProgress(nCurSample);
                nHBitCnt = 0;
                ef = FSTATE_INIT;
            }
            break;
        case FSTATE_PEBIT:
            nHBitVal = GetNextBit(&nCurSample); // get next hbit
            nTemp = nCurSample;
            PostFrame(nFrameStart, nCurSample, FRAME_PEBIT, 0);
            mResults->AddMarker(nFrameStart, AnalyzerResults::Stop, mSettings->mInputChannel);
            ReportProgress(nCurSample);
            nFrameStart = nCurSample + 1;
            nPreambleStart = nFrameStart;
            nBits = nVal = 0;
            switch(LookaheadNextHBit(&nTemp)) { // see what to do at the end of packet
            case 1:
                nHBitCnt = 2;       // marks the case where the next bit might be preamble and we need to count it.
                break;
            case 0:
                nHBitCnt = 0;       // no preamble next so don't count these bits as preamble
                break;
            }
            if (mSettings->mMode == DCCAnalyzerEnums::MODE_CS) {
                if (not CheckPacketEndHold(&nTemp)) {
                    PostFrame(nBitStartSample, nCurSample, FRAME_END_ERR, nHBitVal);
                    mResults->AddMarker(nBitStartSample, AnalyzerResults::ErrorSquare, mSettings->mInputChannel);
                    mResults->AddMarker(nCurSample, AnalyzerResults::ErrorX, mSettings->mInputChannel);
                    ReportProgress(nCurSample);
                    nHBitCnt = 0;
                }
            }
            ef = FSTATE_INIT;
            break;
		default:
			nHBitCnt = 0;
			ef = FSTATE_INIT;
		}
		CheckIfThreadShouldExit();
	}
}

bool DCCAnalyzer::NeedsRerun()
{
	return false;
}

U32 DCCAnalyzer::GenerateSimulationData(U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor **simulation_channels)
{
    if (mSimulationInitilized == false) {
        mSimulationDataGenerator.Initialize(GetSimulationSampleRate(), mSettings.get());
        mSimulationInitilized = true;
    }

    return mSimulationDataGenerator.GenerateSimulationData(minimum_sample_index, device_sample_rate, simulation_channels);
}

U32 DCCAnalyzer::GetMinimumSampleRateHz()
{
    return 1000000;
}

const char *DCCAnalyzer::GetAnalyzerName() const
{
    return "DCC";
}

const char *GetAnalyzerName()
{
    return "DCC";
}

Analyzer *CreateAnalyzer()
{
    return new DCCAnalyzer();
}

void DestroyAnalyzer(Analyzer *analyzer)
{
    delete analyzer;
}

