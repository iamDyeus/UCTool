# UCTool - Unified Compiler Stage Tool

A CLI tool to unify and streamline the execution of compiler stages (Flex, Bison, LLVM) with optional AI-powered explanations for each stage.

> **Project under active development.**

## Features
- Unified execution of compiler phases via CLI
- Modular: calls existing tools (not re-implementing them)
- AI-powered help: uses Gemini LLM to explain compilation stages

## Quick Setup (Development)

### 1. Install System Dependencies
Run this command to install all required build tools and libraries:

```sh
sudo apt-get update && sudo apt-get install -y flex bison build-essential cmake libtool autoconf automake libjsoncpp-dev
```

### 2. Clone and Build
```sh
git clone https://github.com/iamDyeus/UCTool.git
cd UCTool
make
```

### 3. Add Gemini API Key
Create a `.env` file in the project root (or edit the existing one) and add your Gemini API key:

```
GEMINI_API_KEY=your_actual_gemini_api_key_here
```

## Usage

Run the tool with:

```sh
./out/uctool <filename> [--lexical] [--parse] [--help]
```

- `--lexical` : Run lexical analysis (Flex)
- `--parse`   : Run syntax analysis (Bison)
- `--help`    : Get an AI-powered explanation for the last stage (must be the last argument)

Example:
```sh
./out/uctool example.l --lexical --help
```

## Project Structure
- `src/cli/`         : CLI entry point
- `src/executors/`   : Compiler phase runners (Flex, Bison, etc.)
- `src/ai/`          : AI explanation system (Gemini integration)
- `tests/`           : Unit tests
- `docs/`            : Documentation

## License
MIT License
