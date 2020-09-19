TARGET  := libDCC.so
SDK_DIR := ../../../../Kingst/KingstVIS_Analyzer_SDK

LINK := -L "$(SDK_DIR)/lib/Linux" -lAnalyzer

CC       := g++
HFILE    := $(SDK_DIR)/inc/*.h
SRC      := ../../src/*.cpp
INC      := -I $(SDK_DIR)/inc/
CXXFLAGS := -Wall -O2 -c
FPIC     := -fPIC
SHARE    := -shared -o
OBJ      := *.o

$(TARGET) : $(HFILe) $(SRC)
	$(CC) $(CXXFLAGS) $(FPIC) $(SRC) $(INC)
	$(CC) $(SHARE) $(TARGET) $(OBJ) $(LINK)

