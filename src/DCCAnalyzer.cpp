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

void DCCAnalyzer::ComputeSampleOffsets()
{
    /*ClockGenerator clock_generator;
    clock_generator.Init(mSettings->mBitRate, mSampleRateHz);

    mSampleOffsets.clear();

    U32 num_bits = mSettings->mBitsPerTransfer;

    if (mSettings->mDCCMode != SerialAnalyzerEnums::Normal) {
        num_bits++;
    }

    mSampleOffsets.push_back(clock_generator.AdvanceByHalfPeriod(1.5));  //point to the center of the 1st bit (past the start bit)
    num_bits--;  //we just added the first bit.

    for (U32 i = 0; i < num_bits; i++) {
        mSampleOffsets.push_back(clock_generator.AdvanceByHalfPeriod());
    }

    if (mSettings->mParity != AnalyzerEnums::None) {
        mParityBitOffset = clock_generator.AdvanceByHalfPeriod();
    }

    //to check for framing errors, we also want to check
    //1/2 bit after the beginning of the stop bit
    mStartOfStopBitOffset = clock_generator.AdvanceByHalfPeriod(1.0);   //i.e. moving from the center of the last data bit (where we left off) to 1/2 period into the stop bit

    //and 1/2 bit before end of the stop bit period
    mEndOfStopBitOffset = clock_generator.AdvanceByHalfPeriod(mSettings->mStopBits - 1.0);  //if stopbits == 1.0, this will be 0
*/
}

void DCCAnalyzer::SetupResults()
{
    //Unlike the worker thread, this function is called from the GUI thread
    //we need to reset the Results object here because it is exposed for direct access by the GUI, and it can't be deleted from the WorkerThread

    mResults.reset(new DCCAnalyzerResults(this, mSettings.get()));
    SetAnalyzerResults(mResults.get());
    mResults->AddChannelBubblesWillAppearOn(mSettings->mInputChannel);
}

void DCCAnalyzer::WorkerThread()
{
}

bool Analyzer::NeedsRerun()
{
	return false;
}

U32 SerialAnalyzer::GenerateSimulationData(U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor **simulation_channels)
{
    if (mSimulationInitilized == false) {
        mSimulationDataGenerator.Initialize(GetSimulationSampleRate(), mSettings.get());
        mSimulationInitilized = true;
    }

    return mSimulationDataGenerator.GenerateSimulationData(minimum_sample_index, device_sample_rate, simulation_channels);
}

U32 SerialAnalyzer::GetMinimumSampleRateHz()
{
    return mSettings->mBitRate * 4;
}

const char *SerialAnalyzer::GetAnalyzerName() const
{
    return "UART/232/485";
}

const char *GetAnalyzerName()
{
    return "UART/232/485";
}

Analyzer *CreateAnalyzer()
{
    return new SerialAnalyzer();
}

void DestroyAnalyzer(Analyzer *analyzer)
{
    delete analyzer;
}

