#include "DCCAnalyzer.h"
#include "DCCAnalyzerSettings.h"
#include <AnalyzerChannelData.h>

DCCAnalyzer::DCCAnalyzer()
    : Analyzer(),
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

UINT	DCCAnalyzer::LookaheadNextHBit(U64 *nSample)
{
	UINT nHBitLen = mDCC->GetSampleOfNextEdge() - *nSample;
	*nSample = mDCC->GetSampleOfNextEdge();
	if (nHBitLen >= mMin1hbit && nHBitLen <= mMax1hbit)
		return 1;
	else if (nHBitLen >= mMin0hbit && nHBitLen <= mMax0hbit)
		return 0;
	else
		return BIT_ERROR_FLAG; // framing error
}

UINT	DCCAnalyzer::GetNextHBit(U64 *nSample)
{
	UINT nSampNumber = *nSample;
	mDCC->AdvanceToNextEdge();
	*nSample = mDCC->GetSampleNumber();
	UINT	nHBitLen = mDCC->GetSampleNumber() - nSampNumber;
	if (nHBitLen >= mMin1hbit && nHBitLen <= mMax1hbit)
		return 1;
	else if (nHBitLen >= mMin0hbit && nHBitLen <= mMax0hbit)
		return 0;
	else
		return BIT_ERROR_FLAG; // framing error
}

//--------------------------------------------------------------
// Get the next bit (0 or 1).
// Alignment error: return 3;
// Framing and other error return 2;
UINT	DCCAnalyzer::GetNextBit(U64 *nSample)
{
	UINT nHBit1 = GetNextHBit(nSample);
	UINT nHBit2 = GetNextHBit(nSample);
	if (nHBit1 > 1 || nHBit2 > 1)
		return BIT_ERROR_FLAG;
	else if (nHBit1 != nHBit2)
		return FRAMING_ERROR_FLAG;
	else return nHBit1;
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
	mResults->CommitResults();
}



void DCCAnalyzer::WorkerThread()
{
	mSampleRateHz = GetSampleRate();
	mMin1hbit = 55 * (mSampleRateHz / 1000000);
	mMax1hbit = 61 * (mSampleRateHz / 1000000);
	mMin0hbit = 95 * (mSampleRateHz / 1000000);
	mMax0hbit = 9900 * (mSampleRateHz / 1000000);
	mDCC = GetAnalyzerChannelData(mSettings->mInputChannel);
	U32 nHBitCnt = 0;
	U8  nHBitVal = 0;
	U32 nBits = 0;
	U8	nVal = 0;
	U8	nChecksum = 0;
	eFrameState ef = FSTATE_INIT;
	U64	nFrameStart = mDCC->GetSampleNumber();
	U64 nCurSample = nFrameStart;
	U64 nBitStartSample = 0;
	U64 nTemp = nCurSample;
	for (;;) {
		nBitStartSample = nCurSample;
		switch (ef)
		{
		case FSTATE_INIT:
			nHBitVal = GetNextHBit(&nCurSample); // get next hbit
			switch (nHBitVal)
			{
			case 0: // 0 HBit causes a reset in preamble bit count
				nHBitCnt = 0;
				nFrameStart = nCurSample + 1;
				break;
			case 1:
				++nHBitCnt;
				if (nHBitCnt == MIN_PEAMBLE_LEN)
					ef = FSTATE_PREAMBLE;
				break;
			default: // error frame
				nHBitCnt = 0;
				nFrameStart = nCurSample + 1;
				PostFrame(nBitStartSample, nCurSample, FRAME_ERR, nHBitVal);
				mResults->AddMarker(nBitStartSample, AnalyzerResults::ErrorSquare, mSettings->mInputChannel);
				mResults->AddMarker(nCurSample, AnalyzerResults::ErrorX, mSettings->mInputChannel);
				ReportProgress(nCurSample);
			}
			break;
		case FSTATE_PREAMBLE:
			nTemp = nCurSample;
			nHBitVal = LookaheadNextHBit(&nTemp); // get next hbit
			switch(nHBitVal) {
			case 0: // 0 HBit ends the preamble, send frame
				PostFrame(nFrameStart, nCurSample, FRAME_PREAMBLE, 0, nHBitCnt / 2);
				mResults->AddMarker(nFrameStart, AnalyzerResults::Dot, mSettings->mInputChannel);
				ReportProgress(nCurSample);
				nFrameStart = nCurSample + 1;
				ef = FSTATE_SBADDR;
				break;
			case 1:
				nHBitVal = GetNextHBit(&nCurSample); // get next hbit
				++nHBitCnt;
				break;
			deafult:
				PostFrame(nBitStartSample, nCurSample, FRAME_ERR, nHBitVal);
				mResults->AddMarker(nBitStartSample, AnalyzerResults::ErrorSquare, mSettings->mInputChannel);
				mResults->AddMarker(nCurSample, AnalyzerResults::ErrorX, mSettings->mInputChannel);
				ReportProgress(nCurSample);
				nHBitCnt = 0;
				ef = FSTATE_INIT;
			}
			break;
		case FSTATE_SBADDR:
			nHBitVal = GetNextBit(&nCurSample); // get next hbit
			if (nHBitVal == 0) { // this is the start bit, now we are in sync
				PostFrame(nFrameStart, nCurSample, FRAME_SBIT, 0);
				mResults->AddMarker(nFrameStart, AnalyzerResults::Dot, mSettings->mInputChannel);
				ReportProgress(nCurSample);
				nBits = nVal = 0;
				nFrameStart = nCurSample + 1;
				ef = FSTATE_ADDR;
			} else {
				PostFrame(nBitStartSample, nCurSample, FRAME_ERR, FRAMING_ERROR_FLAG);
				mResults->AddMarker(nBitStartSample, AnalyzerResults::ErrorDot, mSettings->mInputChannel);
				ReportProgress(nCurSample);
				nHBitCnt = 0;
				ef = FSTATE_INIT;
			}
			break;
		case FSTATE_ADDR:
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
					PostFrame(nFrameStart, nCurSample, FRAME_ADDR, 0, nVal);
					mResults->AddMarker(nFrameStart, AnalyzerResults::Dot, mSettings->mInputChannel);
					ReportProgress(nCurSample);
					if (nVal <= 0x7F || (nVal == 0xFF))
						ef = FSTATE_SBCMD;
					else if ((nVal >= 0x80) && (nVal < 0xBF))
						ef = FSTATE_SBACC;
					else if ((nVal >= 0xC0) && (nVal < 0xE8))
						ef = FSTATE_SBEADR;
					else 
						ef = FSTATE_INIT; // undefined
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
		case FSTATE_SBEADR:
			nHBitVal = GetNextBit(&nCurSample); // get next bit
			if (nHBitVal == 0) { // start bit
				PostFrame(nFrameStart, nCurSample, FRAME_SBIT, 0);
				mResults->AddMarker(nFrameStart, AnalyzerResults::Dot, mSettings->mInputChannel);
				ReportProgress(nCurSample);
				nBits = nVal = 0;
				nFrameStart = nCurSample + 1;
				ef = FSTATE_EADR;
			}
			else {
				PostFrame(nBitStartSample, nCurSample, FRAME_ERR, FRAMING_ERROR_FLAG);
				mResults->AddMarker(nBitStartSample, AnalyzerResults::ErrorDot, mSettings->mInputChannel);
				ReportProgress(nCurSample);
				nHBitCnt = 0;
				ef = FSTATE_INIT;
			}
			break;

		case FSTATE_EADR:
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
					PostFrame(nFrameStart, nCurSample, FRAME_EADDR, 0, nVal);
					mResults->AddMarker(nFrameStart, AnalyzerResults::Dot, mSettings->mInputChannel);
					ReportProgress(nCurSample);
					nFrameStart = nCurSample + 1;
					ef = FSTATE_SBCMD;
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
		case FSTATE_SBCMD:
			nHBitVal = GetNextBit(&nCurSample); // get next bit
			if (nHBitVal == 0) { // start bit
				PostFrame(nFrameStart, nCurSample, FRAME_SBIT, 0);
				mResults->AddMarker(nFrameStart, AnalyzerResults::Dot, mSettings->mInputChannel);
				ReportProgress(nCurSample);
				nBits = nVal = 0;
				nFrameStart = nCurSample + 1;
				ef = FSTATE_CMD;
			}
			else {
				PostFrame(nBitStartSample, nCurSample, FRAME_ERR, FRAMING_ERROR_FLAG);
				mResults->AddMarker(nBitStartSample, AnalyzerResults::ErrorDot, mSettings->mInputChannel);
				ReportProgress(nCurSample);
				nHBitCnt = 0;
				ef = FSTATE_INIT;
			}
			break;
		case FSTATE_CMD:
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
					PostFrame(nFrameStart, nCurSample, FRAME_CMD, 0, nVal);
					mResults->AddMarker(nFrameStart, AnalyzerResults::Dot, mSettings->mInputChannel);
					ReportProgress(nCurSample);
					nFrameStart = nCurSample + 1;
					ef = FSTATE_SBDAT;
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
		case FSTATE_SBDAT:
			nHBitVal = GetNextBit(&nCurSample); // get next bit
			if (nHBitVal == 0) { // start bit
				PostFrame(nFrameStart, nCurSample, FRAME_SBIT, 0);
				mResults->AddMarker(nFrameStart, AnalyzerResults::Dot, mSettings->mInputChannel);
				ReportProgress(nCurSample);
				nBits = nVal = 0;
				nFrameStart = nCurSample + 1;
				ef = FSTATE_DATA;
			}
			else {
				PostFrame(nBitStartSample, nCurSample, FRAME_ERR, FRAMING_ERROR_FLAG);
				mResults->AddMarker(nBitStartSample, AnalyzerResults::ErrorDot, mSettings->mInputChannel);
				ReportProgress(nCurSample);
				nHBitCnt = 0;
				ef = FSTATE_INIT;
			}
			break;
		case FSTATE_DATA:
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
					if (LookaheadNextHBit(&nTemp) == 0) { //look for end of packet
						PostFrame(nFrameStart, nCurSample, FRAME_DATA, 0, nVal);
						mResults->AddMarker(nFrameStart, AnalyzerResults::Dot, mSettings->mInputChannel);
						ef = FSTATE_SBDAT;
					}
					else {
						nTemp = (nChecksum == 0) ? 0 : CHECKSUM_ERROR_FLAG;
						PostFrame(nFrameStart, nCurSample, FRAME_CHECKSUM, nTemp, nVal);
						if (nTemp != 0){
							mResults->AddMarker(nFrameStart, AnalyzerResults::ErrorX, mSettings->mInputChannel);
							mResults->AddMarker(nCurSample, AnalyzerResults::ErrorSquare, mSettings->mInputChannel);
						}
						else {
							mResults->AddMarker(nFrameStart, AnalyzerResults::Dot, mSettings->mInputChannel);
						}
						nHBitCnt = 0;
						ef = FSTATE_INIT;
					}
					ReportProgress(nCurSample);
					nFrameStart = nCurSample + 1;
				}
				break;
			case FSTATE_SBACC:
				nHBitVal = GetNextBit(&nCurSample); // get next bit
				if (nHBitVal == 0) { // start bit
					PostFrame(nFrameStart, nCurSample, FRAME_SBIT, 0);
					mResults->AddMarker(nFrameStart, AnalyzerResults::Dot, mSettings->mInputChannel);
					ReportProgress(nCurSample);
					nBits = nVal = 0;
					nFrameStart = nCurSample + 1;
					ef = FSTATE_ACC;
				}
				else {
					PostFrame(nBitStartSample, nCurSample, FRAME_ERR, FRAMING_ERROR_FLAG);
					mResults->AddMarker(nBitStartSample, AnalyzerResults::ErrorDot, mSettings->mInputChannel);
					ReportProgress(nCurSample);
					nHBitCnt = 0;
					ef = FSTATE_INIT;
				}
				break;
			case FSTATE_ACC:
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
						PostFrame(nFrameStart, nCurSample, FRAME_ACC, 0, nVal);
						mResults->AddMarker(nFrameStart, AnalyzerResults::Dot, mSettings->mInputChannel);
						ReportProgress(nCurSample);
						nFrameStart = nCurSample + 1;
						ef = FSTATE_SBDAT;
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
			default:
				PostFrame(nBitStartSample, nCurSample, FRAME_ERR, nHBitVal);
				mResults->AddMarker(nBitStartSample, AnalyzerResults::ErrorSquare, mSettings->mInputChannel);
				mResults->AddMarker(nCurSample, AnalyzerResults::ErrorX, mSettings->mInputChannel);
				ReportProgress(nCurSample);
				nHBitCnt = 0;
				ef = FSTATE_INIT;
			}
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

