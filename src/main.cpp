#include "converter_json.h"
#include "inverted_index.h"
#include "search_server.h"

#include <iostream>
#include <utility>
#include <vector>

int main() {
    try {
        ConverterJSON conv;

        std::cout << "Starting " << conv.GetEngineName() << "\n";

        auto docs = conv.GetTextDocuments();

        InvertedIndex idx;
        idx.UpdateDocumentBase(std::move(docs));

        SearchServer server(idx);
        auto requests = conv.GetRequests();
        auto results = server.search(requests);

        const int limit = conv.GetResponsesLimit();

        // Приводим к формату putAnswers: vector<vector<pair<int,float>>>
        std::vector<std::vector<std::pair<int, float>>> out;
        out.reserve(results.size());

        for (auto& one_query : results) {
            std::vector<std::pair<int, float>> v;
            const size_t cap = (limit > 0) ? static_cast<size_t>(limit) : one_query.size();
            for (size_t i = 0; i < one_query.size() && i < cap; ++i) {
                v.push_back({static_cast<int>(one_query[i].doc_id), one_query[i].rank});
            }
            out.push_back(std::move(v));
        }

        conv.putAnswers(out);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
}