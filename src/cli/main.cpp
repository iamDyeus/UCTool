// Entry point for CLI
#include <iostream>
#include <string>
#include <cstring>

extern void performLexicalAnalysis(const char* filename);
extern void performParsing();

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <filename> [--lexical] [--parse]\n";
        return 1;
    }

    std::string filename;
    bool lexical_mode = false;
    bool parse_mode = false;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--lexical") == 0) {
            lexical_mode = true;
        } else if (std::strcmp(argv[i], "--parse") == 0) {
            parse_mode = true;
        } else {
            filename = argv[i];
        }
    }

    if (!lexical_mode && !parse_mode) {
        std::cerr << "Error: At least one of --lexical or --parse is required\n";
        std::cerr << "Usage: " << argv[0] << " <filename> [--lexical] [--parse]\n";
        return 1;
    }

    if (filename.empty()) {
        std::cerr << "Error: No input file specified\n";
        std::cerr << "Usage: " << argv[0] << " <filename> [--lexical] [--parse]\n";
        return 1;
    }

    if (lexical_mode) {
        performLexicalAnalysis(filename.c_str());
    }
    if (parse_mode) {
        performParsing();
    }

    return 0;
}