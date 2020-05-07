#include "DCCAnalyzerSettings.h"

#include <AnalyzerHelpers.h>
#include <sstream>
#include <cstring>

#define CHANNEL_NAME "Track1"

DCCAnalyzerSettings::DCCAnalyzerSettings()
    :   mInputChannel(UNDEFINED_CHANNEL)
{
    mInputChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
    mInputChannelInterface->SetTitleAndTooltip(CHANNEL_NAME, "DCC Track 1");
    mInputChannelInterface->SetChannel(mInputChannel);

    AddInterface(mInputChannelInterface.get());

    AddExportOption(0, "Export as text/csv file");
    AddExportExtension(0, "Text file", "txt");
    AddExportExtension(0, "CSV file", "csv");

    ClearChannels();
    AddChannel(mInputChannel, CHANNEL_NAME, false);
}

DCCAnalyzerSettings::~DCCAnalyzerSettings()
{
}

bool DCCAnalyzerSettings::SetSettingsFromInterfaces()
{
    mInputChannel = mInputChannelInterface->GetChannel();

    ClearChannels();
    AddChannel(mInputChannel, CHANNEL_NAME, true);

    return true;
}

void DCCAnalyzerSettings::UpdateInterfacesFromSettings()
{
    mInputChannelInterface->SetChannel(mInputChannel);
}

void DCCAnalyzerSettings::LoadSettings(const char *settings)
{
    SimpleArchive text_archive;
    text_archive.SetString(settings);

    const char *name_string;    //the first thing in the archive is the name of the protocol analyzer that the data belongs to.
    text_archive >> &name_string;
    if (strcmp(name_string, "Analyzer") != 0) {
        AnalyzerHelpers::Assert("DCCAnalyzer: Provided with a settings string that doesn't belong to us;");
    }

    text_archive >> mInputChannel;

    ClearChannels();
    AddChannel(mInputChannel, CHANNEL_NAME, true);

    UpdateInterfacesFromSettings();
}

const char *DCCAnalyzerSettings::SaveSettings()
{
    SimpleArchive text_archive;

    text_archive << "DCCAnalyzer";
    text_archive << mInputChannel;

    return SetReturnString(text_archive.GetString());
}
