// tests.hh
// List of test functions, to ensure the fucntionality of the board.
#pragma once

#include "board.hh"
#include <string>

namespace Test{

uint64_t perft(Board& board, int depth) ;

void perftDivide(Board& board, int depth) ;

void printMoveTree(Board& board, int depth, std::string prefix = "") ;

} // namespace Test
