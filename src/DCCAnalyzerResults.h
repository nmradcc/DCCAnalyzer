#ifndef DCC_ANALYZER_RESULTS
#define DCC_ANALYZER_RESULTS

#include <AnalyzerResults.h>

#define BIT_ERROR_FLAG ( 1 << 1 )
#define CHECKSUM_ERROR_FLAG ( 1 << 2 )
#define FRAMING_ERROR_FLAG (1 << 3)
#define RAILCOM_CUTOUT_FLAG ( 1 << 4 )
#define RAILCOM_CUTOUT_TCS_FLAG ( 1 << 5 )
#define RAILCOM_CUTOUT_TCE_FLAG ( 1 << 6 )
#define RAILCOM_CUTOUT_END_FLAG ( 1 << 7 )
enum eFrameType { 
    FRAME_ERR, 
    FRAME_PREAMBLE, 
    FRAME_CUTOUT,
    FRAME_SBIT,
    FRAME_ADDR,
    FRAME_EADDR,
    FRAME_CMD,
    FRAME_ACC,
    FRAME_SVC,
    FRAME_DATA,
    FRAME_CHECKSUM,
    FRAME_PEBIT
};

class DCCAnalyzer;
class DCCAnalyzerSettings;

class DCCAnalyzerResults : public AnalyzerResults
{
public:
    DCCAnalyzerResults(DCCAnalyzer *analyzer, DCCAnalyzerSettings *settings);
    virtual ~DCCAnalyzerResults();

    virtual void GenerateBubbleText(U64 frame_index, Channel &channel, DisplayBase display_base);
    virtual void GenerateExportFile(const char *file, DisplayBase display_base, U32 export_type_user_id);

    virtual void GenerateFrameTabularText(U64 frame_index, DisplayBase display_base);
    virtual void GeneratePacketTabularText(U64 packet_id, DisplayBase display_base);
    virtual void GenerateTransactionTabularText(U64 transaction_id, DisplayBase display_base);

protected: //functions
	virtual char * ParseCommand(U8 cCmd);
	virtual char * ParseAccessory(U8 cCmd);
	virtual char * ParseServiceMode(U8 cCmd, bool bLongPacket);

protected:  //vars
    DCCAnalyzerSettings *mSettings;
    DCCAnalyzer *mAnalyzer;
private:
	char sParseBuf[128];
};

#endif //SERIAL_ANALYZER_RESULTS
