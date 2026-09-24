#include "../include/search.hh"
#include "../include/types.hh"
#include "../include/tt.hh"
#include "../include/eval.hh"
#include <algorithm>

using namespace std;
using namespace types;
using namespace eval;

int scoreMoveByFlag(MoveFlag flag)
{
    switch (flag)
    {
    case PROMO_CAPTURE:
        return 850000;
    case PROMOTION:
        return 800000;
    case CAPTURE:
        return 700000;
    case EN_PASSANT:
        return 600000;
    case CASTLING:
        return 500000;
    case QUIET:
        return 0;
    default:
        return -1;
    }
}

types::searchResult findBestMove(Board &board, int depth)
{
    Move bestMove = Move{UNDEFINED, UNDEFINED, PAWN, NONE, QUEEN, false, false, PROMO_CAPTURE};
    int bestScore;
    bool maximizingPlayer = (board.getSideToMove() == WHITE);
    bestScore = maximizingPlayer ? -1000000 : 1000000;

    auto moves = board.generateLegalMoves();

    if (moves.size() == 0)
        return {bestMove, 0};

    std::sort(moves.begin(), moves.end(), [&](const Move &a, const Move &b)
              { return scoreMoveByFlag(a.flag) > scoreMoveByFlag(b.flag); });

    for (Move move : moves)
    {
        board.makeMove(move);
        int score = minimax(board, depth - 1, -1000000, 1000000, !maximizingPlayer);
        board.undoMove();

        if ((maximizingPlayer && score > bestScore) || (!maximizingPlayer && score < bestScore))
        {
            bestScore = score;
            bestMove = move;
        }
    }

    return {bestMove, bestScore};
}

types::searchResult findBestMoveTimed(Board &board, int timeLimitMs)
{
    using namespace std::chrono;
    auto start = high_resolution_clock::now();

    Move bestMove;
    int bestScore;
    int depth = 1;
    bool maximizingPlayer = (board.getSideToMove() == WHITE);
    bestScore = maximizingPlayer ? -1000000 : 1000000;

    while (true)
    {
        auto now = high_resolution_clock::now();
        int elapsed = duration_cast<milliseconds>(now - start).count();
        if (elapsed > timeLimitMs)
            break;

        auto moves = board.generateLegalMoves();
        std::sort(moves.begin(), moves.end(), [&](const Move &a, const Move &b)
                  { return scoreMoveByFlag(a.flag) > scoreMoveByFlag(b.flag); });

        for (const Move &move : moves)
        {
            board.makeMove(move);
            int score = minimax(board, depth - 1, -1000000, 1000000, !maximizingPlayer);
            board.undoMove();

            if ((maximizingPlayer && score > bestScore) || (!maximizingPlayer && score < bestScore) || depth == 1)
            {
                bestScore = score;
                bestMove = move;
            }

            // Check time between moves too
            now = high_resolution_clock::now();
            elapsed = duration_cast<milliseconds>(now - start).count();

            printTimerBar(elapsed, timeLimitMs, 30);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            if (elapsed > timeLimitMs)
                break;
        }

        depth++;
    }

    std::cout << "Found move with depth of: " << depth - 1 << std::endl;
    return {bestMove, bestScore};
}

int minimax(Board &board, int depth, int alpha, int beta, bool maximizingPlayer)
{
    if (depth == 0)
    {
        return quiescence(board, alpha, beta, maximizingPlayer);
        // return evaluate(board);
    }

    int alphaOrig = alpha;
    uint64_t key = board.computeZobristHash();
    int ttScore;

    if (TT::probe(key, depth, alpha, beta, ttScore))
    {
        return ttScore;
    }

    auto moves = board.generateLegalMoves();

    // Sort moves by importance (captures first, etc.)
    std::sort(moves.begin(), moves.end(), [&](const Move &a, const Move &b)
              { return scoreMoveByFlag(a.flag) > scoreMoveByFlag(b.flag); });

    // If no legal moves: checkmate or stalemate
    if (moves.empty())
    {
        if (board.isKingOfTheSideToMoveAttacked())
        {
            return maximizingPlayer ? -1000000 + depth : 1000000 - depth; // Checkmate (bad for side to move)
        }
        else
        {
            return 0; // Stalemate
        }
    }

    int bestEval = maximizingPlayer ? -1000000 : 1000000;
    Move bestMove;

    for (const Move &move : moves)
    {
        board.makeMove(move);
        int eval = minimax(board, depth - 1, alpha, beta, !maximizingPlayer);
        board.undoMove();

        if (maximizingPlayer)
        {
            if (eval > bestEval)
            {
                bestEval = eval;
                bestMove = move;
            }
            alpha = std::max(alpha, eval);
        }
        else
        {
            if (eval < bestEval)
            {
                bestEval = eval;
                bestMove = move;
            }
            beta = std::min(beta, eval);
        }

        if (beta <= alpha)
            break; // Alpha-beta pruning
    }

    // Transposition table storage
    TT::NodeType type;
    if (bestEval <= alphaOrig)
        type = TT::UPPERBOUND;
    else if (bestEval >= beta)
        type = TT::LOWERBOUND;
    else
        type = TT::EXACT;

    TT::store(key, depth, bestEval, bestMove, type);

    return bestEval;
}

int quiescence(Board &board, int alpha, int beta, bool maximizingPlayer)
{
    int stand_pat = evaluate(board);

    if (maximizingPlayer)
    {
        if (stand_pat >= beta)
            return beta;
        if (alpha < stand_pat)
            alpha = stand_pat;
    }
    else
    {
        if (stand_pat <= alpha)
            return alpha;
        if (beta > stand_pat)
            beta = stand_pat;
    }

    auto moves = board.generateLegalMoves();
    for (const Move &move : moves)
    {
        if (move.flag == CAPTURE)
        {
            board.makeMove(move);
            int score = quiescence(board, alpha, beta, !maximizingPlayer);
            board.undoMove();

            if (maximizingPlayer)
            {
                if (score >= beta)
                    return beta;
                if (score > alpha)
                    alpha = score;
            }
            else
            {
                if (score <= alpha)
                    return alpha;
                if (score < beta)
                    beta = score;
            }
        }
    }

    return maximizingPlayer ? alpha : beta;
}
