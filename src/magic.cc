// magic.cc
#include "magic.hh"      // Contains declarations for initMagicTables and getBishopAttacks/getRookAttacks.
#include "magic_data.hh" // Contains precomputed magic arrays: RookMagics and BishopMagics.
#include "bitboard.hh"   // If you keep additional bitboard helpers (pop_lsb, etc.)
#include <cassert>
#include <iostream>
#include <cstdint>
#include <bitset>

// We assume that types::Bitboard is defined as a 64-bit unsigned integer, e.g.:
// using Bitboard = uint64_t;

namespace Magic {

types::Bitboard rookOccupancyMasks[NUM_SQUARES];
types::Bitboard rookMagics[NUM_SQUARES];
int rookShifts[NUM_SQUARES];
types::Bitboard* rookAttackTable[NUM_SQUARES];
    
types::Bitboard bishopOccupancyMasks[NUM_SQUARES];
types::Bitboard bishopMagics[NUM_SQUARES];
int bishopShifts[NUM_SQUARES];
types::Bitboard* bishopAttackTable[NUM_SQUARES];


// Helper: Given a square (0-63), return its rank (0-7) and file (0-7).
static inline int rankOf(int square) {
    return square / 8;
}
static inline int fileOf(int square) {
    return square % 8;
}

types::Bitboard getRookAttacks(int square, types::Bitboard occupancy) {
    // std::cout << "=== Rook Attack Query ===" << std::endl;
    // std::cout << "Square: " << square << std::endl;
    // std::cout << "Original occupancy: " << std::endl;
    // Bitboards::printBitboard(occupancy);

    occupancy &= rookOccupancyMasks[square];

    // std::cout << "Masked occupancy: ";
    // Bitboards::printBitboard(occupancy);
    // Bitboards::printBitboard(rookMagics[square]);
    // Bitboards::printBitboard(rookShifts[square]);
    int index = (int)((occupancy * rookMagics[square]) >> rookShifts[square]);
    // std::cout << "Magic index: " << index << std::endl;
    // Bitboards::printBitboard(rookAttackTable[square][index]);
    return rookAttackTable[square][index];
}

types::Bitboard getBishopAttacks(int square, types::Bitboard occupancy) {
    occupancy &= bishopOccupancyMasks[square];
    int index = (int)((occupancy * bishopMagics[square]) >> bishopShifts[square]);
    return bishopAttackTable[square][index];
}

// ----------------------------------------------------------------
// Compute Rook Occupancy Mask
// For a given square, the occupancy mask includes all squares along its rank and file
// EXCLUDING the edge squares. (This is standard for magic bitboards.)
types::Bitboard computeRookMask(int square) {
    types::Bitboard mask = 0ULL;
    int r = rankOf(square);
    int f = fileOf(square);

    // North: start at one square above and go until rank 6 (stop before edge rank 7)
    for (int i = r + 1; i <= 6; i++) {
        mask |= (1ULL << (i * 8 + f));
    }
    // South: from one square below until rank 1 (stop before edge rank 0)
    for (int i = r - 1; i >= 1; i--) {
        mask |= (1ULL << (i * 8 + f));
    }
    // East: from one square to right until file 6 (stop before edge file 7)
    for (int i = f + 1; i <= 6; i++) {
        mask |= (1ULL << (r * 8 + i));
    }
    // West: from one square to left until file 1 (stop before edge file 0)
    for (int i = f - 1; i >= 1; i--) {
        mask |= (1ULL << (r * 8 + i));
    }
    return mask;
}

// ----------------------------------------------------------------
// Compute Bishop Occupancy Mask
// For a given square, the occupancy mask includes all squares along its diagonals
// EXCLUDING the edge squares.
types::Bitboard computeBishopMask(int square) {
    types::Bitboard mask = 0ULL;
    int r = rankOf(square);
    int f = fileOf(square);

    // Northeast: move up and right until just before the edge.
    for (int i = r + 1, j = f + 1; i <= 6 && j <= 6; i++, j++) {
        mask |= (1ULL << (i * 8 + j));
    }
    // Northwest: move up and left.
    for (int i = r + 1, j = f - 1; i <= 6 && j >= 1; i++, j--) {
        mask |= (1ULL << (i * 8 + j));
    }
    // Southeast: move down and right.
    for (int i = r - 1, j = f + 1; i >= 1 && j <= 6; i--, j++) {
        mask |= (1ULL << (i * 8 + j));
    }
    // Southwest: move down and left.
    for (int i = r - 1, j = f - 1; i >= 1 && j >= 1; i--, j--) {
        mask |= (1ULL << (i * 8 + j));
    }
    return mask;
}

// ----------------------------------------------------------------
// setOccupancy
// Given an index (from 0 to 2^(bitsInMask)-1) and an occupancy mask, this function
// maps the index bits to the corresponding squares in the mask (in order of increasing significance).
// For each bit set in the mask, if the corresponding bit in the index is 1, set that square.
types::Bitboard setOccupancy(int index, int bitsInMask, types::Bitboard mask) {
    types::Bitboard occupancy = 0ULL;
    // For each bit position in the occupancy mask:
    for (int count = 0; count < bitsInMask; count++) {
        // Isolate the least significant bit from mask
        int square = __builtin_ctzll(mask);
        // Remove that bit from mask
        mask &= mask - 1;
        // If the corresponding bit in index is set, add that square to occupancy.
        if (index & (1 << count))
            occupancy |= (1ULL << square);
    }
    // std::cout << "setoccupancy : ->>";
    // Bitboards::printBitboard(occupancy);

    return occupancy;
}

// ----------------------------------------------------------------
// Compute Rook Attack for a Square Under a Given Occupancy
// This function simulates sliding moves in each cardinal direction until a blocker or board edge is hit.
types::Bitboard computeRookAttackForSquare(int square, types::Bitboard occupancy) {
    types::Bitboard attacks = 0ULL;
    int r = rankOf(square);
    int f = fileOf(square);
    int r_temp, f_temp;

     // <- here 
    // if(square == 35){
    //     std::cout << "occupancy while calculating rook attacks :" << std::endl;
    //     Bitboards::printBitboard(occupancy);
    // }
    // North
    for (r_temp = r + 1; r_temp <= 7; r_temp++) {
        int sq = r_temp * 8 + f;
        attacks |= (1ULL << sq);
        if (occupancy & (1ULL << sq))
            break;
    }
    // South
    for (r_temp = r - 1; r_temp >= 0; r_temp--) {
        int sq = r_temp * 8 + f;
        attacks |= (1ULL << sq);
        if (occupancy & (1ULL << sq))
            break;
    }
    // East
    for (f_temp = f + 1; f_temp <= 7; f_temp++) {
        int sq = r * 8 + f_temp;
        attacks |= (1ULL << sq);
        if (occupancy & (1ULL << sq))
            break;
    }
    // West
    for (f_temp = f - 1; f_temp >= 0; f_temp--) {
        int sq = r * 8 + f_temp;
        attacks |= (1ULL << sq);
        if (occupancy & (1ULL << sq))
            break;
    }
    return attacks;
}

// ----------------------------------------------------------------
// Compute Bishop Attack for a Square Under a Given Occupancy
// This function simulates sliding moves diagonally in all four directions until blocked.
types::Bitboard computeBishopAttackForSquare(int square, types::Bitboard occupancy) {
    types::Bitboard attacks = 0ULL;
    int r = rankOf(square);
    int f = fileOf(square);
    int r_temp, f_temp;

    // Northeast
    for (r_temp = r + 1, f_temp = f + 1; r_temp <= 7 && f_temp <= 7; r_temp++, f_temp++) {
        int sq = r_temp * 8 + f_temp;
        attacks |= (1ULL << sq);
        if (occupancy & (1ULL << sq))
            break;
    }
    // Northwest
    for (r_temp = r + 1, f_temp = f - 1; r_temp <= 7 && f_temp >= 0; r_temp++, f_temp--) {
        int sq = r_temp * 8 + f_temp;
        attacks |= (1ULL << sq);
        if (occupancy & (1ULL << sq))
            break;
    }
    // Southeast
    for (r_temp = r - 1, f_temp = f + 1; r_temp >= 0 && f_temp <= 7; r_temp--, f_temp++) {
        int sq = r_temp * 8 + f_temp;
        attacks |= (1ULL << sq);
        if (occupancy & (1ULL << sq))
            break;
    }
    // Southwest
    for (r_temp = r - 1, f_temp = f - 1; r_temp >= 0 && f_temp >= 0; r_temp--, f_temp--) {
        int sq = r_temp * 8 + f_temp;
        attacks |= (1ULL << sq);
        if (occupancy & (1ULL << sq))
            break;
    }
    return attacks;
}

// ----------------------------------------------------------------
// The initialization function for magic tables.
// This function fills in the occupancy masks, uses precomputed magic numbers, computes the shift values,
// allocates the attack table arrays, and populates the tables for every square.
void initMagicTables() {
    for (int square = 0; square < NUM_SQUARES; ++square) {
        // ---- Rook Initialization ----
        rookOccupancyMasks[square] = computeRookMask(square);
        int rookRelevantBits = __builtin_popcountll(rookOccupancyMasks[square]);
        rookShifts[square] = 64 - rookRelevantBits;
        // Use precomputed magic (from magic_data.hh)
        rookMagics[square] = RookMagics[square];
        int rookTableSize = 1 << rookRelevantBits;
        rookAttackTable[square] = new types::Bitboard[rookTableSize]();

        // Loop over all possible blocker configurations for the mask.
        for (types::Bitboard index = 0; index < (1ULL << rookRelevantBits); ++index) {
            types::Bitboard occupancy = setOccupancy(index, rookRelevantBits, rookOccupancyMasks[square]);
            types::Bitboard attack = computeRookAttackForSquare(square, occupancy);
            int magicIndex = (int)((occupancy * rookMagics[square]) >> rookShifts[square]);
            // if (square == 35) {
            //     std::cerr << "Collision detected at square " << square << " with index " << magicIndex << std::endl;
            //     std::cerr << "Existing attack: " << std::endl;
            //     Bitboards::printBitboard(rookAttackTable[square][magicIndex]);
            //     std::cerr << "New attack     : " << std::endl;
            //     Bitboards::printBitboard(attack);
            // }
            rookAttackTable[square][magicIndex] = attack;
        }

        // ---- Bishop Initialization ----
        bishopOccupancyMasks[square] = computeBishopMask(square);
        int bishopRelevantBits = __builtin_popcountll(bishopOccupancyMasks[square]);
        bishopShifts[square] = 64 - bishopRelevantBits;
        bishopMagics[square] = BishopMagics[square];
        int bishopTableSize = 1 << bishopRelevantBits;
        bishopAttackTable[square] = new types::Bitboard[bishopTableSize];

        for (types::Bitboard index = 0; index < (1ULL << bishopRelevantBits); ++index) {
            types::Bitboard occupancy = setOccupancy(index, bishopRelevantBits, bishopOccupancyMasks[square]);
            types::Bitboard attack = computeBishopAttackForSquare(square, occupancy);
            int magicIndex = (int)((occupancy * bishopMagics[square]) >> bishopShifts[square]);
            bishopAttackTable[square][magicIndex] = attack;
        }
    }
}

} // namespace Magic
