//tests.cc
//tests file for WILI2.0

#include "../include/tests.hh"
#include "../include/types.hh"
#include <vector>

using namespace types;
using namespace std;

uint64_t perft(Board& board, int depth) {
    if (depth == 0) return 1;
    
    uint64_t nodes = 0;
    auto moves = board.generateLegalMoves();
    
    for (auto& move : moves) {
        board.makeMove(move);
        nodes += perft(board, depth - 1);
        board.undoMove();
    }
    
    return nodes;
}

void perftDivide(Board& board, int depth) {
    vector<Move> moves = board.generateLegalMoves();
    uint64_t total = 0;

    for (const Move& move : moves) {
        board.makeMove(move);
        uint64_t nodes = perft(board, depth - 1);
        board.undoMove();
        
        cout << move << ": " << nodes << endl;
        total += nodes;
    }

    cout << "Total nodes at depth " << depth << ": " << total << endl;
}

void printMoveTree(Board& board, int depth, string prefix = "") {
    if (depth == 0) {
        cout << prefix << endl;
        return;
    }

    vector<Move> moves = board.generateLegalMoves();

    for (const Move& move : moves) {
        board.makeMove(move);
        ostringstream oss;
        oss << move;
        printMoveTree(board, depth - 1, prefix + oss.str() + " ");

        board.undoMove();
    }
}
