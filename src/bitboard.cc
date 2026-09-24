//Bitboard.cc
//For WILI2.0, bitwise operations implemetation

#include "../include/bitboard.hh"
#include "../include/types.hh"
#include "../include/defs.hh"
#include <cassert>
#include <cstdint>
#include <initializer_list>
#include <cmath>

namespace Bitboards {

types::Bitboard FileMasks[8];
types::Bitboard RankMasks[8];
types::Bitboard DiagonalMasks[64];
types::Bitboard AntiDiagonalMasks[64];
types::Bitboard KnightMoves[64];
types::Bitboard KingMoves[64];
types::Bitboard PawnAttacks[2][64];

constexpr types::Bitboard ONE = 1ULL;


// -----------------------------
// Helper for square coordinates
inline int rank_of(types::Square sq) { return sq / 8; }
inline int file_of(types::Square sq) { return sq % 8; }

//-----------------------------
// Precompute directional attacks

void init(){
    for(int r = 0; r < 8; ++r){
        for(int f = 0; f < 8; ++f){
            int sq = r * 8 + f;

            //rank and file masks
            RankMasks[r] |= ONE << sq;
            FileMasks[f] |= ONE << sq;
            
            //diagonals 
            DiagonalMasks[sq] = 0ULL;
            AntiDiagonalMasks[sq] = 0ULL;
            for(int dr = -7; dr <= 7; ++dr){
                int tr = r + dr;
                int tf = f + dr;
                if( tr >= 0 && tr < 8 && tf >= 0 && tf < 8 ){
                    DiagonalMasks[sq] |= ONE << (tr * 8 + tf);  
                }
                tf = f - dr;
                if(tr >= 0 && tr < 8 && tf >= 0 && tf < 8){
                    AntiDiagonalMasks[sq] |= ONE << (tr * 8 + tf);
                }
            }

            //Knight moves
            KnightMoves[sq] = 0ULL;
            for(int dx : {-2, -1, 1, 2}){
                for(int dy : {-2, -1, 1, 2}){
                    if(abs(dx) != abs(dy)){
                        int tr = r + dy;
                        int tf = f + dx;
                        if(tr >= 0 && tr < 8 && tf >= 0 && tf < 8){
                            KnightMoves[sq] |= ONE << (tr * 8 + tf);
                        }
                    }
                }
            }

            //King moves
            KingMoves[sq] = 0ULL;
            for(int dr = -1; dr <= 1; ++dr){
                for(int df = -1; df <= 1 ; ++df){
                    if(dr == 0 && df == 0) continue;
                    int tr = r + dr;
                    int tf = f + df;
                    if(tr >= 0 && tr < 8 && tf >= 0 && tf < 8){
                        KingMoves[sq] |= ONE << (tr * 8 + tf);
                    }
                }
            }

            // if(sq == types::E1) Bitboards::printBitboard(KingMoves[types::E1]);
            // if(sq == types::F1) Bitboards::printBitboard(KingMoves[types::F1]);


            //Pawn attacks
            PawnAttacks[types::WHITE][sq] = 0ULL;
            PawnAttacks[types::BLACK][sq] = 0ULL;

            if( r < 7 ){
                if(f > 0) PawnAttacks[types::WHITE][sq] |= ONE << ((r - 1) * 8 + (f - 1));
                if(f < 7) PawnAttacks[types::WHITE][sq] |= ONE << ((r - 1) * 8 + (f + 1));
            }
            if(r > 0){
                if(f > 0) PawnAttacks[types::BLACK][sq] |= ONE << ((r + 1) * 8 + (f - 1));
                if(f < 7) PawnAttacks[types::BLACK][sq] |= ONE << ((r + 1) * 8 + (f + 1));
            }

        }
    }
}

int popcount(types::Bitboard b) {
    return __builtin_popcountll(b);
}

int lsb(types::Bitboard b) {
    if(b == 0) return 0;
    //assert(b != 0);
    return __builtin_ctzll(b);
}

int msb(types::Bitboard b){
    assert(b == 0);
    return 63 - __builtin_clzll(b);
}

// -----------------------------
// Returns a bitboard with bits set between two squares (exclusive)
types::Bitboard mask_between(types::Square a, types::Square b) {
    Bitboard btwn = 0ULL;

    int ra = rank_of(a), fa = file_of(a);
    int rb = rank_of(b), fb = file_of(b);

    int dr = (rb > ra) ? 1 : (rb < ra) ? -1 : 0;
    int df = (fb > fa) ? 1 : (fb < fa) ? -1 : 0;

    if (dr == 0 && df == 0)
        return 0;

    int r = ra + dr, f = fa + df;
    while (r != rb || f != fb) {
        btwn |= ONE << (r * 8 + f);
        r += dr;
        f += df;
    }

    return btwn ^ (ONE << a) ^ (ONE << b);
}

void printBitboard(types::Bitboard bb) {
    std::cout << "\nBitboard:\n\n";
    for (int rank = 0; rank <= 7; ++rank) {
        std::cout << 8 - rank << "  ";
        for (int file = 0; file < 8; ++file) {
            int square = rank * 8 + file;
            std::cout << ((bb >> square) & 1ULL ? "1 " : ". ");
        }
        std::cout << std::endl;
    }
    std::cout << "\n   a b c d e f g h\n\n";
}





} // namespace Bitboards
