DEPS := constants.cpp body.cpp model.cpp types.cpp
DEP_FILES := body.hpp main.hpp
ROOT_DEP := $(shell root-config --glibs)
ROOT_DEP_2 := $(shell root-config --cflags)
ROOT_LIBS := $(shell root-config --libs)
NAME := Project
DEBUG := Debug

default: dict_output.cxx libRootLib.so $(NAME).out

dict_output.cxx: $(DEP_FILES) linkdef.h
	rootcling -f $@ $^ 

libRootLib.so: dict_output.cxx $(DEPS) main.cpp
	g++ -shared -g -fPIC -o $@ $(ROOT_DEP_2) -I"./" $(ROOT_DEP) $(ROOT_LIBS) $^ 

$(NAME).out: main.cpp $(DEPS) libRootLib.so
	g++ main.cpp -o $(NAME).out $(DEPS) $(DEP_FILES) $(ROOT_DEP) $(ROOT_DEP_2) libRootLib.so

$(DEBUG).debug: main.cpp $(DEPS) libRootLib.so
	g++ main.cpp -g -o $(DEBUG).debug $(DEPS) $(DEP_FILES) $(ROOT_DEP) $(ROOT_DEP_2) libRootLib.so


all: $(NAME).out $(DEBUG).debug 
.PHONY: all

debug: $(DEBUG).debug
.PHONY: debug

.PHONY: clean


clean: 
	rm $(NAME).out $(DEBUG).debug
