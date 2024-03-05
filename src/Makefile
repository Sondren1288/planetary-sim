DEPS := constants.cpp body.cpp model.cpp types.cpp
DEP_FILES := body.hpp
ROOT_DEP := `root-config --glibs`
ROOT_DEP_2 := `root-config --cflags`
NAME := Project.out


$(NAME): main.cpp
	g++ main.cpp -o $(NAME) $(DEPS) $(DEP_FILES) $(ROOT_DEP) $(ROOT_DEP_2)
