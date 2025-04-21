DIR_BASE=$(shell pwd)
FLAGS_INCLUDE=-I$(DIR_BASE)/include -I$(DIR_BASE)/macro
FLAGS_ROOT=$(shell root-config --cflags --libs)
FLAGS_MINUIT=-lMinuit

all: \
	kit/TreeClone.exe \
	kit/hadd_center.exe \
	kit/RMerge.exe \
	kit/BSMerge.exe \
	macro/SE_PR.exe \
	macro/ME_PR.exe \
	macro/SE_PR_thn.exe \
	macro/ME_PR_thn.exe \
	macro/SE_RR.exe \
	macro/ME_RR.exe \
	macro/plot_PR.exe \
	macro/RFunc_PR.exe 

kit/TreeClone.exe: kit/TreeClone.cpp
	g++ -o $@ $^ $(FLAGS_INCLUDE) $(FLAGS_ROOT)

kit/hadd_center.exe: kit/hadd_center.cpp
	g++ -o $@ $^ $(FLAGS_INCLUDE) $(FLAGS_ROOT)

kit/RMerge.exe: kit/RMerge.cpp
	g++ -o $@ $^ $(FLAGS_INCLUDE) $(FLAGS_ROOT)

kit/BSMerge.exe: kit/BSMerge.cpp
	g++ -o $@ $^ $(FLAGS_INCLUDE) $(FLAGS_ROOT)

macro/NumContribCalibrationTest.exe: macro/NumContribCalibrationTest.cpp
	g++ -o $@ $^ $(FLAGS_INCLUDE) $(FLAGS_ROOT) $(FLAGS_MINUIT)

macro/SE_PR.exe: macro/SE_PR.cpp
	g++ -o $@ $^ $(FLAGS_INCLUDE) $(FLAGS_ROOT) $(FLAGS_MINUIT)

macro/ME_PR.exe: macro/ME_PR.cpp
	g++ -o $@ $^ $(FLAGS_INCLUDE) $(FLAGS_ROOT) $(FLAGS_MINUIT)

macro/SE_PR_thn.exe: macro/SE_PR_thn.cpp
	g++ -o $@ $^ $(FLAGS_INCLUDE) $(FLAGS_ROOT) $(FLAGS_MINUIT)

macro/ME_PR_thn.exe: macro/ME_PR_thn.cpp
	g++ -o $@ $^ $(FLAGS_INCLUDE) $(FLAGS_ROOT) $(FLAGS_MINUIT)

macro/SE_RR.exe: macro/SE_RR.cpp
	g++ -o $@ $^ $(FLAGS_INCLUDE) $(FLAGS_ROOT) $(FLAGS_MINUIT)

macro/ME_RR.exe: macro/ME_RR.cpp
	g++ -o $@ $^ $(FLAGS_INCLUDE) $(FLAGS_ROOT) $(FLAGS_MINUIT)

macro/plot_PR.exe: macro/plot_PR.cpp
	g++ -o $@ $^ $(FLAGS_INCLUDE) $(FLAGS_ROOT) $(FLAGS_MINUIT)

macro/RFunc_PR.exe: macro/RFunc_PR.cpp
	g++ -o $@ $^ $(FLAGS_INCLUDE) $(FLAGS_ROOT) $(FLAGS_MINUIT)
