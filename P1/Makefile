CC = gcc
CFLAGS = -g -Wall

PATH_SRC = ./src
PATH_INC = $(PATH_SRC)/include
PATH_OBJ = ./obj

OUT_RESTAURANT  = restaurant
OUT_SUPPLIER = supplier
OUT_CUSTOMER = customer

# -------------------------------------------------------------------------

vpath %.c $(PATH_SRC)
vpath %.h $(PATH_INC)

OBJS_RESTAURANT  = restaurant.o str.o network.o json.o
OBJS_SUPPLIER = supplier.o str.o network.o
OBJS_CUSTOMER = customer.o str.o network.o json.o

# -------------------------------------------------------------------------

all:	$(PATH_OBJ) $(OUT_RESTAURANT) $(OUT_SUPPLIER) $(OUT_CUSTOMER)

$(OUT_CUSTOMER):	$(addprefix $(PATH_OBJ)/, $(OBJS_CUSTOMER))
	$(CC) $(CFLAGS) -o $@ $^ -lm

$(OUT_RESTAURANT):	$(addprefix $(PATH_OBJ)/, $(OBJS_RESTAURANT))
	$(CC) $(CFLAGS) -o $@ $^ -lm

$(OUT_SUPPLIER):	$(addprefix $(PATH_OBJ)/, $(OBJS_SUPPLIER))
	$(CC) $(CFLAGS) -o $@ $^ -lm

$(PATH_OBJ)/supplier.o:	supplier.c json.h data.h str.h network.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(PATH_OBJ)/restaurant.o:	restaurant.c json.h data.h str.h network.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(PATH_OBJ)/customer.o:	customer.c json.h data.h str.h network.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(PATH_OBJ)/network.o:	network.c network.h str.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(PATH_OBJ)/str.o:	str.c str.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(PATH_OBJ)/json.o:	json.c json.h
	$(CC) $(CFLAGS) -c -o $@ $<

# -------------------------------------------------------------------------

$(PATH_OBJ):	; mkdir -p $@

.PHONY:	all clean

clean:
	rm -rf $(PATH_OBJ) $(OUT_RESTAURANT) $(OUT_SUPPLIER) $(OUT_CUSTOMER) > /dev/null 2>&1

r:	all
	./$(OUT_RESTAURANT) 8080

s:	all
	./$(OUT_SUPPLIER) 8080

c:	all
	./$(OUT_CUSTOMER) 8080