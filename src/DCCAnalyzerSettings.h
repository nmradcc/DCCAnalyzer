#ifndef DCC_ANALYZER_SETTINGS
#define DCC_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

namespace DCCAnalyzerEnums
{
	enum eAnalyzerMode { MODE_DECODER, MODE_CS, MODE_SERVICE };
	enum FrameType { TYPE_Preamble, TYPE_Addr, TYPE_CmdByte, TYPE_CmdData, TYPE_Checksum };
};

class DCCAnalyzerSettings : public AnalyzerSettings

{
public:
    DCCAnalyzerSettings();
    virtual ~DCCAnalyzerSettings();

    virtual bool SetSettingsFromInterfaces();
    void UpdateInterfacesFromSettings();
    virtual void LoadSettings(const char *settings);
    virtual const char *SaveSettings();

    Channel mInputChannel;
	U64		mPreambleBits;
	DCCAnalyzerEnums::eAnalyzerMode mMode;
	int		mCalPPM;

protected:
    std::unique_ptr< AnalyzerSettingInterfaceChannel >    mInputChannelInterface;
	std::unique_ptr< AnalyzerSettingInterfaceInteger >    mPreambleBitsInterface;
	std::unique_ptr< AnalyzerSettingInterfaceNumberList >    mModeInterface;
	std::unique_ptr< AnalyzerSettingInterfaceInteger >    mCalPPMInterface;

};

#endif //SERIAL_ANALYZER_SETTINGS
