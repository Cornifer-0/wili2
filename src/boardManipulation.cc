
#include <cassert>
#include <iostream>
#include "board.hh"
#include "types.hh"
#include "defs.hh"
#include "bitboard.hh"

using namespace std;
using namespace types;

void Board::makeMove(Move move) {
    // Save current state
    MoveState state;
    state.move = move;
    state.piece_captured = move.piece_captured;
    state.enPassantSquare = enPassantSquare;
    state.castlingRights = CastlingRights;
    state.halfMoveClock = halfmoveClock;
    state.fullmoveNumber = fullmoveNumber;
    history.push_back(state);

    Bitboard fromBB = SQUARE_BB[move.from];
    Bitboard toBB   = SQUARE_BB[move.to];

    // Clear the piece from the from-square
    pieces[sideToMove][move.piece] &= ~fromBB;

    // Handle en passant capture
    if (move.is_en_passant) {
        // cout << "IS ENPASSANT " << endl;
        int epCapturedPawnSquare = (sideToMove == WHITE) ? move.to + 8 : move.to - 8;
        // Bitboards::printBitboard(SQUARE_BB[epCapturedPawnSquare]);
        // Bitboards::printBitboard(pieces[!sideToMove][PAWN]);
        pieces[!sideToMove][PAWN] &= ~SQUARE_BB[epCapturedPawnSquare];
        // Bitboards::printBitboard(pieces[!sideToMove][PAWN]);
    }

    // Handle normal capture
    if (move.piece_captured != NONE && !move.is_en_passant) {
        pieces[!sideToMove][move.piece_captured] &= ~toBB;
    }

    // Handle promotions
    if (move.promotion != NONE) {
        pieces[sideToMove][move.promotion] |= toBB;
    } else {
        pieces[sideToMove][move.piece] |= toBB;
    }

    // Handle castling
    if (move.is_castling) {
        if (move.to == G1) { // White kingside
            pieces[WHITE][ROOK] &= ~SQUARE_BB[H1];
            pieces[WHITE][ROOK] |= SQUARE_BB[F1];
        } else if (move.to == C1) { // White queenside
            pieces[WHITE][ROOK] &= ~SQUARE_BB[A1];
            pieces[WHITE][ROOK] |= SQUARE_BB[D1];
        } else if (move.to == G8) { // Black kingside
            pieces[BLACK][ROOK] &= ~SQUARE_BB[H8];
            pieces[BLACK][ROOK] |= SQUARE_BB[F8];
        } else if (move.to == C8) { // Black queenside
            pieces[BLACK][ROOK] &= ~SQUARE_BB[A8];
            pieces[BLACK][ROOK] |= SQUARE_BB[D8];
        }
    }

    // Update en passant square
    if (move.piece == PAWN && abs(move.from - move.to) == 16) {
        enPassantSquare = static_cast<types::Square>((move.from + move.to) / 2);
    } else {
        enPassantSquare = UNDEFINED;
    }

    // Update castling rights
    if (move.piece == KING) {
        if (sideToMove == WHITE) {
            CastlingRights &= ~(1 << 0); // No white king-side
            CastlingRights &= ~(1 << 1); // No white queen-side
        } else {
            CastlingRights &= ~(1 << 2); // No black king-side
            CastlingRights &= ~(1 << 3); // No black queen-side
        }
    } else if (move.piece == ROOK) {
        if (move.from == H1) CastlingRights &= ~(1 << 0);
        if (move.from == A1) CastlingRights &= ~(1 << 1);
        if (move.from == H8) CastlingRights &= ~(1 << 2);
        if (move.from == A8) CastlingRights &= ~(1 << 3);
    }

    // Also remove castling rights if a rook was captured
    if (move.piece_captured == ROOK) {
        if (move.to == H1) CastlingRights &= ~(1 << 0);
        if (move.to == A1) CastlingRights &= ~(1 << 1);
        if (move.to == H8) CastlingRights &= ~(1 << 2);
        if (move.to == A8) CastlingRights &= ~(1 << 3);
    }

    // Update halfmove clock
    if (move.piece == PAWN || move.piece_captured != NONE) {
        halfmoveClock = 0;
    } else {
        halfmoveClock++;
    }

    // Update fullmove number
    if (sideToMove == BLACK) {
        fullmoveNumber++;
    }

    // Change side
    sideToMove = !sideToMove;

    // Update color bitboards
    colors[WHITE] = pieces[WHITE][PAWN] | pieces[WHITE][KNIGHT] | pieces[WHITE][BISHOP] |
                    pieces[WHITE][ROOK] | pieces[WHITE][QUEEN] | pieces[WHITE][KING];
    colors[BLACK] = pieces[BLACK][PAWN] | pieces[BLACK][KNIGHT] | pieces[BLACK][BISHOP] |
                    pieces[BLACK][ROOK] | pieces[BLACK][QUEEN] | pieces[BLACK][KING];
    occupancy = colors[WHITE] | colors[BLACK];
}

void Board::undoMove() {
    if (history.empty()) {
        cout << "No move to undo!" << endl;
        return;
    }

    // Pop the last move state
    MoveState lastState = history.back();
    history.pop_back();

    Move move = lastState.move;

    // Switch side back
    sideToMove = !sideToMove;

    // Restore clocks and state
    enPassantSquare = lastState.enPassantSquare;
    CastlingRights  = lastState.castlingRights;
    halfmoveClock   = lastState.halfMoveClock;
    fullmoveNumber  = lastState.fullmoveNumber;

    Bitboard fromBB = SQUARE_BB[move.from];
    Bitboard toBB   = SQUARE_BB[move.to];

    // Handle promotions
    if (move.promotion != NONE) {
        pieces[sideToMove][move.promotion] &= ~toBB; // Remove promoted piece
        pieces[sideToMove][PAWN] |= fromBB;           // Restore pawn at from square
    } else {
        pieces[sideToMove][move.piece] &= ~toBB;
        pieces[sideToMove][move.piece] |= fromBB;
    }

    // Handle captures
    if (move.is_en_passant) {
        // Restore captured pawn
        int capturedPawnSq = (sideToMove == WHITE) ? move.to + 8 : move.to - 8;
        pieces[!sideToMove][PAWN] |= SQUARE_BB[capturedPawnSq];

        // Remove the capturing pawn from the target square
        pieces[sideToMove][PAWN] &= ~toBB;
        // It was already restored to `fromBB` above
    } else if (move.piece_captured != NONE) {
        pieces[!sideToMove][move.piece_captured] |= toBB;
    }

    // Undo castling
    if (move.is_castling) {
        if (move.to == G1) { // White kingside
            pieces[WHITE][ROOK] &= ~SQUARE_BB[F1];
            pieces[WHITE][ROOK] |= SQUARE_BB[H1];
        } else if (move.to == C1) { // White queenside
            pieces[WHITE][ROOK] &= ~SQUARE_BB[D1];
            pieces[WHITE][ROOK] |= SQUARE_BB[A1];
        } else if (move.to == G8) { // Black kingside
            pieces[BLACK][ROOK] &= ~SQUARE_BB[F8];
            pieces[BLACK][ROOK] |= SQUARE_BB[H8];
        } else if (move.to == C8) { // Black queenside
            pieces[BLACK][ROOK] &= ~SQUARE_BB[D8];
            pieces[BLACK][ROOK] |= SQUARE_BB[A8];
        }
    }

    // Recalculate color and occupancy bitboards
    colors[WHITE] = pieces[WHITE][PAWN] | pieces[WHITE][KNIGHT] | pieces[WHITE][BISHOP] |
                    pieces[WHITE][ROOK] | pieces[WHITE][QUEEN] | pieces[WHITE][KING];
    colors[BLACK] = pieces[BLACK][PAWN] | pieces[BLACK][KNIGHT] | pieces[BLACK][BISHOP] |
                    pieces[BLACK][ROOK] | pieces[BLACK][QUEEN] | pieces[BLACK][KING];
    occupancy = colors[WHITE] | colors[BLACK];
}
