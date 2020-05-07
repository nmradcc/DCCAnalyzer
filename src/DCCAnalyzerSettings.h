#ifndef DCC_ANALYZER_SETTINGS
#define DCC_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

namespace DCCAnalyzerEnums
{
    enum Mode { Normal, Service };
	enum FrameType {TYPE_Preamble, TYPE_Addr, TYPE_CmdByte, TYPE_CmdData, TYPE_Checksum};
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

protected:
    std::auto_ptr< AnalyzerSettingInterfaceChannel >    mInputChannelInterface;
};

#endif //SERIAL_ANALYZER_SETTINGS
