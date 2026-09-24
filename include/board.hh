// board.hh
// main file for board representation in WILI2.0

#pragma once
#include <cstdint>
#include <array>
#include <string>
#include <vector>
#include "types.hh"
#include "defs.hh"

using Bitboard = uint64_t;

class Board{
private:
    Bitboard pieces[2][6]; // [COLOR][PIECETYPE]
    Bitboard colors[2]; // all white or black pieces
    Bitboard occupancy; // occupancy

    uint8_t CastlingRights; // 0000 [BQ][BK][WQ][WK] 8 bits
    types::Color sideToMove; // 0 for white, 1 for black 

    types::Square enPassantSquare;
    uint8_t halfmoveClock;
    uint16_t fullmoveNumber;

    std::vector<types::MoveState> history;

public:

    /**
        @brief Default constructor
    */
    Board();

    /**
        @brief Initialize board with fen
        @param fen
    */
    Board(std::string fen);

    /**
        @brief Loads fen into the board
        @param fen
    */
    void loadFen(std::string fen);
    
    /**
        @brief Renders the board through the channel output.
    */
    void render() const;


    /**
     * @brief Returns the list of Pseudo-Legal Moves in the position.
     * Meaning, moves that can be made regarding king safety.
     * 
     * @return std::vector<types::Move> 
     */
    std::vector<types::Move> generatePseudoLegalMoves() const;

    /**
     * @brief Returns the list of Legal Chess Moves that can be made
     * in the current position.
     * 
     * @return std::vector<types::Move> 
     */
    std::vector<types::Move> generateLegalMoves();

    /**
     * @brief Plays the move Move in the current positino.
     * WARNING: It doesn't check of the legality of the move, therefore
     * Move should be checked to be legal.
     * @param types::Move
     */
    void makeMove(types::Move) ;


    /**
     * @brief Yields you the board state before the last move was played
        If there is no register of a last move. It doesn't do annything.
     * 
     */
    void undoMove() ;

    // Depracted (Now not a method, but a function in eval.hh)
    // /**
    //     @brief Returns an static evaluation of the board. 
    //     @return int
    // */
    // int evaluate() const ;

    /**
     * @brief It initializes the Zorbist random integers,
     * must be called at the beggening of the execution, so the Transposition Table works.
     * 
     */
    void ZorbistInit() const ;

    /**
     * @brief Returns the ZorbistHash of the current position.
     * For this Hash to be "unique", ZorbistInit() must have been called before.
     * @return uint64_t 
     */
    uint64_t computeZobristHash() const ;

    /**
     * @brief Returns true if the king of the SideToMove is attacked.
     * Useful to differentiate between checkmate and stalemate.
     * @return true 
     * @return false 
     */
    bool isKingOfTheSideToMoveAttacked() const ;


    /**
     * @brief Returns true if and only if the Square square is attacked by 
     * the Color byColor.
     * 
     * @param square 
     * @param byColor 
     * @return true 
     * @return false 
     */
    bool isSquareAttacked(types::Square square, types::Color byColor) const ;


    /**
     * @brief Get the Bitboard object, returns you the bitboard from the Board
     * pieces[col][pt].
     * 
     * @param pt 
     * @param col 
     * @return Bitboard 
     */
    inline Bitboard getBitboard(types::PieceType p, types::Color col) const { return pieces[col][p]; }

    /**
     * @brief Get the Side To Move.
     * 
     * @return types::Color 
     */
    inline types::Color getSideToMove() const { return sideToMove; }

    /**
     * @brief Get the colors[col]
     * 
     * @param col 
     * @return Bitboard 
     */
    inline Bitboard getOcupancy(types::Color col) const { return colors[col]; }

    /**
     * @brief Get the Ocupancy bitboard
     * 
     * @return Bitboard 
     */
    inline Bitboard getOcupancy() const { return occupancy; }


    /**
     * @brief Get the Piece On. Returns the PieceType of the piece on the paramater to.
     * If there is no piece on the Square to. It returns NONE.
     * 
     * @param to 
     * @return types::PieceType 
     */
    inline types::PieceType getPieceOn(types::Square to) const {
        for (int color = 0; color < 2; ++color) {
            for (int pieceType = 0; pieceType < 6; ++pieceType) {
                if (pieces[color][pieceType] & SQUARE_BB[to]) {
                    return static_cast<types::PieceType>(pieceType);
                }
            }
        }
        return types::PieceType::NONE;
    }


private:

    /**
     * @brief Resets all bitboards, and sets them to 0.
     */
    void resetBitboards();


    /**
     * @brief Get the Friendly Occupancy of the sideToMove,
     * Meaning, a bitboard, where all active bits represent the pieces from the sideToMove.
     * 
     * @return Bitboard 
     */
    inline Bitboard getFriendlyOccupancy() const {
        return colors[sideToMove];
    }

    /**
     * @brief Returns all pseudo-legal Moves for the Kights of the sideToMove color.
     * 
     * @param moves 
     */
    void generateKnightMoves(std::vector<types::Move>& moves) const ;

    /**
     * @brief Returns all pseudo-legal Moves for the Kings of the sideToMove color.
     * 
     * @param moves 
     */
    void generateKingMoves(std::vector<types::Move>& moves) const ;

    /**
     * @brief Returns all pseudo-legal Moves for the Pawns of the sideToMove color.
     * 
     * @param moves 
     */
    void generatePawnMoves(std::vector<types::Move>& moves) const ;

    /**
     * @brief Returns all pseudo-legal Moves for the Bishops of the sideToMove color.
     * 
     * @param moves 
     */
    void generateBishopMoves(std::vector<types::Move>& moves) const ;

    /**
     * @brief Returns all pseudo-legal Moves for the Rooks of the sideToMove color.
     * 
     * @param moves 
     */
    void generateRookMoves(std::vector<types::Move>& moves) const ;

    /**
     * @brief Returns all pseudo-legal Moves for the Queens of the sideToMove color.
     * 
     * @param moves 
     */
    void generateQueenMoves(std::vector<types::Move>& moves) const ;

};
