#ifndef DCC_ANALYZER_RESULTS
#define DCC_ANALYZER_RESULTS

#include <AnalyzerResults.h>

#define BIT_ERROR_FLAG ( 1 << 1 )
#define PACKET_ERROR_FLAG ( 1 << 2 )
#define FRAMING_ERROR_FLAG (1 << 3)

enum eFrameType { 
    FRAME_PREAMBLE, 
    FRAME_PSBIT,
    FRAME_ADBYTE,
    FRAME_DSBIT,
    FRAME_DBYTE,
    FRAME_EDBYTE,
    FRAME_PEBIT,
    FRAME_ERR,
    FRAME_END_ERR
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

protected:  //vars
    DCCAnalyzerSettings *mSettings;
    DCCAnalyzer *mAnalyzer;
private:
	char sParseBuf[128];
};

#endif //SERIAL_ANALYZER_RESULTS
