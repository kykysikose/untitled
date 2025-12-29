#pragma once
#include "inverted_index.h"

#include <string>
#include <vector>

struct RelativeIndex {
    size_t doc_id{};
    float rank{};

    bool operator==(const RelativeIndex& other) const {
        return doc_id == other.doc_id && rank == other.rank;
    }
};

class SearchServer {
public:
    explicit SearchServer(InvertedIndex& idx) : index_(idx) {}

    std::vector<std::vector<RelativeIndex>> search(const std::vector<std::string>& queries_input);

private:
    InvertedIndex& index_;
};