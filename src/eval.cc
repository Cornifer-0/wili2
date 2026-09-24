#include "../include/board.hh"
#include "../include/types.hh"
#include "../include/bitboard.hh"
#include "../include/eval.hh"
#include "../include/PST_data.hh"
#include "../include/bitboard.hh"
#include "../include/magic.hh"

#include <iostream>

using namespace types;
using namespace Bitboards;

namespace eval{

int evaluate(const Board& board) {
    int middleGameScore = 0;
    int endGameScore = 0;

    middleGameScore += evalMaterial(board, MIDGAME);
    endGameScore += evalMaterial(board, ENDGAME);

    middleGameScore += evalPieceSquareTables(board, MIDGAME) / 1;
    endGameScore += evalPieceSquareTables(board, ENDGAME) / 1;

    middleGameScore += evalMobility(board, MIDGAME) / 1;
    endGameScore += evalMobility(board, ENDGAME) / 1;

    middleGameScore += evalKingSafety(board) / 1;
    middleGameScore += evalPawnStructure(board) / 1;
    middleGameScore += evalPassedPawns(board) / 1;

    int phase = computeGamePhase(board);
    int score = (
        middleGameScore * phase + endGameScore * (24 - phase)
    ) / 24;

    return score;
}


int evalMaterial(const Board &board, Phase phase) {
    // Simple material evaluation
    int score = 0;

    const int pawnVal   = (phase == MIDGAME) ? 100 : 120;
    const int knightVal = (phase == MIDGAME) ? 320 : 300;
    const int bishopVal = (phase == MIDGAME) ? 330 : 320;
    const int rookVal   = (phase == MIDGAME) ? 500 : 510;
    const int queenVal  = 900;

    score += pawnVal * popcount(board.getBitboard(PAWN, WHITE));
    score += knightVal * popcount(board.getBitboard(KNIGHT, WHITE));
    score += bishopVal * popcount(board.getBitboard(BISHOP, WHITE));
    score += rookVal * popcount(board.getBitboard(ROOK, WHITE));
    score += queenVal* popcount(board.getBitboard(QUEEN, WHITE));
    
    score -= pawnVal * popcount(board.getBitboard(PAWN, BLACK));
    score -= knightVal * popcount(board.getBitboard(KNIGHT, BLACK));
    score -= bishopVal * popcount(board.getBitboard(BISHOP, BLACK));
    score -= rookVal * popcount(board.getBitboard(ROOK, BLACK));
    score -= queenVal * popcount(board.getBitboard(QUEEN, BLACK));


    return score;
}

int evalMobility(const Board& board, Phase phase) {
    int score = 0;

    Color us = board.getSideToMove();
    Color them = !us;

    Bitboard friendly = board.getOcupancy(us);
    Bitboard themFriendly = board.getOcupancy(them);
    Bitboard occupied = board.getOcupancy();

    // Knight mobility
    Bitboard knights = board.getBitboard(KNIGHT, us);
    while (knights) {
        int sq = pop_lsb(knights);
        Bitboard moves = Bitboards::KnightMoves[sq] & ~friendly;
        int count = popcount(moves);
        score += (phase == MIDGAME ? 4 : 2) * count;
    }

    // Bishop mobility (magic bitboards or attack tables)
    Bitboard bishops = board.getBitboard(BISHOP, us);
    while (bishops) {
        int sq = pop_lsb(bishops);
        Bitboard attacks = Magic::getBishopAttacks(sq, occupied) & ~friendly;
        int count = popcount(attacks);
        score += (phase == MIDGAME ? 5 : 3) * count;
    }

    // Rook mobility
    Bitboard rooks = board.getBitboard(ROOK, us);
    while (rooks) {
        int sq = pop_lsb(rooks);
        Bitboard attacks = Magic::getRookAttacks(sq, occupied) & ~friendly;
        int count = popcount(attacks);
        score += (phase == MIDGAME ? 2 : 4) * count;
    }

    // Queen mobility
    Bitboard queens = board.getBitboard(QUEEN, us);
    while (queens) {
        int sq = pop_lsb(queens);
        Bitboard attacks = (Magic::getRookAttacks(sq, occupied) |
                            Magic::getBishopAttacks(sq, occupied)) & ~friendly;
        int count = popcount(attacks);
        score += (phase == MIDGAME ? 0 : 2) * count;
    }

    //THEM======
    // Knight mobility
    knights = board.getBitboard(KNIGHT, them);
    while (knights) {
        int sq = pop_lsb(knights);
        Bitboard moves = Bitboards::KnightMoves[sq] & ~themFriendly;
        int count = popcount(moves);
        score -= (phase == MIDGAME ? 4 : 2) * count;
    }

    // Bishop mobility (magic bitboards or attack tables)
    bishops = board.getBitboard(BISHOP, them);
    while (bishops) {
        int sq = pop_lsb(bishops);
        Bitboard attacks = Magic::getBishopAttacks(sq, occupied) & ~themFriendly;
        int count = popcount(attacks);
        score -= (phase == MIDGAME ? 5 : 3) * count;
    }

    // Rook mobility
    rooks = board.getBitboard(ROOK, them);
    while (rooks) {
        int sq = pop_lsb(rooks);
        Bitboard attacks = Magic::getRookAttacks(sq, occupied) & ~themFriendly;
        int count = popcount(attacks);
        score -= (phase == MIDGAME ? 2 : 4) * count;
    }

    // Queen mobility
    queens = board.getBitboard(QUEEN, them);
    while (queens) {
        int sq = pop_lsb(queens);
        Bitboard attacks = (Magic::getRookAttacks(sq, occupied) |
                            Magic::getBishopAttacks(sq, occupied)) & ~themFriendly;
        int count = popcount(attacks);
        score -= (phase == MIDGAME ? 0 : 2) * count;
    }
    //std::cout << "Eval score: " << score << std::endl;

    return score;
}


int evalPieceSquareTables(const Board& board, Phase phase) {
    int score = 0;
    for (Color col = WHITE; col <= BLACK; col = static_cast<Color>(col + 1)){
        for (PieceType piece = PAWN; piece <= KING; piece = static_cast<PieceType>(piece + 1)) {
            Bitboard bitboard = board.getBitboard(piece, col);
            while (bitboard) {
                Square sq = Square(pop_lsb(bitboard));
                score += getPSTValue(piece, sq, col, phase);
            }
        }
    }
    //std::cout << "PST score: " << score << std::endl;

    return score;
}

int getPSTValue(PieceType pt, Square sq, Color col, Phase phase) {
    // Mirror the square for Blackj
    //int sq = (col == WHITE) ? sq : ((7 - (sq / 8)) * 8 + (sq % 8));
    int index = (col == WHITE) ? sq : ((7 - (sq / 8)) * 8 + (sq % 8));
    int score;

    if (phase == MIDGAME) {
        switch (pt) {
            case PAWN:   score = pawn_table_midgame[index]; break;
            case KNIGHT: score = knight_table_midgame[index]; break;
            case BISHOP: score = bishop_table_midgame[index]; break;
            case ROOK:   score = rook_table_midgame[index]; break;
            case QUEEN:  score = queen_table_midgame[index]; break;
            case KING:   score = king_table_midgame[index]; break;
            default:     score = 0; break;
        }
    } else { // ENDGAME
        switch (pt) {
            case PAWN:   score = pawn_table_endgame[index]; break;
            case KNIGHT: score = knight_table_endgame[index]; break;
            case BISHOP: score = bishop_table_endgame[index]; break;
            case ROOK:   score = rook_table_endgame[index]; break;
            case QUEEN:  score = queen_table_endgame[index]; break;
            case KING:   score = king_table_endgame[index]; break;
            default:     score = 0; break;
        }
    }

    if(col == WHITE) return score;
    return -score;
}

int evalKingSafety(const Board& board) {
    int score = 0;

    for (Color color : {WHITE, BLACK}) {
        Square kingSq = Square(lsb(board.getBitboard(KING, color)));
        int kingRank = kingSq / 8;
        int kingFile = kingSq % 8;

        // Penalize being uncastled in the center (E1/E8, D1/D8)
        if ((color == WHITE && kingRank == 0) || (color == BLACK && kingRank == 7)) {
            if (kingFile >= 3 && kingFile <= 5) {
                score += (color == WHITE ? -25 : 25); // Penalize exposed kings
            }
        }

        // Bonus for castled king with pawn shield
        int pawnShieldScore = 0;
        int forward = (color == WHITE ? 1 : -1);
        int baseRank = kingRank + forward;

        for (int df = -1; df <= 1; ++df) {
            int f = kingFile + df;
            if (f >= 0 && f < 8 && baseRank >= 0 && baseRank < 8) {
                Square sq = static_cast<Square>(baseRank * 8 + f);
                if (board.getBitboard(PAWN, color) & SQUARE_BB[sq]) {
                    pawnShieldScore += 10;  // Each protecting pawn worth 10
                }
            }
        }

        if (color == WHITE)
            score += pawnShieldScore;
        else
            score -= pawnShieldScore;
    }
    //std::cout << "King Safety score: " << score << std::endl;
    return score;
}

int evalPawnStructure(const Board& board) {
    int score = 0;

    for (Color color : {WHITE, BLACK}) {
        Bitboard pawns = board.getBitboard(PAWN, color);
        Bitboard opponentPawns = board.getBitboard(PAWN, !color);

        while (pawns) {
            int sq = pop_lsb(pawns);
            int file = sq % 8;
            int rank = sq / 8;
            //int dir = (color == WHITE) ? 8 : -8;

            // Doubled pawns
            Bitboard fileMask = FileMasks[file];
            int count = popcount(board.getBitboard(PAWN, color) & fileMask);
            if (count > 1)
                score += (color == WHITE ? -10 : 10);

            // Isolated pawn
            Bitboard adjFiles = 0;
            if (file > 0) adjFiles |= FileMasks[file - 1];
            if (file < 7) adjFiles |= FileMasks[file + 1];
            if ((board.getBitboard(PAWN, color) & adjFiles) == 0)
                score += (color == WHITE ? -15 : 15);

            // Passed pawn
            Bitboard forward = 0;
            for (int r = rank + (color == WHITE ? 1 : -1); r >= 0 && r <= 7; r += (color == WHITE ? 1 : -1)) {
                for (int f = std::max(0, file - 1); f <= std::min(7, file + 1); ++f) {
                    forward |= SQUARE_BB[r * 8 + f];
                }
            }
            if ((opponentPawns & forward) == 0)
                score += (color == WHITE ? 30 : -30);
        }
    }

    //std::cout << "Pawn Structure score: " << score << std::endl;

    return score;
}

int evalPassedPawns(const Board& board) {
    int score = 0;

    for (Color color : {WHITE, BLACK}) {
        Bitboard pawns = board.getBitboard(PAWN, color);
        Bitboard opponentPawns = board.getBitboard(PAWN, !color);
        Bitboard friendPawns = pawns;

        while (pawns) {
            int sq = pop_lsb(pawns);
            int file = sq % 8;
            int rank = sq / 8;

            // Check if passed
            Bitboard forward = 0;
            for (int r = rank + (color == WHITE ? 1 : -1);
                 r >= 0 && r <= 7;
                 r += (color == WHITE ? 1 : -1)) {
                for (int f = std::max(0, file - 1); f <= std::min(7, file + 1); ++f) {
                    forward |= SQUARE_BB[r * 8 + f];
                }
            }
            if ((opponentPawns & forward) != 0)
                continue; // Not a passed pawn

            // Passed pawn detected

            // Advance bonus
            int r = (color == WHITE) ? rank : 7 - rank;
            int bonus = 10 + 10 * r;  // increases from 10 to 70
            score += (color == WHITE ? bonus : -bonus);

            // Supported passed pawn
            int supportRank = (color == WHITE) ? rank - 1 : rank + 1;
            if (supportRank >= 0 && supportRank <= 7) {
                for (int f = std::max(0, file - 1); f <= std::min(7, file + 1); ++f) {
                    int suppSq = supportRank * 8 + f;
                    if (friendPawns & SQUARE_BB[suppSq]) {
                        score += (color == WHITE ? 10 : -10);
                        break;
                    }
                }
            }

            // Blockade penalty
            int blockSq = sq + (color == WHITE ? 8 : -8);
            if (blockSq >= 0 && blockSq < 64 && board.getPieceOn(static_cast<Square>(blockSq)) != NONE) {
                score += (color == WHITE ? -15 : 15);
            }
        }
    }

    //std::cout << "Passed Pawn score: " << score << std::endl;

    return score;
}

int computeGamePhase(const Board& board) {
    const int maxPhase = 24;

    int phase = maxPhase;

    // Subtract remaining piece phase values
    phase -= popcount(board.getBitboard(ROOK, WHITE))   * 2;
    phase -= popcount(board.getBitboard(KNIGHT, WHITE)) * 1;
    phase -= popcount(board.getBitboard(BISHOP, WHITE)) * 1;
    phase -= popcount(board.getBitboard(QUEEN, WHITE))  * 4;

    phase -= popcount(board.getBitboard(ROOK, BLACK))   * 2;
    phase -= popcount(board.getBitboard(KNIGHT, BLACK)) * 1;
    phase -= popcount(board.getBitboard(BISHOP, BLACK)) * 1;
    phase -= popcount(board.getBitboard(QUEEN, BLACK))  * 4;

    // Clamp between 0 and 24
    if (phase < 0) phase = 0;
    if (phase > maxPhase) phase = maxPhase;

    return phase;
}


};
