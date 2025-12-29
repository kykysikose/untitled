#include "converter_json.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

static constexpr const char* APP_VERSION = "0.1";

nlohmann::json ConverterJSON::load_json_file(const std::string& path, const std::string& missing_error) {
    std::ifstream in(path);
    if (!in.is_open()) {
        throw std::runtime_error(missing_error);
    }
    nlohmann::json j;
    in >> j;
    return j;
}

ConverterJSON::ConverterJSON() {
    config_ = load_json_file("config.json", "config file is missing");

    if (!config_.contains("config") || config_["config"].is_null()) {
        throw std::runtime_error("config file is empty");
    }

    // version check (по ТЗ)
    if (config_["config"].contains("version")) {
        const std::string v = config_["config"]["version"].get<std::string>();
        if (v != APP_VERSION) {
            throw std::runtime_error("config.json has incorrect file version");
        }
    }
}

std::string ConverterJSON::GetEngineName() const {
    if (config_.contains("config") && config_["config"].contains("name")) {
        return config_["config"]["name"].get<std::string>();
    }
    return {};
}

std::string ConverterJSON::GetEngineVersion() const {
    if (config_.contains("config") && config_["config"].contains("version")) {
        return config_["config"]["version"].get<std::string>();
    }
    return {};
}

std::vector<std::string> ConverterJSON::GetTextDocuments() const {
    std::vector<std::string> docs;

    if (!config_.contains("files") || !config_["files"].is_array()) {
        return docs;
    }

    for (const auto& path_j : config_["files"]) {
        const std::string path = path_j.get<std::string>();
        std::ifstream f(path);
        if (!f.is_open()) {
            std::cerr << "File is missing: " << path << "\n";
            continue;
        }
        std::ostringstream ss;
        ss << f.rdbuf();
        docs.push_back(ss.str());
    }
    return docs;
}

int ConverterJSON::GetResponsesLimit() const {
    if (config_.contains("config") && config_["config"].contains("max_responses")) {
        return config_["config"]["max_responses"].get<int>();
    }
    return 5;
}

std::vector<std::string> ConverterJSON::GetRequests() const {
    auto req = load_json_file("requests.json", "requests file is missing");

    std::vector<std::string> out;
    if (!req.contains("requests") || !req["requests"].is_array()) return out;

    out.reserve(req["requests"].size());
    for (const auto& r : req["requests"]) {
        out.push_back(r.get<std::string>());
    }
    return out;
}

void ConverterJSON::putAnswers(const std::vector<std::vector<std::pair<int, float>>>& answers) const {
    nlohmann::json root;
    root["answers"] = nlohmann::json::object();

    for (size_t i = 0; i < answers.size(); ++i) {
        std::ostringstream key;
        key << "request" << std::setw(3) << std::setfill('0') << (i + 1);

        const auto& vec = answers[i];
        nlohmann::json node;

        if (vec.empty()) {
            node["result"] = "false";
        } else {
            node["result"] = "true";
            nlohmann::json rel = nlohmann::json::array();
            for (auto& [docid, rank] : vec) {
                rel.push_back({{"docid", docid}, {"rank", rank}});
            }
            node["relevance"] = std::move(rel);
        }

        root["answers"][key.str()] = std::move(node);
    }

    std::ofstream out("answers.json", std::ios::trunc);
    out << root.dump(2);
}