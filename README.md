# DCCAnalyzer v0.0.8

This repository is for maintaining the software associated with decoding DCC packets using a Saleae logic analyzer.

The nmradcc/DCCAnalyzer/src directory contains the source code for the parsing functions necessary.

This analyzer requires the [Saleae Logic 2 software](https://ideas.saleae.com/f/changelog/). It's not compatible with the Logic 1.x software, but it could be with only small modifications.

## Installation

Once you have either build the analyzer library, or downloaded the compiled libraries, follow these instructions to install the DCC Analyzer in the Logic 2 software:
https://support.saleae.com/faq/technical-faq/setting-up-developer-directory

_Note: MacOS Users have an extra step, which is explained in the article above_

## Getting Started

### MacOS

Dependencies:

- [XCode with command line tools](https://apps.apple.com/us/app/xcode/id497799835)
- [CMake 3.11+](https://cmake.org/download/)

Installing command line tools after XCode is installed:

```bash
xcode-select --install
```

Then open XCode, open Preferences from the main menu, go to locations, and select the only option under 'Command line tools'.

Installing CMake on MacOS:

1. Download the binary distribution for MacOS, `cmake-*-Darwin-x86_64.dmg`, from https://cmake.org/download/
2. Install the usual way by dragging into applications.
3. Open a terminal and run the following:

```bash
/Applications/CMake.app/Contents/bin/cmake-gui --install
```

_Note: Errors may occur if older versions of CMake are installed._

**Building the analyzer:**

```bash
mkdir build
cd build
cmake ..
cmake --build .
cd Analyzers
install_name_tool -change @executable_path/libAnalyzer.dylib @rpath/libAnalyzer.dylib libdcc_analyzer.so
```

### Ubuntu 16.04

Dependencies:

- [CMake 3.11+](https://cmake.org/download/)
- gcc 4.8+

Misc dependencies:

```bash
sudo apt-get install build-essential
```

**Building the analyzer:**

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Windows

Dependencies:

- [Visual Studio 2015 Update 3](https://visualstudio.microsoft.com/)
- [CMake 3.11+](https://cmake.org/download/)

**Visual Studio 2015**

_Note - newer versions of Visual Studio should be fine._

Setup options:

- Programming Languages > Visual C++ > select all sub-components.

Note - if CMake has any problems with the MSVC compiler, it's likely a component is missing.

**CMake**

Download and install the latest CMake release here.
https://cmake.org/download/

**Building the analyzer:**

```bat
mkdir build
cd build
cmake ..
```

Then, open the newly created solution file located here: `build\dcc_analyzer.sln`

# Important Note

Do NOT connect the Saleae analyzer probes directly to either the DCC Command Station, DCC Power Station outputs, or the track. You MUST use a suitable method per the Saleae input specifications, for adequately conditioning the voltage levels. For the development of this code, the NMRA testing team used a passive-input digital isolator with a CMOS output to convert the differential DCC signal to a single-ended CMOS digital input suitable for the Saleae logic input. This device is the NVE Corporation IL610 (https://www.nve.com/Downloads/il600.pdf). IMPORTANT: the input coil of the IL610 requires a 1K series current-limiting resistor. You will need to power the CMOS output side of the IL610 with 5V. A USB breakout adapter and a small breadboard can be used to build this circuit fairly easily.
