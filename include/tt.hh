// tt.hh
// Main file for the Transposition Table of WILI2.0

#pragma once
#include <cstdint>
#include "board.hh"
#include "types.hh"

namespace TT {

enum NodeType { EXACT, LOWERBOUND, UPPERBOUND };

struct Entry {
    uint64_t key;
    int depth;
    int score;
    types::Move bestMove;
    NodeType type;
};

/**
 * @brief It initializes the space for the Transposition Table, by default 64MB
 * Must be called before using the Transposition Table.
 * @param sizeMB 
 */
void init(size_t sizeMB = 64);

/**
 * @brief This function stores a key (zobristHash) with it's correspoding depth, score, bestMove found and type.
 * 
 * @param key 
 * @param depth 
 * @param score 
 * @param bestMove 
 * @param type 
 */
void store(uint64_t key, int depth, int score, types::Move bestMove, NodeType type);

/**c
 * @brief This function checks if there exists an entry to the tranposition table with the key
 * key. Returns true if and only if that is true. If the function returns true, it also set the reference 
 * variable outScore to be the score found in the entry with the key key.
 * 
 * @param key 
 * @param depth 
 * @param alpha 
 * @param beta 
 * @param outScore 
 * @return true 
 * @return false 
 */
bool probe(uint64_t key, int depth, int alpha, int beta, int& outScore);

} // namespace TT

