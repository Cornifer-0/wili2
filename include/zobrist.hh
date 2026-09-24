// zobrist.hh
// Main file for the zorbist keys in WILI2.0

#pragma once
#include <cstdint>
#include <random>
#include "types.hh"
#include "board.hh"

namespace Zobrist {
    extern uint64_t pieceKeys[2][6][64];
    extern uint64_t sideKey;
    extern uint64_t castlingKeys[16];
    extern uint64_t enPassantKeys[8];
} //namespace Zobrist