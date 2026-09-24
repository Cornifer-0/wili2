#pragma once
#include "types.hh"

namespace Magic {
    // Number of squares on board
    constexpr int NUM_SQUARES = 64;
    
    // For rooks:
    extern types::Bitboard rookOccupancyMasks[NUM_SQUARES];
    extern types::Bitboard rookMagics[NUM_SQUARES];
    extern int rookShifts[NUM_SQUARES];
    extern types::Bitboard* rookAttackTable[NUM_SQUARES]; // pointer per square to an array of bitboards

    // For bishops:
    extern types::Bitboard bishopOccupancyMasks[NUM_SQUARES];
    extern types::Bitboard bishopMagics[NUM_SQUARES];
    extern int bishopShifts[NUM_SQUARES];
    extern types::Bitboard* bishopAttackTable[NUM_SQUARES];

    /**
     * @brief Calculates all magic tables before move generation.
     * This must be called at the beggining of the execution.
     */
    void initMagicTables();
    
    // Functions that use the magic: given a square and current occupancy, return sliding attacks.
    
    /**
     * @brief Given a square and current occupancy returns the rook attacks.
     * This doesn't not take into account friendly Ocupancy.
     * @param square 
     * @param occupancy 
     * @return types::Bitboard 
     */
    types::Bitboard getRookAttacks(int square, types::Bitboard occupancy);

    /**
     * @brief Given a square and current occupancy returns the bishop attacks.
     * This doesn't not take into account friendly Ocupancy.
     * @param square 
     * @param occupancy 
     * @return types::Bitboard 
     */
    types::Bitboard getBishopAttacks(int square, types::Bitboard occupancy);
}
