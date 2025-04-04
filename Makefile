DIR_BASE=$(shell pwd)
FLAGS_INCLUDE=-I$(DIR_BASE)/include
FLAGS_ROOT=$(shell root-config --cflags --libs)
FLAGS_MINUIT=-lMinuit

kit/TreeClone.exe: kit/TreeClone.cpp
	g++ -o $@ $^ $(FLAGS_INCLUDE) $(FLAGS_ROOT)

macro/NumContribCalibrationTest.exe: macro/NumContribCalibrationTest.cpp
	g++ -o $@ $^ $(FLAGS_INCLUDE) $(FLAGS_ROOT) $(FLAGS_MINUIT)

macro/SE_PR.exe: macro/SE_PR.cpp
	g++ -o $@ $^ $(FLAGS_INCLUDE) $(FLAGS_ROOT) $(FLAGS_MINUIT)

macro/ME_PR.exe: macro/ME_PR.cpp
	g++ -o $@ $^ $(FLAGS_INCLUDE) $(FLAGS_ROOT) $(FLAGS_MINUIT)

macro/plot_PR.exe: macro/plot_PR.cpp
	g++ -o $@ $^ $(FLAGS_INCLUDE) $(FLAGS_ROOT) $(FLAGS_MINUIT)