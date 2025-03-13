DIR_BASE=$(shell pwd)
FLAGS_INCLUDE=-I$(DIR_BASE)/include
FLAGS_ROOT=$(shell root-config --cflags --libs)
FLAGS_MINUIT=-lMinuit

kit/TreeClone.exe: kit/TreeClone.cpp
	g++ -o $@ $^ $(FLAGS_INCLUDE) $(FLAGS_ROOT)

tutorial/RDataFrame_PIDSeparationPower.exe:     tutorial/RDataFrame_PIDSeparationPower.cpp
	g++ -o $@ $^ $(FLAGS_INCLUDE) $(FLAGS_ROOT) $(FLAGS_MINUIT)

tutorial/MultStudy.exe:	tutorial/MultStudy.cpp
	g++ -o $@ $^ $(FLAGS_INCLUDE) $(FLAGS_ROOT) $(FLAGS_MINUIT)

macro/TriggerStudy.exe:	macro/TriggerStudy.cpp
	g++ -o $@ $^ $(FLAGS_INCLUDE) $(FLAGS_ROOT) $(FLAGS_MINUIT)

macro/Control_MultShow.exe:	macro/Control_MultShow.cpp
	g++ -o $@ $^ $(FLAGS_INCLUDE) $(FLAGS_ROOT) $(FLAGS_MINUIT)

macro/NumContribCalibrationTest.exe: macro/NumContribCalibrationTest.cpp
	g++ -o $@ $^ $(FLAGS_INCLUDE) $(FLAGS_ROOT) $(FLAGS_MINUIT)