#include "DCCAnalyzerSettings.h"

#include <AnalyzerHelpers.h>
#include <sstream>
#include <cstring>

#define CHANNEL_NAME "Track1"

DCCAnalyzerSettings::DCCAnalyzerSettings()
	: mInputChannel(UNDEFINED_CHANNEL),
	mPreambleBits(14),
	mMode(DCCAnalyzerEnums::MODE_DECODER),
	mCalPPM(0)
{
    mInputChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
    mInputChannelInterface->SetTitleAndTooltip(CHANNEL_NAME, "DCC Track 1");
    mInputChannelInterface->SetChannel(mInputChannel);
	AddInterface(mInputChannelInterface.get());

	mPreambleBitsInterface.reset(new AnalyzerSettingInterfaceInteger());
	mPreambleBitsInterface->SetTitleAndTooltip("Preamble Size", "Minimum preamble bit length");
	mPreambleBitsInterface->SetMin(8);
	mPreambleBitsInterface->SetMax(22);
	mPreambleBitsInterface->SetInteger(mPreambleBits);
	AddInterface(mPreambleBitsInterface.get());

	mModeInterface.reset(new AnalyzerSettingInterfaceNumberList());
	mModeInterface->SetTitleAndTooltip("Mode", "Analysis Mode: Decoder, Command Station, Service Mode");
	mModeInterface->ClearNumbers();
	mModeInterface->AddNumber(DCCAnalyzerEnums::MODE_DECODER, "Decoder", "Set decoder bit and packet timing");
	mModeInterface->AddNumber(DCCAnalyzerEnums::MODE_CS, "Command Station", "Set command station bit and packet timing");
	mModeInterface->AddNumber(DCCAnalyzerEnums::MODE_SERVICE, "Service Mode", "Set Service Mode packets and timing");
	mModeInterface->SetNumber(mMode);
	AddInterface(mModeInterface.get());
	
	mCalPPMInterface.reset(new AnalyzerSettingInterfaceInteger());
	mCalPPMInterface->SetTitleAndTooltip("Calibration Factor [+/-PPM]", "bit timing limits error correction");
	mCalPPMInterface->SetMin(-1000000);
	mCalPPMInterface->SetMax(1000000);
	mCalPPMInterface->SetInteger(mCalPPM);
	AddInterface(mCalPPMInterface.get());

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
	mPreambleBits = mPreambleBitsInterface->GetInteger();
	mMode = (DCCAnalyzerEnums::eAnalyzerMode)(int)mModeInterface->GetNumber();
	mCalPPM = mCalPPMInterface->GetInteger();
    ClearChannels();
    AddChannel(mInputChannel, CHANNEL_NAME, true);

	return true;
}

void DCCAnalyzerSettings::UpdateInterfacesFromSettings()
{
    mInputChannelInterface->SetChannel(mInputChannel);
	mPreambleBitsInterface->SetInteger(mPreambleBits);
	mModeInterface->SetNumber(mMode);
	mCalPPMInterface->SetInteger(mCalPPM);
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
	text_archive >> mPreambleBits;
	text_archive >> *(int *)&mMode;
	text_archive >> mCalPPM;

    ClearChannels();
    AddChannel(mInputChannel, CHANNEL_NAME, true);

    UpdateInterfacesFromSettings();
}

const char *DCCAnalyzerSettings::SaveSettings()
{
    SimpleArchive text_archive;

    text_archive << "DCCAnalyzer";
    text_archive << mInputChannel;
	text_archive << mPreambleBits;
	text_archive << (int)mMode;
	text_archive << mCalPPM;


    return SetReturnString(text_archive.GetString());
}
 