#include "DCCSimulationDataGenerator.h"
#include "DCCAnalyzerSettings.h"

DCCSimulationDataGenerator::DCCSimulationDataGenerator()
{
}

DCCSimulationDataGenerator::~DCCSimulationDataGenerator()
{
}

void DCCSimulationDataGenerator::Initialize(U32 simulation_sample_rate, DCCAnalyzerSettings *settings)
{
    mSimulationSampleRateHz = simulation_sample_rate;
    mSettings = settings;

	mClockGenerator.Init(10000.0, simulation_sample_rate);
    mDCCSimulationData.SetChannel(mSettings->mInputChannel);
    mDCCSimulationData.SetSampleRate(simulation_sample_rate);

    mBitLow = BIT_LOW;
    mBitHigh = BIT_HIGH;
	mHBit1Samples = HB_1 * simulation_sample_rate / 1000000.0;
	mHBit0Samples = HB_0 * simulation_sample_rate / 1000000.0;

    mDCCSimulationData.SetInitialBitState(mBitHigh);
	mDCCSimulationData.Advance(10 * mHBit1Samples);  //insert 10 bit-periods of idle

    mValue = 0;

    mMpModeAddressMask = 0;
    mMpModeDataMask = 0;
    mNumBitsMask = 0;

    U32 num_bits = 8;
    for (U32 i = 0; i < num_bits; i++) {
        mNumBitsMask <<= 1;
        mNumBitsMask |= 0x1;
    }

}

U32 DCCSimulationDataGenerator::GenerateSimulationData(U64 largest_sample_requested, U32 sample_rate, 
	    SimulationChannelDescriptor **simulation_channels)
{
	U8 DCCData[] = { 0x11, 0x22, 0x33, 0x44 };
    U64 adjusted_largest_sample_requested = 
		AnalyzerHelpers::AdjustSimulationTargetSample(largest_sample_requested, sample_rate, mSimulationSampleRateHz);
	int CmdType;
	while (mDCCSimulationData.GetCurrentSampleNumber() < adjusted_largest_sample_requested) {
		for (CmdType = CMD_Reset; CmdType < CMD_End; CmdType++) {
			CreateDCCPreamble(DCC_PREAMBLE_BITS);
			CreateDCCPacket(3, CmdType, DCCData);
		} 
		for (int i = 0; i < 2; i++)
			CreateDCCPreamble(DCC_PREAMBLE_BITS);
	}
		
	*simulation_channels = &mDCCSimulationData;
	return 1;  // we are retuning the size of the SimulationChannelDescriptor array.  In our case, the "array" is length 1.
}

//------------------------------------------------------------------------
// Generate a preamble and send to buffer
// nBits - number of '1' bits in preamble
void DCCSimulationDataGenerator::CreateDCCPreamble(U32 nBits)
{
    //assume we start high
	for (UINT i = 0; i < (nBits * 2); i++)
	{
		mDCCSimulationData.Transition();  //flip bit
		mDCCSimulationData.Advance(mHBit1Samples);    //add HalfBit bit time
	}

}

void DCCSimulationDataGenerator::GenerateByte(U8 nVal)
{
	mDCCSimulationData.Transition();  //start bit, flip bit
	mDCCSimulationData.Advance(mHBit0Samples);    //add HalfBit bit time
	mDCCSimulationData.Transition();  //flip bit
	mDCCSimulationData.Advance(mHBit0Samples);    //add HalfBit bit time
	for (int i = 0; i < 8; i++)
	{
		if ((nVal & 0x80) != 0)
		{
			mDCCSimulationData.Transition();  //flip bit
			mDCCSimulationData.Advance(mHBit1Samples);    //add HalfBit bit time
			mDCCSimulationData.Transition();  //flip bit
			mDCCSimulationData.Advance(mHBit1Samples);    //add HalfBit bit time
		} else	{
			mDCCSimulationData.Transition();  //flip bit
			mDCCSimulationData.Advance(mHBit0Samples);    //add HalfBit bit time
			mDCCSimulationData.Transition();  //flip bit
			mDCCSimulationData.Advance(mHBit0Samples);    //add HalfBit bit time
		}
		nVal <<= 1;
	}
}

void DCCSimulationDataGenerator::CreateDCCPacket(U32 Address, int cmd, U8 *DCC_Data)
{
// Generate Address Byte
	U8 nChecksum = Address & 0xFF;
	GenerateByte(nChecksum);
// Generate Command Byte
	nChecksum ^= CmdList[cmd][0];
	GenerateByte(CmdList[cmd][0]);
// Generate Data bytes
	for (int i = 0; i < CmdList[cmd][1]; i++)
	{
		nChecksum ^= DCC_Data[i];
		GenerateByte(DCC_Data[i]);
	}
// Generate Checksum
	GenerateByte(nChecksum);
}


