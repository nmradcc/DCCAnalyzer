#ifndef DCC_SIMULATION_DATA_GENERATOR
#define DCC_SIMULATION_DATA_GENERATOR

#include <AnalyzerHelpers.h>

typedef unsigned int UINT;

class DCCAnalyzerSettings;

enum DCC_Cmd_t {CMD_Reset, CMD_End};
const U32 DCC_PREAMBLE_BITS = 18;
const double HB_1 = 58.0;
const double HB_0 = 116.0;

const U8 CmdList[][2] = { 
	{ 0, 0 }, // CMD_Reset
	{ 1, 0 }
};

class DCCSimulationDataGenerator
{
public:
    DCCSimulationDataGenerator();
    ~DCCSimulationDataGenerator();

    void Initialize(U32 simulation_sample_rate, DCCAnalyzerSettings *settings);
    U32 GenerateSimulationData(U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor **simulation_channels);

protected:
    DCCAnalyzerSettings *mSettings;
    U32 mSimulationSampleRateHz;
    BitState mBitLow;
    BitState mBitHigh;
    U64 mValue;

    U64 mMpModeAddressMask;
    U64 mMpModeDataMask;
    U64 mNumBitsMask;

protected: //DCC specific
	void GenerateByte(U8 nVal);
	void CreateDCCPreamble(U32 nBits);
	void CreateDCCPacket(U32 Address, int cmd, U8 DCC_Data[4]);
    ClockGenerator mClockGenerator;
    SimulationChannelDescriptor mDCCSimulationData;  //if we had more than one channel to simulate, they would need to be in an array
};

#endif //UNIO_SIMULATION_DATA_GENERATOR
