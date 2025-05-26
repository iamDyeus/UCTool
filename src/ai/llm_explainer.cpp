// Calls LLM models from C++

#include "llm_explainer.h"
#include "gemini_client.h"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <regex>
#include <jsoncpp/json/json.h>

std::string get_env_api_key() {
    std::ifstream env_file(".env");
    std::string line;
    while (std::getline(env_file, line)) {
        if (line.find("GEMINI_API_KEY=") == 0) {
            return line.substr(15);
        }
    }
    return "";
}

std::string load_prompt_template(const std::string& stage, const std::string& input_file, const std::string& input_data, const std::string& output_data) {
    std::ifstream prompt_file("src/ai/prompt_templates/help_prompt.txt");
    std::stringstream buffer;
    buffer << prompt_file.rdbuf();
    std::string prompt = buffer.str();
    // Replace placeholders
    size_t pos;
    while ((pos = prompt.find("{{stage}}")) != std::string::npos) prompt.replace(pos, 9, stage);
    while ((pos = prompt.find("{{input_file}}")) != std::string::npos) prompt.replace(pos, 14, input_file);
    while ((pos = prompt.find("{{input_data}}")) != std::string::npos) prompt.replace(pos, 13, input_data);
    while ((pos = prompt.find("{{output_data}}")) != std::string::npos) prompt.replace(pos, 14, output_data);
    return prompt;
}

std::string extract_gemini_text(const std::string& response) {
    // Fallback: just find the first occurrence of "text" and extract everything until the next quote
    std::string key = "\"text\":";
    size_t start = response.find(key);
    if (start == std::string::npos) return response; // fallback: print raw response
    start = response.find('"', start + key.length());
    if (start == std::string::npos) return response;
    ++start;
    size_t end = response.find('"', start);
    if (end == std::string::npos) end = response.length();
    std::string text = response.substr(start, end - start);
    // Unescape common sequences
    size_t pos = 0;
    while ((pos = text.find("\\n", pos)) != std::string::npos) {
        text.replace(pos, 2, "\n");
        pos += 1;
    }
    while ((pos = text.find("\\\"", 0)) != std::string::npos) {
        text.replace(pos, 2, "\"");
    }
    return text;
}

std::string read_file_contents(const std::string& path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string generate_ai_help(const std::string& stage, const std::string& input_file, const std::string& input_data, const std::string& output_data) {
    std::string api_key = get_env_api_key();
    if (api_key.empty()) {
        return "[Gemini API key missing in .env]";
    }
    // Always include the full input file for context
    std::string file_contents = read_file_contents(input_file);
    std::string prompt = load_prompt_template(stage, input_file, file_contents + "\n---\n" + input_data, output_data);
    std::string response = call_gemini_api(prompt, api_key);
    return extract_gemini_text(response);
}
