# Makefile for building UCTool
CC = g++
FLEX = flex
BISON = bison
CFLAGS = -I./src/include -std=c++17 -DYY_NO_UNISTD_H
LDFLAGS = -lfl

# Directories
SRC_DIR = src/executors
INCLUDE_DIR = src/include
BUILD_DIR = build
OUT_DIR = out
TEMP_DIR = tools-temp

# Output Files
LEXER_C = $(TEMP_DIR)/lex.yy.c
PARSER_C = $(TEMP_DIR)/parser.yy.c
PARSER_H = $(TEMP_DIR)/parser.yy.h
PARSER_OUTPUT = $(TEMP_DIR)/parser.yy.output

# Object Files
OBJS = $(BUILD_DIR)/lex.yy.o \
       $(BUILD_DIR)/lex-main.o \
       $(BUILD_DIR)/parser.yy.o \
       $(BUILD_DIR)/parser-main.o \
       $(BUILD_DIR)/uctool-main.o

# Final Executable
TARGET = $(OUT_DIR)/uctool

all: directories $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

# Ensure necessary directories exist
directories:
	@mkdir -p $(BUILD_DIR) $(OUT_DIR) $(TEMP_DIR)

# Build Lexer
$(BUILD_DIR)/lex.yy.o: $(LEXER_C) $(INCLUDE_DIR)/lexer.h $(PARSER_H)
	$(CC) $(CFLAGS) -c $(LEXER_C) -o $@

$(LEXER_C): $(SRC_DIR)/lexer.l $(INCLUDE_DIR)/lexer_utils.hpp $(PARSER_H)
	$(FLEX) -o $@ $<

# Build Lexer Main
$(BUILD_DIR)/lex-main.o: $(SRC_DIR)/lex-main.cpp $(INCLUDE_DIR)/lexer_utils.hpp $(INCLUDE_DIR)/lexer.h $(LEXER_C)
	$(CC) $(CFLAGS) -c $< -o $@

# Build Parser
$(BUILD_DIR)/parser.yy.o: $(PARSER_C) $(PARSER_H) $(INCLUDE_DIR)/parser_utils.hpp $(INCLUDE_DIR)/lexer_utils.hpp
	$(CC) $(CFLAGS) -c $(PARSER_C) -o $@

$(PARSER_C) $(PARSER_H) $(PARSER_OUTPUT): $(SRC_DIR)/parser.y
	$(BISON) -d -o $(PARSER_C) $<

# Build Parser Main
$(BUILD_DIR)/parser-main.o: $(SRC_DIR)/parser-main.cpp $(INCLUDE_DIR)/parser_utils.hpp $(PARSER_H)
	$(CC) $(CFLAGS) -c $< -o $@

# Build UCTool Main
$(BUILD_DIR)/uctool-main.o: src/cli/main.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Cleanup
clean:
	rm -rf $(BUILD_DIR)/* $(TEMP_DIR)/* $(OUT_DIR)/*

.PHONY: all clean run directories
