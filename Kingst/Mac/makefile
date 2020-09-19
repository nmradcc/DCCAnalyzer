TARGET  := libDCC.dylib
ADIR	:= /Applications/KingstVIS.app/Contents/Resources/Analyzer
SDK_DIR := ../../../../Kingst/KingstVIS_Analyzer_SDK

LINK := -L "$(SDK_DIR)/lib/Mac" -lAnalyzer

CC       := clang++
HFILE    := $(SDK_DIR)/inc/*.h
SRC      := ../../src/*.cpp
INC      := -I $(SDK_DIR)/inc/
CXXFLAGS := -Wall -O2 -c
FPIC     := -fPIC
SHARE    := -dynamiclib -o
OBJ      := *.o

$(TARGET) : $(HFILe) $(SRC)
	$(CC) $(CXXFLAGS) $(FPIC) $(SRC) $(INC)
	$(CC) $(SHARE) $(TARGET) $(OBJ) $(LINK)

install:
	cp $(TARGET) $(ADIR)

setup:
	cd $(ADIR)
	install_name_tool -change "libAnalyzer.dylib" "@executable_path/libAnalyzer.dylib" "$(TARGET)"
