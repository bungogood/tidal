SRC_DIR := src
BIN_DIR := .
LIB_DIR := lib
INC_DIR := inc
OBJ_DIR := .obj

TARGET  := $(BIN_DIR)/chess

CC = gcc
CCFLAGS = -c -g -Wall

SOURCES := $(shell find $(SRC_DIR) -name "*.c")
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
HEADERS := $(shell find $(INC_DIR) -name "*.h")
# sort is just to remove duplicates. Not necessary, but tidy
HEADERDIRS := $(sort $(dir $(HEADERS)))
INCLUDEFLAGS := $(addprefix -I,$(HEADERDIRS))

.PHONY: all clean

all: $(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	@$(CC) $< $(INCLUDEFLAGS) -o $@ $(CCFLAGS)

$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CC) $^ -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

