# DCCAnalyzer

This repository is for maintaining the software associated with decoding DCC packets using a logic analyzer.

The nmradcc/DCCAnalyzer/src directory contains the source code for the parsing functions necessary to support the following two analyzers.

The nmradcc/DCCAnalyzer/Kingst directory contains the build files needed to build the software analzyer (DCCAnalyzer) for the Kingst LA2016 digital logic analyzer.

The nmradcc/DCCAnalyzer/Saleae directory contains the build files needed to build the software analyzer (DCCANalyzer) for the Saleae Logic Pro 8 digital logic analyzer.

Use the following directory structure to build for either of these environments:

ProjectRootDirectory
        |
        |
        +------nmradcc (issue the command 'git clone https://github.com/nmradcc/DCCAnalyzer.git' here and it will put the DCCAnalyzer directory here:)
        |         |
        |         |
        |         +------DCCAnalzer
        |                     |
        |                     |
        |                     +-------src (common source files for DCCAnalyzer)
        |                     |
        |                     |
        |                     +-------Kingst (directory to build for the Kingst Analyzer)
        |                     |
        |                     |
        |                     +-------Saleae (directory to build for the Saleae Analyzer)
        |
        +------Kingst (copy the directory KingstVIS\_Analyzer\_SDK when you download it from the Kingst website)
        |         |
        |         |
        |         +------KinstVIS\_Analyzer\_SDK
        |
        +------Saleae (issue the command 'git clone https://github.com/saleae/AnalyzerSDK.git' here and it will put the AnalyzerSDK directory here)
                  |
                  |
                  +------AnalyzerSDK
                         (note: in order to build for Logic2, you need to issue 'git switch alpha' to get the Logic2 dev branch)

