#pragma once
#include <string>
#include <vector>
#include <cctype>

inline std::vector<std::string> split_words(const std::string& text) {
    std::vector<std::string> out;
    std::string cur;
    cur.reserve(32);

    auto flush = [&]() {
        if (!cur.empty()) {
            out.push_back(cur);
            cur.clear();
        }
    };

    for (unsigned char ch : text) {
        if (std::isalpha(ch)) {
            cur.push_back(static_cast<char>(std::tolower(ch)));
        } else {
            flush();
        }
    }
    flush();
    return out;
}