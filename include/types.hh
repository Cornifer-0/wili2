// types.hh
// File for All types for WILI2.0

#pragma once
#include <cstdint>
#include <string>
#include <iostream>
#include <ostream>
#include <sstream>

namespace types
{

  using Bitboard = uint64_t;

  enum PieceType
  {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    NONE
  };

  enum Color
  {
    WHITE,
    BLACK
  };

  /**
   * @brief Returns the opposite color of c.
   *
   * @param c
   * @return Color
   */
  inline Color operator!(Color c) { return c == WHITE ? BLACK : WHITE; }

  enum Square
  {
    A8,
    B8,
    C8,
    D8,
    E8,
    F8,
    G8,
    H8,
    A7,
    B7,
    C7,
    D7,
    E7,
    F7,
    G7,
    H7,
    A6,
    B6,
    C6,
    D6,
    E6,
    F6,
    G6,
    H6,
    A5,
    B5,
    C5,
    D5,
    E5,
    F5,
    G5,
    H5,
    A4,
    B4,
    C4,
    D4,
    E4,
    F4,
    G4,
    H4,
    A3,
    B3,
    C3,
    D3,
    E3,
    F3,
    G3,
    H3,
    A2,
    B2,
    C2,
    D2,
    E2,
    F2,
    G2,
    H2,
    A1,
    B1,
    C1,
    D1,
    E1,
    F1,
    G1,
    H1,
    UNDEFINED
  };

  /**
   * @brief Returns the string in Standard Chess notation of the Square square.
   * 0 = A8, 2 = B8 ...
   *
   * @param square
   * @return std::string
   */
  inline std::string squareToString(Square square)
  {
    if (square < A8 || square > H1)
      return "undefined";
    char file = 'a' + (square % 8);
    char rank = '8' - (square / 8);
    return std::string{file, rank};
  }

  enum MoveFlag
  {
    QUIET = 0,         // non-capture, non-promotion, non-special
    CAPTURE = 1,       // regular capture
    PROMOTION = 2,     // non-capture promotion
    PROMO_CAPTURE = 3, // promotion + capture
    EN_PASSANT = 4,
    CASTLING = 5,
    CHECK = 6,
  };

  struct Move
  {
    Square from, to;
    PieceType piece, piece_captured;
    PieceType promotion;
    bool is_castling, is_en_passant;
    MoveFlag flag;

    friend std::ostream &operator<<(std::ostream &os, const Move &move)
    {
      if (move.is_castling)
      {
        if (move.to == G1 || move.to == G8)
        {
          os << "O-O"; // Kingside castling
        }
        else if (move.to == C1 || move.to == C8)
        {
          os << "O-O-O"; // Queenside castling
        }
      }
      else
      {
        if (move.piece != PAWN)
        {
          os << "NBRQK"[move.piece - KNIGHT]; // Piece notation (excluding pawn)
        }
        if (move.piece_captured != NONE)
        {
          if (move.piece == PAWN)
          {
            os << squareToString(move.from)[0]; // File of the pawn for captures
          }
          os << "x"; // Capture notation
        }
        os << squareToString(move.to);
        if (move.promotion != NONE)
        {
          os << "=" << "NBRQK"[move.promotion - KNIGHT]; // Promotion notation
        }
      }
      return os;
    }

    bool operator==(const Move &other) const
    {
      return from == other.from &&
             to == other.to &&
             piece == other.piece &&
             piece_captured == other.piece_captured &&
             promotion == other.promotion;
    }
  };

  enum Phase
  {
    MIDGAME,
    ENDGAME,
    // maybe more detailed in the future
  };

  /**
   * @brief Returns the Icon in UNICODE for the PieceType in the color of color.
   *
   * @param piece
   * @param color
   * @return std::string
   */
  inline std::string pieceIcon(PieceType piece, Color color)
  {
    switch (piece)
    {
    case PAWN:
      return color == BLACK ? "\xE2\x99\x99" : "\xE2\x99\x9F";
    case KNIGHT:
      return color == BLACK ? "\xE2\x99\x98" : "\xE2\x99\x9E";
    case BISHOP:
      return color == BLACK ? "\xE2\x99\x97" : "\xE2\x99\x9D";
    case ROOK:
      return color == BLACK ? "\xE2\x99\x96" : "\xE2\x99\x9C";
    case QUEEN:
      return color == BLACK ? "\xE2\x99\x95" : "\xE2\x99\x9B";
    case KING:
      return color == BLACK ? "\xE2\x99\x94" : "\xE2\x99\x9A";
    default:
      return " "; // EMPTY_ICON
    }
  }

  struct MoveState
  {
    Move move; // move played
    PieceType piece_captured;
    Square enPassantSquare;
    uint8_t castlingRights;
    int halfMoveClock;
    int fullmoveNumber;
  };

  struct searchResult
  {
    Move move;
    int evaluation;
  };

}; // namespace types
