SRC_DIR := src
OBJ_DIR := obj

# all src files
SRC := $(wildcard $(SRC_DIR)/*.c)
# all objects
OBJ := $(OBJ_DIR)/y.tab.o $(OBJ_DIR)/lex.yy.o $(OBJ_DIR)/parse.o \
		$(OBJ_DIR)/respend.o $(OBJ_DIR)/log.o $(OBJ_DIR)/buffer.o 

# all binaries
BIN := example liso_server liso_client
# C / C++ compiler
CC  := gcc
# C PreProcessor Flag
CPPFLAGS := -Iinclude
# compiler flags
CFLAGS   := -g -Wall
# DEPS = parse.h y.tab.h

default: all
all : example liso_server liso_client

liso_server: $(OBJ) $(OBJ_DIR)/echo_server.o 
	$(CC) -Werror $^ -o $@

liso_client: $(OBJ_DIR)/echo_client.o 
	$(CC) -Werror $^ -o $@

example: $(OBJ) $(OBJ_DIR)/example.o 
	$(CC) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@


$(SRC_DIR)/lex.yy.c: $(SRC_DIR)/lexer.l
	flex -o $@ $^

$(SRC_DIR)/y.tab.c: $(SRC_DIR)/parser.y
	yacc -d $^
	mv y.tab.c $@
	mv y.tab.h $(SRC_DIR)/y.tab.h

$(OBJ_DIR):
	mkdir $@


clean:
	$(RM) $(OBJ) $(BIN) $(SRC_DIR)/lex.yy.c $(SRC_DIR)/y.tab.*
	$(RM) -r $(OBJ_DIR)
