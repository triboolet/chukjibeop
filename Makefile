.PHONY: debug release all clean

CC = gcc
DEBUG_CFLAGS = -Wall -Wextra -g
RELEASE_CFLAGS = -Wall -Wextra -O2
INCLUDE_DIRS =/opt/homebrew/include
LIBS_DIRS =/opt/homebrew/lib
LIBS= -lgmp -lcrypto
OBJDIR = build
BINDIR = bin
SRCDIR = src

SOURCES := $(wildcard $(SRCDIR)/*.c)
BASENAMES := $(notdir $(SOURCES:.c=)) 
OBJECTS := $(addprefix $(OBJDIR)/, $(addsuffix .o, $(BASENAMES)))
TARGET = $(BINDIR)/btc

debug: CFLAGS = $(DEBUG_CFLAGS)
debug : all

release : CFLAGS = $(RELEASE_CFLAGS)
release: all

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIRS) -L$(LIBS_DIRS) $(LIBS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIRS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(BINDIR)

# === Tests configuration ===
TESTDIR = tests
TEST_SOURCES := $(wildcard $(TESTDIR)/*.c)
TEST_BASENAMES := $(notdir $(TEST_SOURCES:.c=))
TEST_OBJECTS := $(addprefix build/tests/, $(addsuffix .o, $(TEST_BASENAMES)))
TEST_BIN := bin/tests/tests

TEST_OBJDIR = build/tests
TEST_BINDIR = bin/tests
CHECK_FLAGS := -I/opt/homebrew/include -L/opt/homebrew/lib -lcheck

# we need to remove the main function from our lib
LIB_SOURCES := $(filter-out $(SRCDIR)/main.c, $(SOURCES))
LIB_BASENAMES := $(notdir $(LIB_SOURCES:.c=))
LIB_OBJECTS := $(addprefix $(OBJDIR)/, $(addsuffix .o, $(LIB_BASENAMES)))

tests: CFLAGS = $(RELEASE_CFLAGS)
tests: $(TEST_BIN)
	@echo "Running tests..."
	./$(TEST_BIN)

$(TEST_BIN): $(TEST_OBJECTS) $(LIB_OBJECTS)
	@mkdir -p $(TEST_BINDIR)
	$(CC) $(CFLAGS) $(TEST_OBJECTS) $(LIB_OBJECTS) -I$(INCLUDE_DIRS) -Isrc -L$(LIBS_DIRS) $(LIBS) $(CHECK_FLAGS) -o $@

$(TEST_OBJDIR)/%.o: $(TESTDIR)/%.c
	@mkdir -p $(TEST_OBJDIR)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIRS) -Isrc -c $< -o $@
