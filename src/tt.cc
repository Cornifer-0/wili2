//tt.cc

#include "../include/board.hh"
#include "../include/types.hh"
#include "../include/defs.hh"
// tt.cc
#include "tt.hh"
#include <vector>
#include <cstring>

namespace TT {

static std::vector<Entry> table;
static size_t tableSize = 0;

void init(size_t sizeMB) {
    tableSize = (sizeMB * 1024 * 1024) / sizeof(Entry);
    table.resize(tableSize);
    std::memset(table.data(), 0, tableSize * sizeof(Entry));
}

void store(uint64_t key, int depth, int score, types::Move bestMove, NodeType type) {
    size_t index = key % tableSize;
    table[index] = { key, depth, score, bestMove, type };
}

bool probe(uint64_t key, int depth, int alpha, int beta, int& outScore) {
    
    size_t index = key % tableSize;
    const Entry& entry = table[index];

    if (entry.key != key) return false;
    if (entry.depth < depth) return false;

    if (entry.type == EXACT) {
        outScore = entry.score;
        return true;
    } else if (entry.type == LOWERBOUND && entry.score >= beta) {
        outScore = entry.score;
        return true;
    } else if (entry.type == UPPERBOUND && entry.score <= alpha) {
        outScore = entry.score;
        return true;
    }

    return false;
}

} // TT namespace 
