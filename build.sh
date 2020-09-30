#!/bin/bash

if [ ! -d build ]; then
  mkdir build;
fi;
cd build
cmake ..
cmake --build .
cd Analyzers
install_name_tool -change @executable_path/libAnalyzer.dylib @rpath/libAnalyzer.dylib libdcc_analyzer.so


