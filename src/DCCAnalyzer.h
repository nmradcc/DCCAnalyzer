#ifndef DCC_ANALYZER_H
#define DCC_ANALYZER_H

#include <Analyzer.h>
#include "DCCAnalyzerResults.h"
#include "DCCSimulationDataGenerator.h"

class DCCAnalyzerSettings;

enum eFrameState { FSTATE_INIT, FSTATE_PREAMBLE, FSTATE_SBADDR, FSTATE_ADDR, FSTATE_SBEADR, FSTATE_EADR, FSTATE_SBCMD, FSTATE_CMD, FSTATE_SBDAT,
	FSTATE_DATA, FSTATE_SBACC, FSTATE_ACC, FSTATE_SBCHK, FSTATE_CHK };

class ANALYZER_EXPORT DCCAnalyzer : public Analyzer
{
public:
    DCCAnalyzer();
    virtual ~DCCAnalyzer();
    virtual void SetupResults();
    virtual void WorkerThread();

    virtual U32 GenerateSimulationData(U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor **simulation_channels);
    virtual U32 GetMinimumSampleRateHz();

    virtual const char *GetAnalyzerName() const;
    virtual bool NeedsRerun();


#pragma warning( push )
#pragma warning( disable : 4251 ) //warning C4251: 'SerialAnalyzer::<...>' : class <...> needs to have dll-interface to be used by clients of class

protected: //functions
	UINT LookaheadNextHBit(U64 *nSample);
	UINT GetNextHBit(U64 *nSample);
	UINT GetNextBit(U64 *nSample);
	void DCCAnalyzer::PostFrame(U64 nStartSample, U64 nEndSample, eFrameType ft, U8 Flags, U64 Data1, U64 Data2);

protected: //vars
    std::auto_ptr< DCCAnalyzerSettings > mSettings;
    std::auto_ptr< DCCAnalyzerResults > mResults;
    AnalyzerChannelData *mDCC;

    DCCSimulationDataGenerator mSimulationDataGenerator;
    bool mSimulationInitilized;

    //Serial analysis vars:
    U32 mSampleRateHz;
    std::vector<U32> mSampleOffsets;
    U32 mParityBitOffset;
    U32 mStartOfStopBitOffset;
    U32 mEndOfStopBitOffset;
    BitState mBitLow;
    BitState mBitHigh;
	UINT mMin1hbit;
	UINT mMax1hbit;
	UINT mMin0hbit;
	UINT mMax0hbit;
#pragma warning( pop )
};

extern "C" ANALYZER_EXPORT const char *__cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer *__cdecl CreateAnalyzer();
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer(Analyzer *analyzer);

#endif //SERIAL_ANALYZER_H
