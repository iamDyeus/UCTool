# UCTool - Unified Compiler Stage Tool

### 🚀 A CLI-based wrapper to integrate multiple compiler phases seamlessly!

UCTool is a command-line tool designed to unify and streamline the execution of compiler stages by invoking existing tools like **Flex**, **Bison**, and **LLVM**. Additionally, it integrates an AI-powered explanation system to describe each stage.

> [!Important]
> Project under active development. Please Check Back Later!

## 📌 Features

- **Unified Execution**: Run one or multiple compiler phases based on user input.
- **Command-Based Execution**: Execute specific stages via CLI arguments.
- **AI-Powered Help**: Uses an LLM to explain compilation processes.
- **Modular Design**: Calls existing tools instead of implementing them from scratch.

## ⚙️ How It Works

UCTool acts as a wrapper around well-known compiler tools:

```sh
# Run Lexical & Syntax Analysis
uctool file1.l --lexical --syntax
```

Calls **Flex** for lexical analysis and **Bison** for syntax analysis.

```sh
# Run Full Compilation Pipeline
uctool file2.l --compile
```

Executes all compilation stages sequentially.

```sh
# Get AI Explanation for Stages
uctool file3.l --lexical --syntax --help
```

Runs lexical and syntax analysis, then invokes the LLM for explanations.

## 🏗️ Project Structure

```
UCTool/
│── src/                      # Source code
│   │── cli/                  # CLI handling
│   │   ├── main.cpp          # Entry point for CLI
│   │   ├── command_parser.cpp  # Parses CLI arguments
│   │   ├── command_parser.h    # Header file for CLI parsing
│   │── executors/            # Handles execution of external tools
│   │   ├── flex_runner.cpp   # Calls Flex for lexical analysis
│   │   ├── flex_runner.h     # Header for Flex execution
│   │   ├── bison_runner.cpp  # Calls Bison for syntax analysis
│   │   ├── bison_runner.h    # Header for Bison execution
│   │   ├── llvm_runner.cpp   # Calls LLVM for IR generation & optimization
│   │   ├── llvm_runner.h     # Header for LLVM execution
│   │── ai/                   # AI-powered explanation system
│   │   ├── llm_explainer.cpp # Calls LLM models from C++
│   │   ├── llm_explainer.h   # Header for AI explanation system
│   │   ├── prompt_templates/ # JSON templates for AI prompts
│   │       ├── lexical.json  # LLM prompt for lexical analysis
│   │       ├── syntax.json   # LLM prompt for syntax analysis
│   │       ├── semantic.json # LLM prompt for semantic analysis
│── tests/                    # Unit tests
│   │── test_cli.cpp          # Tests CLI functionality
│   │── test_executors.cpp    # Tests tool execution logic
│── docs/                     # Documentation files
│   │── README.md             # Main project documentation
│   │── USAGE.md              # Usage guide
│   │── ARCHITECTURE.md       # Technical details
│── scripts/                  # Utility scripts
│   │── install_deps.sh       # Installs required dependencies
│── config/                   # Configuration files
│   │── tool_paths.json       # Paths to external tools
│── Makefile                  # Build system configuration
```

## 🛠️ Setup

### 🔹 Prerequisites

- **C++ Compiler** (GCC/Clang/MSVC)
- **Flex & Bison** (for lexical and syntax analysis)
- **LLVM** (for intermediate representation & optimizations)
- **LLM API** (for AI-powered explanations)

### 🔹 Installation

```sh
git clone https://github.com/iamDyeus/UCTool.git
cd UCTool
make   # or use cmake for cross-platform builds
```

## 🎯 Usage

Run the tool with the following options:

```sh
uctool <filename> [options]
```

| Option       | Description                                |
| ------------ | ------------------------------------------ |
| `--lexical`  | Perform lexical analysis using Flex        |
| `--syntax`   | Perform syntax analysis using Bison        |
| `--semantic` | Perform semantic analysis (AST validation) |
| `--compile`  | Run the entire compilation pipeline        |
| `--help`     | Get AI-powered explanations for stages     |

## 📜 License

MIT License - See [LICENSE](LICENSE) for details.

---

🔧 **UCTool** - Making Compiler Stages Seamless! 🚀
