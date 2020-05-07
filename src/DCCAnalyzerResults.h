#ifndef DCC_ANALYZER_RESULTS
#define DCC_ANALYZER_RESULTS

#include <AnalyzerResults.h>

#define FRAMING_ERROR_FLAG ( 1 << 0 )
#define CHECKSUM_ERROR_FLAG ( 1 << 1 )
#define MP_MODE_ADDRESS_FLAG ( 1 << 2 )
enum eFrameType { FRAME_PREAMBLE, FRAME_SBIT, FRAME_ADDR, FRAME_CMD, FRAME_DATA, FRAME_CHECKSUM };

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

protected:  //vars
    DCCAnalyzerSettings *mSettings;
    DCCAnalyzer *mAnalyzer;
};

#endif //SERIAL_ANALYZER_RESULTS
