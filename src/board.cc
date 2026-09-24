//board.cc
//for wili2.0 extends board.hh

#include "../include/board.hh"
#include "../include/types.hh"
#include "../include/magic.hh"
#include "../include/bitboard.hh"
#include <iostream>
#include <sstream>
#include <cctype>
#include <bitset>

using namespace std;
using namespace types;

Board::Board() {
    resetBitboards();
    ZorbistInit();
    //pieces[1][2] = 13;
}

void Board::render() const {
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){

            Square sq = Square((i * 8) + j);

            bool found = false;
            for(int color = 0; !found && color < 2; color++){
                for(int piece = PAWN; !found && piece < 6; piece++){
                    if(pieces[color][piece] & SQUARE_BB[sq]){
                        found = true;
                        cout << pieceIcon(PieceType(piece), Color(color)) << ' ';
                    }
                }
            }
            if(not found) cout << ". ";

        }
        cout << endl;
    }
    if(enPassantSquare != UNDEFINED) {
        cout << enPassantSquare << endl;
    }


}



void Board::loadFen(string fen) {
    istringstream iss(fen);

    //Fen page

    string piecePlacement;
    char activeColor;
    string castlingFen;
    string possibleEnPassantSquare;

    iss >> piecePlacement >> activeColor >> castlingFen;
    iss >> possibleEnPassantSquare >> halfmoveClock >> fullmoveNumber;

    // Set up the bitboards
    Square sq = A8;
    //cout << "SETTING UP : " << piecePlacement << endl;
    for(char c : piecePlacement){
        if( isalpha(c) ){
            //its a piece
            Color col;
            if( c >= 'a' && c <= 'z') col = BLACK;
            else{
                col = WHITE;
                c = tolower(c);
            }

            //cout << c << endl;
            PieceType pt;
            if(c == 'p') pt = PAWN;
            else if(c == 'n') pt = KNIGHT;
            else if(c == 'b') pt = BISHOP;
            else if(c == 'r') pt = ROOK;
            else if(c == 'q') pt = QUEEN;
            else if(c == 'k') pt = KING;
            else continue;

            pieces[col][pt] |= SQUARE_BB[sq];
            colors[col] |= SQUARE_BB[sq];
            occupancy |= SQUARE_BB[sq];

            sq = Square(int(sq) + 1); // Increment square after placing a piece

        }else if( c != '/' ){
            //number of blank spaces;
            sq = Square(int(sq) + (c - '0')); // Increment square for blank spaces
        }
    }

    if(activeColor == 'w') sideToMove = WHITE;
    else sideToMove = BLACK;

    CastlingRights = 0;
    for(char c : castlingFen){
        if( c == 'K' ) CastlingRights |= WHITE_KINGSIDE;
        else if(c == 'Q') CastlingRights |= WHITE_QUEENSIDE;
        else if(c == 'k') CastlingRights |= BLACK_KINGSIDE;
        else if(c == 'q') CastlingRights |= BLACK_QUEENSIDE;
    }

    //cout << "castling rights -> " << static_cast<int>(CastlingRights) << endl;

    //En passant square

    // From a string "c4" [col][row] returns the Square (int form). 

    if(possibleEnPassantSquare != "-"){
        char file = possibleEnPassantSquare[0];
        char rank = possibleEnPassantSquare[1];

        int fileIndex = file - 'a';
        int rankIndex = '8' - rank;

        enPassantSquare = static_cast<Square>(rankIndex * 8 + fileIndex);
    }else enPassantSquare = UNDEFINED;

    //Printing types
    // for (int c = 0; c < 2; ++c) { // Assuming 2 colors
    //     for (int p = 0; p < 6; ++p) { // Replace NUM_PIECE_TYPES with the actual number
    //         cout << "pieces[" << c << "][" << p << "] = " << bitset<64>(pieces[c][p]) << endl;
    //     }
    // }

    //Done.
}

void Board::resetBitboards() {
    for(int i = 0; i < 2; i++){
        for(int j = 0; j < 6; j++){
            pieces[i][j] = 0;
        }
    }
    colors[WHITE] = 0;
    colors[BLACK] = 0;
    occupancy = 0;

    history = {};

}
 

// inline Bitboard Board::getBitboard(PieceType p, Color col) const {
//     //cout << pt << ' ' << col << endl;
//     //cout << "BItboard of knights : "<< static_cast<int>(pieces[col][pt]) << endl;
//     return pieces[col][p];
// }


bool Board::isSquareAttacked(types::Square square, types::Color byColor) const {
    // Pawns
    Bitboard pawns = pieces[byColor][PAWN];
    Bitboard attackers = 0;

    if (byColor == BLACK) {
        if (square >= 9 && square % 8 != 0)
            attackers |= (1ULL << (square - 9));
        if (square >= 7 && square % 8 != 7)
            attackers |= (1ULL << (square - 7));
    } else {
        if (square <= 55 && square % 8 != 0)
            attackers |= (1ULL << (square + 7));
        if (square <= 54 && square % 8 != 7)
            attackers |= (1ULL << (square + 9));
    }

    if (attackers & pawns) return true;

    //Knights
    if(Bitboards::KnightMoves[square] & pieces[byColor][KNIGHT]) return true;

    //Bishops and queens
    Bitboard bishopAttacks = Magic::getBishopAttacks(square, occupancy);
    if(bishopAttacks & (pieces[byColor][BISHOP] | pieces[byColor][QUEEN])) return true;

    //Rooks and Queens
    Bitboard rookAttacks = Magic::getRookAttacks(square, occupancy);
    if (rookAttacks & (pieces[byColor][ROOK] | pieces[byColor][QUEEN])) return true;

    // Kings
    if (Bitboards::KingMoves[square] & pieces[byColor][KING]) return true;

    return false;
}

bool Board::isKingOfTheSideToMoveAttacked() const {
    Square myKing = Square(Bitboards::lsb(pieces[sideToMove][KING]));
    return isSquareAttacked(myKing, !sideToMove);
}
