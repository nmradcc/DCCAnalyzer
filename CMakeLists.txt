cmake_minimum_required (VERSION 3.11)
project(dcc_analyzer)

add_definitions( -DLOGIC2 )

# enable generation of compile_commands.json, helpful for IDEs to locate include files.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# custom CMake Modules are located in the cmake directory.
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(ExternalAnalyzerSDK)

set(SOURCES 
src/DCCAnalyzer.cpp
src/DCCAnalyzer.h
src/DCCAnalyzerResults.cpp
src/DCCAnalyzerResults.h
src/DCCAnalyzerSettings.cpp
src/DCCAnalyzerSettings.h
src/DCCSimulationDataGenerator.cpp
src/DCCSimulationDataGenerator.h
)

add_analyzer_plugin(dcc_analyzer SOURCES ${SOURCES})
