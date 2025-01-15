CC=clang
CFLAGS = -O0 -Wall -Wpedantic -Wextra -std=c11 -ggdb

SRC = src
OBJ = objs

SRCS = $(wildcard $(SRC)/*.c)
OBJS = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

TEST_SRCS = $(filter-out $(SRC)/main.c, $(SRCS))
TEST_OBJS = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(TEST_SRCS))

MAIN_SRCS = $(filter-out $(SRC)/test.c, $(SRCS))
MAIN_OBJS = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(MAIN_SRCS))

BINDIR = bin
BIN = $(BINDIR)/ll
TESTBIN = $(BINDIR)/test

all: $(BIN) $(TESTBIN)

$(BIN): $(MAIN_OBJS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CINCLUDES) $^ -o $@ $(CLIBS)

$(TESTBIN): $(TEST_OBJS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CINCLUDES) $^ -o $@ $(CLIBS)

$(OBJ)/%.o: $(SRC)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CINCLUDES) -c $< -o $@

clean:
	rm -rf $(BINDIR) $(OBJ)
	rm -rf tests/*results*

$(OBJ):
	@mkdir -p $@

test: $(TESTBIN)
	$(TESTBIN)

run: $(BIN)
	$(BIN)

