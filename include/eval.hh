//eval.hh
//Main function for static evaluation of positions in WILI2.0

#pragma once

#include "board.hh"
#include "types.hh"


namespace eval {

//stores all preloaded tables. for piece positioning
// Piece Square Tables
int evaluate( const Board& board) ;
int evalMaterial(const Board& board, types::Phase phase) ;
int evalPieceSquareTables(const Board& board, types::Phase phase);
int evalMobility(const Board& board, types::Phase phase);
int evalKingSafety(const Board& board);
int evalPawnStructure(const Board& board);
int evalPassedPawns(const Board& board);

int computeGamePhase(const Board& board);
 
int getPSTValue(types::PieceType pt, types::Square sq, types::Color col, types::Phase phase) ;

} // eval
