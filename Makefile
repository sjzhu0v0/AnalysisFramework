DIR_BASE=$(shell pwd)
FLAGS_INCLUDE=-I$(DIR_BASE)/include
FLAGS_ROOT=$(shell root-config --cflags --libs) 
FLAGS_MINUIT=-lMinuit 

kit/TreeClone.exe: kit/TreeClone.exe
 	g++ -o $@ $^ $(FLAGS_INCLUDE) $(FLAGS_ROOT)

tutorial/RDataFrame_PIDSeparationPower.exe:
	g++ -o $@ $^ $(FLAGS_INCLUDE) $(FLAGS_ROOT) $(FLAGS_MINUIT)