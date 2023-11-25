PATH_SRC = ./src
PATH_INC = $(PATH_SRC)/include
PATH_OBJ = ./obj
PATH_BIN = ./bin
PATH_FIFO = ./.fifo

ARGS = ./buildings

CPP = g++
CPP_FLAGS = -Wall -std=c++17

OUT_BILL  = bill
OUT_BILL_CALC = bill_calc
OUT_BUILDING = building
OUT_METER = meter

ALL_OUT  = $(OUT_BILL) $(OUT_BILL_CALC) $(OUT_BUILDING) $(OUT_METER)

# -------------------------------------------------------------------------

vpath %.cpp $(PATH_SRC)
vpath %.hpp $(PATH_INC)

OBJS_BILL  = bill.o log.o util.o
OBJS_BILL_CALC = bill_calc.o log.o util.o
OBJS_BUILDING = building.o log.o util.o
OBJS_METER = meter.o log.o util.o

# -------------------------------------------------------------------------

all:	$(PATH_BIN) $(PATH_OBJ) $(PATH_FIFO) $(addprefix $(PATH_BIN)/, $(ALL_OUT)) copy_main

$(PATH_BIN)/$(OUT_BUILDING):	$(addprefix $(PATH_OBJ)/, $(OBJS_BUILDING))
	$(CPP) $(CPP_FLAGS) -o $@ $^

$(PATH_BIN)/$(OUT_BILL):	$(addprefix $(PATH_OBJ)/, $(OBJS_BILL))
	$(CPP) $(CPP_FLAGS) -o $@ $^

$(PATH_BIN)/$(OUT_BILL_CALC):	$(addprefix $(PATH_OBJ)/, $(OBJS_BILL_CALC))
	$(CPP) $(CPP_FLAGS) -o $@ $^

$(PATH_BIN)/$(OUT_METER):	$(addprefix $(PATH_OBJ)/, $(OBJS_METER))
	$(CPP) $(CPP_FLAGS) -o $@ $^

# -------------------------------------------------------------------------

$(PATH_OBJ)/bill_calc.o:	bill_calc.cpp log.hpp color.hpp defs.hpp
	$(CPP) $(CPP_FLAGS) -c -o $@ $<

$(PATH_OBJ)/bill.o:	bill.cpp log.hpp color.hpp defs.hpp
	$(CPP) $(CPP_FLAGS) -c -o $@ $<

$(PATH_OBJ)/meter.o:	meter.cpp log.hpp color.hpp defs.hpp
	$(CPP) $(CPP_FLAGS) -c -o $@ $<

$(PATH_OBJ)/building.o:	building.cpp log.hpp color.hpp defs.hpp building.hpp
	$(CPP) $(CPP_FLAGS) -c -o $@ $<

# -------------------------------------------------------------------------

$(PATH_OBJ)/log.o:	log.cpp log.hpp color.hpp
	$(CPP) $(CPP_FLAGS) -c -o $@ $<

$(PATH_OBJ)/util.o:	util.cpp defs.hpp log.hpp
	$(CPP) $(CPP_FLAGS) -c -o $@ $<

# -------------------------------------------------------------------------

$(PATH_OBJ):	; mkdir -p $@

$(PATH_FIFO):	; mkdir -p $@

$(PATH_BIN):	; mkdir -p $@

# -------------------------------------------------------------------------

.PHONY:	all clean copy_main run reset

copy_main:
	cp $(PATH_BIN)/$(OUT_BILL) .

clean:
	rm -rf $(PATH_OBJ) $(PATH_BIN) $(PATH_FIFO) $(OUT_BILL) > /dev/null 2>&1

reset:
	rm -rf $(PATH_FIFO) > /dev/null 2>&1

run:	all
	./$(OUT_BILL) $(ARGS)