# DCCAnalyzer

Visual Studio Build Instructions

Open Visual Studio and click File->Open->Project/Solution

Select the DCCAnalyzer.vcxproj file

If it asks you if you want to update it to the latest version, click ok

In the top banner, click on the pull down for "Debug" or "Release", select "Release"

Next to that, selct either "x86" if you have a 32-bit machien or "x64" if you have a 64-bit machine

From the Build menu at the top, select "Build Solution"

It should say something like "========== Build: 1 succeeded..." at the bottom when it's done.

To install the analyzer, (these instructions are from the SDK manual)

you need to copy the DCCAnalyzer.dll from either the Release/x86 or Release/x64 directory to C:\Program Files\KingsVIS\Analyzer directory.

The analyze should show up in the list of Analyzers when you restart KingstVIS


