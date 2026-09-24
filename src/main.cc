// main file of WILI2.0

#include <iostream>

#include "../include/bitboard.hh"
#include "../include/board.hh"
#include "../include/types.hh"
#include "../include/defs.hh"
#include "../include/magic.hh"
#include "../include/tt.hh"

#include "../include/search.hh"
#include "../include/eval.hh"
#include "../include/tests.hh"
// #include "../include/book.hh"
// #include "../Book.bin"

#include <cstdint>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>

using namespace std;
using namespace types;
using Bitboard = uint64_t;

// TODO => Include WILI file so i can just #include "wili"
// TODO => OPENING BOOK with Polyglot probably
// TODO => ENDGAME Tablebases with Syzygy or Lomosonov.
// TODO => Iterative searching (so i can play against the machine)
// TODO => ...

int main()
{

    Bitboards::init();
    Magic::initMagicTables();
    TT::init();

    Board board;
    board.loadFen(STARTING_FEN);

    cout << "Welcome to WILI Chess! You're playing as WHITE." << endl;

    // cout << "EVAL " <<  findBestMoveTimed(board, 5000).evaluation << endl;
    // cout << "EVAL " <<  findBestMoveTimed(board, 5000).move << endl;

    // board.loadFen("rnb1kbnr/pp1ppppp/2p5/8/3PP3/8/PPPq1PPP/RN1QKBNR w KQkq - 0 4");
    // cout << eval::evaluate(board) << endl;

    // cout << findBestMove(board, 2).move << endl;
    // cout << findBestMove(board, 4).move << endl;
    // cout << findBestMove(board, 6).move << endl;

    while (true)
    {
        board.render();

        // if (board.isGameOver()) {
        //     cout << "Game over!" << endl;
        //     break;`
        // }

        if (board.getSideToMove() == WHITE)
        {
            cout << "Your move (e.g., e4): ";
            string input;
            cin >> input;

            auto legalMoves = board.generateLegalMoves();
            bool found = false;

            if (legalMoves.size() == 0)
            {
                // There are no more moves
                cout << "You lose!" << endl;
            }

            for (const Move &move : legalMoves)
            {
                ostringstream oss;
                oss << move;
                if (oss.str() == input)
                {
                    board.makeMove(move);
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                cout << "Illegal move! Try again." << endl;
            }
        }
        else
        {
            cout << "Bot is thinking...\n";

            auto [botMove, score] = findBestMoveTimed(board, 3000);

            if (botMove.from == UNDEFINED)
            {
                // end of the game, bot lost
                cout << "YOU WIN BY CHECKMATE" << endl;
                return 0;
            }

            cout << "Bot plays: " << botMove << " (Score: " << score << ")" << endl;
            board.makeMove(botMove);

            // if (auto bookMove = Book::getBookMove(board)) {
            //     cout << "Book move: " << *bookMove << endl;
            //     board.makeMove(*bookMove);
            // } else {
            //     auto [botMove, score] = findBestMove(board, 6);
            //     cout << "Bot plays: " << botMove << " (Score: " << score << ")" << endl;
            //     board.makeMove(botMove);
            // }
        }
    }

    return 0;
}

// int main(){

//     Bitboards::init();
//     Magic::initMagicTables();
//     TT::init();

//     Board b;
//     b.loadFen(STARTING_FEN);

//     auto e = findBestMove(b, 7);
//     cout << e.move << endl;
//     cout << e.evaluation << endl;

//     //TODO:

// Perft
//  perftDivide(b, 5);

// printMoveTree(b, 1, "");

// for(int i = 0; i < 9; i++){
//     cout << perft(b, i) << endl;
// }

// cout << b.isSquareAttacked(D8, WHITE);

// auto moves = b.generateLegalMoves();
// int index = 0;
// for(auto m : moves){
//     cout << index << ". " << m << ' ';
//     index++;
// }
// int i;
// while(cin >> i){
//     if(i == -1){
//         b.undoMove();

//     }else {
//         b.makeMove(moves[i]);
//     }
//     b.render();
//     moves = b.generateLegalMoves();
//     int index = 0;
//     for(auto m : moves){
//         cout << index << ". " << m << ' ';
//         index++;
//     }

// }

//     return 0;
// }
