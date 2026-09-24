// zobrist.cc
#include "zobrist.hh"
#include <random>
#include "types.hh"

using namespace types;

uint64_t pieceKeys[2][6][64];
uint64_t sideKey;
uint64_t castlingKeys[16];
uint64_t enPassantKeys[8];

void Board::ZorbistInit() const
{
    std::mt19937_64 rng(0xABCDEF123456789); // fixed seed for reproducibility
    auto rand64 = [&]()
    { return rng(); };

    for (int c = 0; c < 2; ++c)
        for (int p = 0; p < 6; ++p)
            for (int sq = 0; sq < 64; ++sq)
                pieceKeys[c][p][sq] = rand64();

    for (int i = 0; i < 16; ++i)
        castlingKeys[i] = rand64();

    for (int i = 0; i < 8; ++i)
        enPassantKeys[i] = rand64();

    sideKey = rand64();
}

uint64_t Board::computeZobristHash() const
{
    uint64_t h = 0ULL;

    for (int color = 0; color < 2; ++color)
    {
        for (int piece = 0; piece < 6; ++piece)
        {
            Bitboard b = pieces[color][piece];
            while (b)
            {
                int sq = pop_lsb(b);
                h ^= pieceKeys[color][piece][sq];
            }
        }
    }

    if (sideToMove == BLACK)
        h ^= sideKey;

    h ^= castlingKeys[CastlingRights];

    if (enPassantSquare != UNDEFINED)
    {
        int file = enPassantSquare % 8;
        h ^= enPassantKeys[file];
    }

    return h;
}
