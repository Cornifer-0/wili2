// bitboard.hh
// for WILI2.0, bitwise operations file

/**
 * @file bitboard.hh
 */

#pragma once
#include "types.hh"
#include <cstdint>

using Bitboard = uint64_t;

namespace Bitboards{


extern Bitboard FileMasks[8];
extern Bitboard RankMasks[8];
extern Bitboard DiagonalMasks[64];
extern Bitboard AntiDiagonalMasks[64];
extern Bitboard KnightMoves[64];
extern Bitboard KingMoves[64];
extern Bitboard PawnAttacks[2][64]; // [Color][Square]

/**
 * @brief Initializes the bitboards : FilesMasks, RankMasks, DiagonalMasks
 * AntiDiagonalMasks KnightMoves, KingMoves and PawnAttacks.
 * Called once at the start of the main program.
 */
void init(); //call once at start

/**
 * @brief Returns the number of active bits in the bitboard b
 * @param b 
 * @return int 
 */
int popcount(Bitboard b);

/**
 * @brief Returns the position of the least significant active bit in the bitboard b
 * @param b 
 * @return int 
 */
int lsb(Bitboard b);

/**
 * @brief Returns the position of the most siginificant active bit in the bitboard b 
 * 
 * @param b 
 * @return int 
 */
int msb(Bitboard b);

/**
 * @brief Returns a mask bitboard from Square a to Square b.
 * @param a 
 * @param b 
 * @return Bitboard 
 */
Bitboard mask_between(types::Square a, types::Square b);

/**
 * @brief Prints a bitboard through the console. With the coordinates.
 * @param bb 
 */
void printBitboard(types::Bitboard bb);

}; //namespace Bitboards
