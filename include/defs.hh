// defs.hh
// Contains the constants warehouse of wili2
#pragma once

#include <string>
#include <cstdint>

using Bitboard = uint64_t;

const std::string STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

constexpr Bitboard SQUARE_BB[64] = {
  1ULL << 0,  1ULL << 1,  1ULL << 2,  1ULL << 3,  1ULL << 4,  1ULL << 5,  1ULL << 6,  1ULL << 7,
  1ULL << 8,  1ULL << 9,  1ULL << 10, 1ULL << 11, 1ULL << 12, 1ULL << 13, 1ULL << 14, 1ULL << 15,
  1ULL << 16, 1ULL << 17, 1ULL << 18, 1ULL << 19, 1ULL << 20, 1ULL << 21, 1ULL << 22, 1ULL << 23,
  1ULL << 24, 1ULL << 25, 1ULL << 26, 1ULL << 27, 1ULL << 28, 1ULL << 29, 1ULL << 30, 1ULL << 31,
  1ULL << 32, 1ULL << 33, 1ULL << 34, 1ULL << 35, 1ULL << 36, 1ULL << 37, 1ULL << 38, 1ULL << 39,
  1ULL << 40, 1ULL << 41, 1ULL << 42, 1ULL << 43, 1ULL << 44, 1ULL << 45, 1ULL << 46, 1ULL << 47,
  1ULL << 48, 1ULL << 49, 1ULL << 50, 1ULL << 51, 1ULL << 52, 1ULL << 53, 1ULL << 54, 1ULL << 55,
  1ULL << 56, 1ULL << 57, 1ULL << 58, 1ULL << 59, 1ULL << 60, 1ULL << 61, 1ULL << 62, 1ULL << 63
};

constexpr Bitboard RANK_1 = 0xFFULL;
constexpr Bitboard RANK_2 = RANK_1 << 8;
constexpr Bitboard RANK_3 = RANK_2 << 8;
constexpr Bitboard RANK_4 = RANK_3 << 8;
constexpr Bitboard RANK_5 = RANK_4 << 8;
constexpr Bitboard RANK_6 = RANK_5 << 8;
constexpr Bitboard RANK_7 = RANK_6 << 8;
constexpr Bitboard RANK_8 = RANK_7 << 8;

constexpr Bitboard FILE_A = 0x0101010101010101ULL;
constexpr Bitboard FILE_B = FILE_A << 7;
constexpr Bitboard FILE_C = FILE_B << 7;
constexpr Bitboard FILE_D = FILE_C << 7;
constexpr Bitboard FILE_E = FILE_D << 7;
constexpr Bitboard FILE_F = FILE_E << 7;
constexpr Bitboard FILE_G = FILE_F << 7;
constexpr Bitboard FILE_H = FILE_G << 7;


// Useful for castling rights stored in a single uint8_t
enum CastlingRightsEnum {
  WHITE_KINGSIDE = 1 << 0,
  WHITE_QUEENSIDE = 1 << 1,
  BLACK_KINGSIDE = 1 << 2,
  BLACK_QUEENSIDE = 1 << 3,
};

/**
 * @brief Returns the position of the least significant bit of bb. And turns it to 0.
 * If bb == 0. Returns -1.
 * 
 * @param bb 
 * @return int 
 */
inline int pop_lsb(Bitboard& bb) {
  if (bb == 0) {
    return -1; // Return -1 or handle the error case as needed
  }
  int lsb = __builtin_ctzll(bb); // Find the index of the least significant bit
  bb &= bb - 1;                 // Clear the least significant bit
  return lsb;
}


