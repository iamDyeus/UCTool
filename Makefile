# Makefile for building UCTool
CC = g++
FLEX = flex
BISON = bison
CFLAGS = -I./src/include -std=c++17 -Wall -DYY_NO_UNISTD_H
LDFLAGS = -lfl -ljsoncpp

# Directories
SRC_DIR = src/executors
CLI_SRC_DIR = src/cli
AI_SRC_DIR = src/ai
INCLUDE_DIR = src/include
BUILD_DIR = build
OUT_DIR = out
TEMP_DIR = tools-temp

# Output Files
LEXER_C = $(TEMP_DIR)/lex.yy.c
PARSER_C = $(TEMP_DIR)/parser.yy.c
PARSER_H = $(TEMP_DIR)/parser.yy.h
PARSER_OUTPUT = $(TEMP_DIR)/parser.yy.output

# Source Files
COMMON_SRCS = $(SRC_DIR)/AST.cpp \
              $(SRC_DIR)/DAG.cpp \
              $(SRC_DIR)/Parser.cpp \
              $(SRC_DIR)/SymbolTable.cpp \
              $(SRC_DIR)/TAC.cpp \
              $(SRC_DIR)/SemanticAnalyzer.cpp

MAINLIKE_SRCS = $(SRC_DIR)/semantic_main.cpp \
                $(SRC_DIR)/tac_main.cpp

# Object Files
COMMON_OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(COMMON_SRCS))
MAINLIKE_OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(MAINLIKE_SRCS))
AI_OBJS = $(AI_SRC_DIR)/llm_explainer.o $(AI_SRC_DIR)/gemini_client.o
MAIN_OBJ = $(BUILD_DIR)/main.o
OBJS = $(BUILD_DIR)/lex.yy.o \
       $(BUILD_DIR)/lex-main.o \
       $(BUILD_DIR)/parser.yy.o \
       $(BUILD_DIR)/parser-main.o \
       $(COMMON_OBJS) \
       $(MAINLIKE_OBJS) \
       $(AI_OBJS) \
       $(MAIN_OBJ)

# Final Executable
TARGET = $(OUT_DIR)/uctool

# Default target
all: directories $(TARGET)

# Create directories
directories:
	@mkdir -p $(BUILD_DIR) $(OUT_DIR) $(TEMP_DIR) $(CLI_SRC_DIR) temp

# Link executable
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

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

# Build Common Object Files (with corresponding headers)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(INCLUDE_DIR)/%.h
	$(CC) $(CFLAGS) -c $< -o $@

# Build Main-like Object Files (no header dependency)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Build AI Object Files
$(AI_SRC_DIR)/llm_explainer.o: $(AI_SRC_DIR)/llm_explainer.cpp $(AI_SRC_DIR)/llm_explainer.h $(AI_SRC_DIR)/gemini_client.h
	$(CC) $(CFLAGS) -c $(AI_SRC_DIR)/llm_explainer.cpp -o $@

$(AI_SRC_DIR)/gemini_client.o: $(AI_SRC_DIR)/gemini_client.cpp $(AI_SRC_DIR)/gemini_client.h
	$(CC) $(CFLAGS) -c $(AI_SRC_DIR)/gemini_client.cpp -o $@

# Build CLI Main Object
$(BUILD_DIR)/main.o: $(CLI_SRC_DIR)/main.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Cleanup
clean:
	rm -rf $(BUILD_DIR)/* $(TEMP_DIR)/* $(OUT_DIR)/* temp/*

.PHONY: all clean directories