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
    std::ifstream env_file("../.env");
    std::string line;
    while (std::getline(env_file, line)) {
        if (line.find("GEMINI_API_KEY=") == 0) {
            return line.substr(15);
        }
    }
    return "";
}

std::string load_prompt_template(const std::string& stage, const std::string& input_file, const std::string& input_data, const std::string& output_data) {
    std::ifstream prompt_file("../src/ai/prompt_templates/help_prompt.txt");
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
    // Try to parse as JSON and extract the "text" field if present
    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errs;
    std::istringstream iss(response);
    if (Json::parseFromStream(builder, iss, &root, &errs)) {
        // Try to find "text" at the top level or nested
        if (root.isMember("text")) {
            return root["text"].asString();
        }
        // Sometimes Gemini responses are arrays or nested objects
        if (root.isArray() && root.size() > 0 && root[0].isMember("text")) {
            return root[0]["text"].asString();
        }
        // Try to find "text" recursively (for deeply nested cases)
        std::function<std::string(const Json::Value&)> find_text;
        find_text = [&](const Json::Value& val) -> std::string {
            if (val.isObject()) {
                for (const auto& key : val.getMemberNames()) {
                    if (key == "text" && val[key].isString()) {
                        return val[key].asString();
                    }
                    std::string found = find_text(val[key]);
                    if (!found.empty()) return found;
                }
            } else if (val.isArray()) {
                for (const auto& item : val) {
                    std::string found = find_text(item);
                    if (!found.empty()) return found;
                }
            }
            return "";
        };
        std::string found = find_text(root);
        if (!found.empty()) return found;
    }
    // Fallback: return the raw response (don't try to parse quotes)
    return response;
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
