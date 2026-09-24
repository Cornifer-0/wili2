
#include "../include/board.hh"
#include "../include/types.hh"
#include "../include/bitboard.hh"
#include "../include/magic.hh"
#include <iostream>
#include <sstream>
#include <cctype>
#include <bitset>
#include <vector>

using namespace std;
using namespace types;
using namespace Bitboards;


vector<Move> Board::generateLegalMoves() {
    vector<Move> legalMoves;
    vector<Move> pseudoLegalMoves = generatePseudoLegalMoves();
    // cout << "PSEUDO LEGAL MOVES ------------" << endl;
    // for(Move m : pseudoLegalMoves) {
    //     cout << m << ' ';
    // }

    for (const Move& move : pseudoLegalMoves) {
        // cout << "Before : -> : " << move << endl;
        // render();
        makeMove(move);

        Square kingSq = Square(lsb(pieces[!sideToMove][KING]));
        bool legal = true;

        if (move.is_castling) {
            // Get the path the king takes during castling
            Square from = move.from;
            Square to = move.to;

            std::vector<Square> castlePath;
            if (from < to) {
                // Kingside castling
                castlePath = {from, (Square)(from + 1), (Square)(from + 2)};
            } else {
                // Queenside castling
                castlePath = {from, (Square)(from - 1), (Square)(from - 2)};
            }

            for (Square sq : castlePath) {
                if (isSquareAttacked(sq, sideToMove)) {
                    legal = false;
                    break;
                }
            }
        } else {
            // For normal moves, check if our king is in check after the move
            kingSq = Square(lsb(pieces[!sideToMove][KING]));  // After move, sideToMove has switched!
            if (isSquareAttacked(kingSq, sideToMove)) {
                legal = false;
            }
        }

        if (legal) legalMoves.push_back(move);

        undoMove();
        // cout << "After : -> : " << endl;
        // render();
    } 
    

    return legalMoves;
}


vector<Move> Board::generatePseudoLegalMoves() const {
    vector<Move> moves;
    generatePawnMoves(moves);
    generateKnightMoves(moves);
    generateBishopMoves(moves);
    generateRookMoves(moves);
    generateQueenMoves(moves);
    generateKingMoves(moves);

    //cout << "size " << moves.size() << endl;
    return moves;
}

void Board::generateKnightMoves(vector<Move>& moves) const {
    Bitboard knights = getBitboard(KNIGHT, sideToMove);
    //cout << "KNIGHTS " << static_cast<int>(knights) << endl;

    while (knights) {
        int from = pop_lsb(knights);
        Bitboard attacks = KnightMoves[from] & ~getFriendlyOccupancy();

        //cout << "attacks " << bitset<64>(attacks) << endl; 
        while (attacks) {
            int to = pop_lsb(attacks);
            PieceType pt = getPieceOn(Square(to));
            MoveFlag flag = (pt == NONE) ? QUIET : CAPTURE;
            Move move{ static_cast<Square>(from), static_cast<Square>(to), KNIGHT, getPieceOn(Square(to)), NONE, false, false, flag };
            moves.push_back(move);
        }
    }
}

void Board::generatePawnMoves(vector<Move>& moves) const {
    Bitboard pawns = getBitboard(PAWN, sideToMove);

    while (pawns) {
        int from = pop_lsb(pawns);

        // Single pawn push
        Bitboard singlePush = (sideToMove == BLACK ? (1ULL << (from + 8)) : (1ULL << (from - 8))) & ~occupancy;
        if (singlePush) {
            int to = pop_lsb(singlePush);

            // Promotion
            if ((sideToMove == BLACK && to / 8 == 7) || (sideToMove == WHITE && to / 8 == 0)) {
                moves.push_back(Move{ static_cast<Square>(from), static_cast<Square>(to), PAWN, NONE, QUEEN, false, false, PROMOTION });
                moves.push_back(Move{ static_cast<Square>(from), static_cast<Square>(to), PAWN, NONE, ROOK, false, false, PROMOTION  });
                moves.push_back(Move{ static_cast<Square>(from), static_cast<Square>(to), PAWN, NONE, BISHOP, false, false, PROMOTION  });
                moves.push_back(Move{ static_cast<Square>(from), static_cast<Square>(to), PAWN, NONE, KNIGHT, false, false, PROMOTION  });
            } else {
                moves.push_back(Move{ static_cast<Square>(from), static_cast<Square>(to), PAWN, NONE, NONE, false, false, PROMOTION  });
            }

            // Double pawn push
            if ((sideToMove == BLACK && from / 8 == 1) || (sideToMove == WHITE && from / 8 == 6)) {
                Bitboard doublePush = (sideToMove == BLACK ? (1ULL << (from + 16)) : (1ULL << (from - 16))) & ~occupancy;
                if (doublePush) {
                    int to = pop_lsb(doublePush);
                    moves.push_back(Move{ static_cast<Square>(from), static_cast<Square>(to), PAWN, NONE, NONE, false, false, QUIET });
                }
            }
        }

        // Pawn captures
        Bitboard attacks = PawnAttacks[sideToMove][from] & colors[!sideToMove];
        while (attacks) {
            int to = pop_lsb(attacks);

            // Promotion on capture
            if ((sideToMove == BLACK && to / 8 == 7) || (sideToMove == WHITE && to / 8 == 0)) {
                moves.push_back(Move{ static_cast<Square>(from), static_cast<Square>(to), PAWN, getPieceOn(Square(to)), QUEEN, false, false, PROMO_CAPTURE  });
                moves.push_back(Move{ static_cast<Square>(from), static_cast<Square>(to), PAWN, getPieceOn(Square(to)), ROOK, false, false, PROMO_CAPTURE  });
                moves.push_back(Move{ static_cast<Square>(from), static_cast<Square>(to), PAWN, getPieceOn(Square(to)), BISHOP, false, false, PROMO_CAPTURE  });
                moves.push_back(Move{ static_cast<Square>(from), static_cast<Square>(to), PAWN, getPieceOn(Square(to)), KNIGHT, false, false, PROMO_CAPTURE });
            } else {
                moves.push_back(Move{ static_cast<Square>(from), static_cast<Square>(to), PAWN, getPieceOn(Square(to)), NONE, false, false, CAPTURE  });
            }
        }

        // En passant
        if (enPassantSquare != UNDEFINED) {
            Bitboard enPassantAttacks = PawnAttacks[sideToMove][from] & (1ULL << enPassantSquare);
            if (enPassantAttacks) {
                int to = lsb(enPassantAttacks);
                moves.push_back(Move{ static_cast<Square>(from), static_cast<Square>(to), PAWN, PAWN, NONE, false, true, EN_PASSANT });
            }
        }
    }
}

void Board::generateBishopMoves(vector<types::Move>& moves) const {
    //cout << bitset<64>(occupancy) << endl;
    Bitboard bishops = getBitboard(BISHOP, sideToMove);
    Bitboard friends = getFriendlyOccupancy();
    Bitboard occ = occupancy;

    while (bishops) {
        int from = pop_lsb(bishops);
        // Retrieve bishop attacks via magic bitboards
        Bitboard attacks = Magic::getBishopAttacks(from, occ);
        // Remove moves that land on friendly pieces
        attacks &= ~friends;

        while (attacks) {

            int to = pop_lsb(attacks);
            types::PieceType captured = getPieceOn(static_cast<types::Square>(to));
            MoveFlag flag = QUIET;
            if(captured != NONE) flag = CAPTURE;
            types::Move move{
                static_cast<types::Square>(from),
                static_cast<types::Square>(to),
                BISHOP,  // moving piece type
                captured,
                NONE,    // no promotion for bishops
                false,   // not castling
                false,    // not en passant
                flag,
            };
            moves.push_back(move);
        }
    }
}

void Board::generateRookMoves(vector<types::Move>& moves) const {
    Bitboard rooks = getBitboard(ROOK, sideToMove);
    Bitboard friends = getFriendlyOccupancy();
    Bitboard occ = occupancy;

    while (rooks) {

        int from = pop_lsb(rooks);
        Bitboard attacks = Magic::getRookAttacks(from, occ);
        //cout << "rook attacks : " << bitset<64>(attacks) << endl;

        attacks &= ~friends;

        while (attacks) {

            int to = pop_lsb(attacks);
            types::PieceType captured = getPieceOn(static_cast<types::Square>(to));
            MoveFlag flag = QUIET;
            if(captured != NONE) flag = CAPTURE;
            types::Move move{
                static_cast<types::Square>(from),
                static_cast<types::Square>(to),
                ROOK,
                captured,
                NONE,
                false,
                false,
                flag
            };
            moves.push_back(move);
        }
    }
}

void Board::generateQueenMoves(vector<types::Move>& moves) const {
    // Queen moves are a union of rook and bishop moves.
    Bitboard queens = getBitboard(QUEEN, sideToMove);
    Bitboard friends = getFriendlyOccupancy();
    Bitboard occ = occupancy;

    while (queens) {
        int from = pop_lsb(queens);
        Bitboard attacks = Magic::getRookAttacks(from, occ) | Magic::getBishopAttacks(from, occ);
        attacks &= ~friends;

        while (attacks) {
            int to = pop_lsb(attacks);
            types::PieceType captured = getPieceOn(static_cast<types::Square>(to));
            MoveFlag flag = QUIET;
            if(captured != NONE) flag = CAPTURE;
            types::Move move{
                static_cast<types::Square>(from),
                static_cast<types::Square>(to),
                QUEEN,
                captured,
                NONE,
                false,
                false,
                flag
            };
            moves.push_back(move);
        }
    }
}

void Board::generateKingMoves(vector<Move>& moves) const {
    Bitboard king = getBitboard(KING, sideToMove);
    int from = lsb(king);
    Bitboard attacks = KingMoves[from] & ~getFriendlyOccupancy();

    // Bitboards::printBitboard(king);
    // cout << from << endl;
    // Bitboards::printBitboard(getFriendlyOccupancy());
    // Bitboards::printBitboard((~getFriendlyOccupancy()));
    // Bitboards::printBitboard(attacks);
    //Normal king moves
    while (attacks) {
        int to = pop_lsb(attacks);
        PieceType pt = getPieceOn(Square(to));
        MoveFlag flag = QUIET;
        if(pt != NONE) flag = CAPTURE;
        Move move{ static_cast<Square>(from), static_cast<Square>(to), KING, pt, NONE, false, false, flag };
        moves.push_back(move);
    }
    // Castling moves
    if (sideToMove == WHITE) {
        // White kingside castling
        if ((CastlingRights & 0b0001) && !(occupancy & 0x6000000000000000)) {
            moves.push_back(Move{ E1, G1, KING, NONE, NONE, true, false, CASTLING });
        }
        // White queenside castling
        if ((CastlingRights & 0b0010) && !(occupancy & 0xE00000000000000)) {
            moves.push_back(Move{ E1, C1, KING, NONE, NONE, true, false, CASTLING  });
        }
    } else {
        // Black kingside castling
        if ((CastlingRights & 0b0100) && !(occupancy & 0x60)) {
            moves.push_back(Move{ E8, G8, KING, NONE, NONE, true, false, CASTLING  });
        }
        // Black queenside castling
        if ((CastlingRights & 0b1000) && !(occupancy & 0xE)) {
            moves.push_back(Move{ E8, C8, KING, NONE, NONE, true, false, CASTLING  });
        }
    }
}

