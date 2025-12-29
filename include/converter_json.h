#pragma once
#include <nlohmann/json.hpp>

#include <string>
#include <utility>
#include <vector>

class ConverterJSON {
public:
    ConverterJSON(); // читает config.json и валидирует

    std::vector<std::string> GetTextDocuments() const;
    int GetResponsesLimit() const;
    std::vector<std::string> GetRequests() const;

    void putAnswers(const std::vector<std::vector<std::pair<int, float>>>& answers) const;

    std::string GetEngineName() const;
    std::string GetEngineVersion() const;

private:
    nlohmann::json config_;

    static nlohmann::json load_json_file(const std::string& path, const std::string& missing_error);
};