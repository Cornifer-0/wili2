//search.hh
//main file in WILI2.0 for searching algorithms.

#pragma once
#include "board.hh"
#include <chrono>
#include <thread>
//#include "types.hh"

/**
 * @brief Using the negamax algorithm (a version of the minimax algorithm). Calculates moves to a certain depth.
 * It computes the static evaluation at the leafs of the tree, and returns the most beneficial move for the sideToMove.
 * It uses alpha-beta pruning to prune unnecessary branches. 
 * 
 * @param board 
 * @param depth 
 * @param alpha 
 * @param beta 
 * @return int 
 */
int minimax(Board& board, int depth, int alpha, int beta, bool maximizingPlayer);

/**
 * @brief Finds the "best move" in the position calculating to a ceratin depth using hte negamax funciton.
 * 
 * @param board 
 * @param depth 
 * @return types::searchResult 
 */
types::searchResult findBestMove(Board& board, int depth);
types::searchResult findBestMoveTimed(Board& board, int timeLimitMs);

/**
 * @brief It gives a score to each MoveFlag, useful when sorting moves by probability of them, being good moves.
 * 
 * @param flag 
 * @return int 
 */
int scoreMoveByFlag(types::MoveFlag flag);


/**
 * @brief Construct a new quiescence
 * 
 * @param board 
 * @param alpha 
 * @param beta 
 */
int quiescence(Board& board, int alpha, int beta, bool maximizingPlayer);


inline void printTimerBar(int elapsedMs, int totalMs, int barWidth) {
    float progress = std::min(1.0f, float(elapsedMs) / totalMs);
    int pos = progress * barWidth;

    std::cout << "\r[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << elapsedMs / 1000.0f << "s / " << totalMs / 1000.0f << "s";
    std::cout.flush();
}