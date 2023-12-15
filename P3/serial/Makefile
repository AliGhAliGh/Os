PATH_SRC = ./src
PATH_INC = $(PATH_SRC)/include
PATH_OBJ = ./obj

ARGS = ../input.bmp

CPP = g++
CPP_FLAGS = -Wall -std=c++17

ALL_OUT  = ImageFilters.out

# -------------------------------------------------------------------------

vpath %.cpp $(PATH_SRC)
vpath %.hpp $(PATH_INC)

OBJS  = main.o bmp.o

# -------------------------------------------------------------------------

all:	$(PATH_OBJ) $(ALL_OUT)

$(ALL_OUT):	$(addprefix $(PATH_OBJ)/, $(OBJS))
	$(CPP) $(CPP_FLAGS) -o $@ $^

# -------------------------------------------------------------------------

$(PATH_OBJ)/main.o:	main.cpp bmp.hpp
	$(CPP) $(CPP_FLAGS) -c -o $@ $<

$(PATH_OBJ)/bmp.o:	bmp.cpp bmp.hpp
	$(CPP) $(CPP_FLAGS) -c -o $@ $<

# -------------------------------------------------------------------------

$(PATH_OBJ):	; mkdir -p $@

# -------------------------------------------------------------------------

.PHONY:	all clean copy_main run reset

clean:
	rm -rf $(PATH_OBJ) $(ALL_OUT) > /dev/null 2>&1

run:	all
	./$(ALL_OUT) $(ARGS)