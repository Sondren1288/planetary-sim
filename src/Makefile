DEPS := constants.cpp body.cpp model.cpp types.cpp
DEP_FILES := body.hpp
ROOT_DEP := `root-config --glibs`
ROOT_DEP_2 := `root-config --cflags`
NAME := Project


$(NAME).out: main.cpp $(DEPS)
	g++ main.cpp -o $(NAME).out $(DEPS) $(DEP_FILES) $(ROOT_DEP) $(ROOT_DEP_2)

$(NAME).debug: main.cpp $(DEPS)
	g++ main.cpp -g -o $(NAME).debug $(DEPS) $(DEP_FILES) $(ROOT_DEP) $(ROOT_DEP_2)


all: $(NAME).out $(NAME).debug
.PHONY: all

debug: $(NAME).debug
.PHONY: debug

.PHONY: clean


clean: 
	rm $(NAME).out $(NAME).debug
