
<center>
<img src="img/nmra.png"><br><br>
</center>

# NMRA DCCAnalyzer Plugin v0.0.9

A low level DCC Decoder plugin for Saleae Logic analyzers.  If you own a Saleae Logic 8, Logic Pro 8, or Logic pro 16 analyzer you may install this plugin to display decoded DCC frames above your captured signals.

## Background

If you are a model railroader or a product developer you may find this plugin helpful in discovering what DCC commands are present on your railroad.  The plugin may be used in the following modes:

| Mode | Use Case |
|:----:| :----- |
| Decoder | Timing and protocol is interpreted per NMRA specification for DCC Decoders (default) |
| Cmd Station | The same overall but the perspective of of the command station |
| Service Mode | Service mode commands are interpreted from the decoder perspective |

![alt text](img/TimeLine.png)
<br><br>
Decoded DCC may be displayed as shown above, streamed to a terminal, or in table form.  The plugin is available for Windows, Linux, and macOS hosted logic analyzers and works with Saleae Logic 2 software.
<br>

## Quick Start

1. Install Saleae Logic 2 on your computer following Saleae's instructions: https://www.saleae.com/pages/downloads  
2. Download the DCCAnalyzer plugin which is available in the *Releases* section of this repository.  Press the Green *Latest* link on the right side of this page.
3. You only need the file *Analyzer.zip*.  Place this in a temporary folder on your PC.
4. Unzip.  In the unzipped files browse to the directory for your computer's operating system.  There will be only one file there.
5. Copy the file to a permanent location on your computer and remember where you put it.
6. Follow Saleae's instructions to install the plugin. https://support.saleae.com/faq/technical-faq/setting-up-developer-directory

## Connecting to your layout

It is imperative that You familiarize yourself with Saleae Analyzer input specifications, especially for *Supported Voltages* and *Logic Thresholds*. https://support.saleae.com/user-guide.  If you connect your Saleae Analyzer directly to the rails you risk violating the hardware's over voltage protection safety rating.  Furthermore the DCC rails do not share a ground reference with the analyzer.

We recommend using the following resistor network to reduce DCC voltage and to support a *gentle* ground reference for your analyzer.
<br><br>
![alt text](img/ResNet.png)

# For Developers

This section shows you how to download the project files for the DCCAnalyzer Plugin and how to build it.  There are instructions for macOS, Linux, and Windows. 

## MacOS

You may build for X86 based Macintosh versions or Apple's Arm Macs by following these steps.

1. Install XCode with command line tools, available at: https://apps.apple.com/us/app/xcode/id497799835.
2. Then Install command line tools as follows:
```bash
xcode-select --install
```
3. Then open XCode, open Preferences from the main menu, go to locations, and select the only option under 'Command line tools'.
4. Download  the cmake-*-Darwin-x86_64.dmg file: https://cmake.org/download/
5. Double-click the .dmg file to mount it.
6. Drag the CMake application to the Applications folder (or any other desired location).
7. Add CMake to your system's PATH environment variable so you can run it from the command line. This can be done by adding the following line to your .bashrc or .zshrc file:
```bash
export PATH="/Applications/CMake.app/Contents/bin:$PATH"
```

### Building the analyzer

```bash
mkdir build
cd build
cmake ..
cmake --build .
cd Analyzers
install_name_tool -change @executable_path/libAnalyzer.dylib @rpath/libAnalyzer.dylib libdcc_analyzer.so
```

## Ubuntu

Follow these steps to build for Linux Ubuntu
1. Install the latest *build-essential* package as follows:
```bash
* sudo aptitude update
* sudo aptitude install build-essential
```
2. Download the latest CMake for Linux available at: https://cmake.org/download/
3. Download the latest gcc compiler available at: https://gcc.gnu.org.  Note: this should have been installed automatically when *build-essential* was installed.

### Building the analyzer
Open a terminal at the root of the project
* mkdir build
* cd build
* cmake ..
* cmake --build .

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
