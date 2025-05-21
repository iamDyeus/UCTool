#include "gemini_client.h"
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>

std::string call_gemini_api(const std::string& prompt, const std::string& api_key) {
    // Write JSON body (fix: do not include comments or extra lines)
    std::ofstream json_file("/tmp/gemini_body.json");
    json_file << "{\"contents\":[{\"parts\":[{\"text\":\"";
    // Escape double quotes, backslashes, and also split long prompts into chunks if needed
    int chunk_size = 2000; // Gemini API may truncate long input, so send in chunks if needed
    for (size_t i = 0; i < prompt.size(); ++i) {
        char c = prompt[i];
        if (c == '\\' || c == '"') json_file << '\\';
        json_file << c;
        // Insert a newline every chunk_size characters to avoid any buffer/line length issues
        if ((i + 1) % chunk_size == 0) json_file << "\n";
    }
    json_file << "\"}]}]}";
    json_file.close();

    // Prepare curl command
    std::string command =
        "curl -s 'https://generativelanguage.googleapis.com/v1beta/models/gemini-2.0-flash:generateContent?key=" + api_key +
        "' -H 'Content-Type: application/json' -X POST -d @/tmp/gemini_body.json > /tmp/gemini_response.json";

    int ret = std::system(command.c_str());
    if (ret != 0) {
        return "[Error calling Gemini API]";
    }

    // Read response
    std::ifstream resp_file("/tmp/gemini_response.json");
    std::stringstream buffer;
    buffer << resp_file.rdbuf();
    resp_file.close();
    return buffer.str();
}
