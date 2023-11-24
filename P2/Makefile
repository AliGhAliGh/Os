CC = g++
CFLAGS = -g -Wall

PATH_SRC = ./src
PATH_INC = $(PATH_SRC)/include
PATH_OBJ = ./obj

OUT_BILL  = bill
OUT_BILL_CALC = bill_calc
OUT_BUILDING = building
OUT_METER = meter

# -------------------------------------------------------------------------

vpath %.cpp $(PATH_SRC)
vpath %.hpp $(PATH_INC)

OBJS_BILL  = bill.o log.o util.o
OBJS_BILL_CALC = bill_calc.o log.o util.o
OBJS_BUILDING = building.o log.o util.o
OBJS_METER = meter.o log.o util.o

# -------------------------------------------------------------------------

all:	$(PATH_OBJ) $(OUT_BILL) $(OUT_BILL_CALC) $(OUT_BUILDING) $(OUT_METER)

$(OUT_BUILDING):	$(addprefix $(PATH_OBJ)/, $(OBJS_BUILDING))
	$(CC) $(CFLAGS) -o $@ $^

$(OUT_BILL):	$(addprefix $(PATH_OBJ)/, $(OBJS_BILL))
	$(CC) $(CFLAGS) -o $@ $^

$(OUT_BILL_CALC):	$(addprefix $(PATH_OBJ)/, $(OBJS_BILL_CALC))
	$(CC) $(CFLAGS) -o $@ $^

$(OUT_METER):	$(addprefix $(PATH_OBJ)/, $(OBJS_METER))
	$(CC) $(CFLAGS) -o $@ $^

# -------------------------------------------------------------------------

$(PATH_OBJ)/bill_calc.o:	bill_calc.cpp log.hpp color.hpp defs.hpp
	$(CC) $(CFLAGS) -c -o $@ $<

$(PATH_OBJ)/bill.o:	bill.cpp log.hpp color.hpp defs.hpp
	$(CC) $(CFLAGS) -c -o $@ $<

$(PATH_OBJ)/meter.o:	meter.cpp log.hpp color.hpp defs.hpp
	$(CC) $(CFLAGS) -c -o $@ $<

$(PATH_OBJ)/building.o:	building.cpp log.hpp color.hpp defs.hpp building.hpp
	$(CC) $(CFLAGS) -c -o $@ $<

# -------------------------------------------------------------------------

$(PATH_OBJ)/log.o:	log.cpp log.hpp color.hpp
	$(CC) $(CFLAGS) -c -o $@ $<

$(PATH_OBJ)/util.o:	util.cpp defs.hpp log.hpp
	$(CC) $(CFLAGS) -c -o $@ $<

# -------------------------------------------------------------------------

$(PATH_OBJ):	; mkdir -p $@

.PHONY:	all clean

clean:
	rm -rf $(PATH_OBJ) $(OUT_BILL) $(OUT_BILL_CALC) $(OUT_BUILDING) $(OUT_METER) > /dev/null 2>&1