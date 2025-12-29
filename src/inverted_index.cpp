#include "inverted_index.h"
#include "text_utils.h"

#include <algorithm>
#include <thread>
#include <unordered_map>

void InvertedIndex::UpdateDocumentBase(std::vector<std::string> input_docs) {
    docs_ = std::move(input_docs);
    freq_dictionary_.clear();

    const size_t n = docs_.size();
    std::vector<std::unordered_map<std::string, size_t>> local_counts(n);
    std::vector<std::thread> threads;
    threads.reserve(n);

    for (size_t doc_id = 0; doc_id < n; ++doc_id) {
        threads.emplace_back([this, doc_id, &local_counts]() {
            auto words = split_words(docs_[doc_id]);
            auto& m = local_counts[doc_id];
            for (auto& w : words) {
                ++m[w];
            }
        });
    }

    for (auto& t : threads) t.join();

    // Слияние строго в порядке doc_id -> детерминированный порядок Entry
    for (size_t doc_id = 0; doc_id < n; ++doc_id) {
        for (auto& [word, cnt] : local_counts[doc_id]) {
            freq_dictionary_[word].push_back(Entry{doc_id, cnt});
        }
    }

    // На всякий случай отсортируем списки по doc_id (если захотите менять схему слияния)
    for (auto& [word, vec] : freq_dictionary_) {
        std::sort(vec.begin(), vec.end(), [](const Entry& a, const Entry& b) {
            return a.doc_id < b.doc_id;
        });
    }
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string& word) const {
    auto it = freq_dictionary_.find(word);
    if (it == freq_dictionary_.end()) return {};
    return it->second;
}