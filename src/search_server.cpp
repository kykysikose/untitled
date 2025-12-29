#include "search_server.h"
#include "text_utils.h"

#include <algorithm>
#include <unordered_map>

static size_t total_word_frequency(const InvertedIndex& idx, const std::string& word) {
    size_t sum = 0;
    for (const auto& e : idx.GetWordCount(word)) sum += e.count;
    return sum;
}

std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string>& queries_input) {
    std::vector<std::vector<RelativeIndex>> all_results;
    all_results.reserve(queries_input.size());

    for (const auto& query : queries_input) {
        auto words = split_words(query);
        if (words.empty()) {
            all_results.push_back({});
            continue;
        }

        std::sort(words.begin(), words.end());
        words.erase(std::unique(words.begin(), words.end()), words.end());

        // Если хоть одного слова нет в индексе — результатов нет
        bool missing = false;
        for (auto& w : words) {
            if (index_.GetWordCount(w).empty()) { missing = true; break; }
        }
        if (missing) {
            all_results.push_back({});
            continue;
        }

        // Сортировка по редкости (суммарной частоте по всем документам)
        std::sort(words.begin(), words.end(), [&](const std::string& a, const std::string& b) {
            const auto fa = total_word_frequency(index_, a);
            const auto fb = total_word_frequency(index_, b);
            if (fa != fb) return fa < fb;
            return a < b;
        });

        // Кандидаты по самому редкому слову
        std::unordered_map<size_t, size_t> abs_rel; // doc_id -> absolute relevance
        for (const auto& e : index_.GetWordCount(words[0])) {
            abs_rel[e.doc_id] = e.count;
        }

        // Пересечение по остальным словам + накопление абсолютной релевантности
        for (size_t i = 1; i < words.size() && !abs_rel.empty(); ++i) {
            std::unordered_map<size_t, size_t> doc_to_count;
            for (const auto& e : index_.GetWordCount(words[i])) {
                doc_to_count[e.doc_id] = e.count;
            }

            for (auto it = abs_rel.begin(); it != abs_rel.end(); ) {
                auto f = doc_to_count.find(it->first);
                if (f == doc_to_count.end()) {
                    it = abs_rel.erase(it);
                } else {
                    it->second += f->second;
                    ++it;
                }
            }
        }

        if (abs_rel.empty()) {
            all_results.push_back({});
            continue;
        }

        size_t max_abs = 0;
        for (auto& [doc_id, a] : abs_rel) max_abs = std::max(max_abs, a);

        std::vector<RelativeIndex> result;
        result.reserve(abs_rel.size());
        for (auto& [doc_id, a] : abs_rel) {
            float rank = static_cast<float>(a) / static_cast<float>(max_abs);
            result.push_back(RelativeIndex{doc_id, rank});
        }

        std::sort(result.begin(), result.end(), [](const RelativeIndex& x, const RelativeIndex& y) {
            if (x.rank != y.rank) return x.rank > y.rank;
            return x.doc_id < y.doc_id;
        });

        all_results.push_back(std::move(result));
    }

    return all_results;
}