# DCCAnalyzer

This repository is for maintaining the software associated with decoding DCC packets using a logic analyzer.

The nmradcc/DCCAnalyzer/src directory contains the source code for the parsing functions necessary to support the following two analyzers.

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
