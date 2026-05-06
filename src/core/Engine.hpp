#pragma once
#include <array>
#include "Game.hpp"
#include "Board.hpp"
#include "Common.hpp"

// AI Engine (Search & Evaluation)
// Analyzes chess positions and determines optimal moves.
// Uses minimax search with position evaluation to find the best move at a given depth.
// Does not modify game state; returns recommended moves for Game to execute.
class Engine
{
public:
    // `Color c` is the color that the Engine will evaluate for (positive multipliers for evaluation).
    Engine(Color c);

    // Evaluates what is the next best move for the engine.
    // Returns an array of coords, index 0 is `from` and index 1 is `to`.
    // May return an Out-of-bounds coord array if the turn doesn't match the Engine color.
    std::array<Coord, 2> getBestMove(Game game);

private:
    static constexpr int SEARCH_DEPTH = 3;
    Color selfColor;
    // All position value tables
    // Pawn
    static constexpr int pawnTable[8][8] = {
            {0, 0, 0, 0, 0, 0, 0, 0}, // Rank 1
            {5, 5, 5, 5, 5, 5, 5, 5}, // Rank 2
            {10, 10, 10, 10, 10, 10, 10, 10}, // Rank 3
            {20, 20, 25, 30, 30, 25, 20, 20}, // Rank 4
            {30, 30, 35, 40, 40, 35, 30, 30}, // Rank 5
            {50, 50, 55, 60, 60, 55, 50, 50}, // Rank 6
            {100, 100, 100, 100, 100, 100, 100, 100}, // Rank 7 (close to promotion!)
            {0, 0, 0, 0, 0, 0, 0, 0} // Rank 8 (promote)
            };

    // Knight
    static constexpr int knightTable[8][8] = {
            {-50, -40, -30, -30, -30, -30, -40, -50},
            {-40, -20, 0, 0, 0, 0, -20, -40},
            {-30, 0, 10, 15, 15, 10, 0, -30},
            {-30, 5, 15, 20, 20, 15, 5, -30},
            {-30, 0, 15, 20, 20, 15, 0, -30},
            {-30, 5, 10, 15, 15, 10, 5, -30},
            {-40, -20, 0, 5, 5, 0, -20, -40},
            {-50, -40, -30, -30, -30, -30, -40, -50}
            };

    // Bishop
    static constexpr int bishopTable[8][8] = {
            {-20, -10, -10, -10, -10, -10, -10, -20},
            {-10, 0, 0, 0, 0, 0, 0, -10},
            {-10, 0, 5, 10, 10, 5, 0, -10},
            {-10, 5, 5, 10, 10, 5, 5, -10},
            {-10, 0, 10, 10, 10, 10, 0, -10},
            {-10, 10, 10, 10, 10, 10, 10, -10},
            {-10, 5, 0, 0, 0, 0, 5, -10},
            {-20, -10, -10, -10, -10, -10, -10, -20}
            };

    // Rook
    static constexpr int rookTable[8][8] = {
            {0, 0, 0, 0, 0, 0, 0, 0},
            {5, 10, 10, 10, 10, 10, 10, 5},
            {-5, 0, 0, 0, 0, 0, 0, -5},
            {-5, 0, 0, 0, 0, 0, 0, -5},
            {-5, 0, 0, 0, 0, 0, 0, -5},
            {-5, 0, 0, 0, 0, 0, 0, -5},
            {-5, 0, 0, 0, 0, 0, 0, -5},
            {0, 0, 0, 5, 5, 0, 0, 0}
            };

    // Queen
    static constexpr int queenTable[8][8] = {
            {-20, -10, -10, -5, -5, -10, -10, -20},
            {-10, 0, 0, 0, 0, 0, 0, -10},
            {-10, 0, 5, 5, 5, 5, 0, -10},
            {-5, 0, 5, 5, 5, 5, 0, -5},
            {0, 0, 5, 5, 5, 5, 0, -5},
            {-10, 5, 5, 5, 5, 5, 0, -10},
            {-10, 0, 5, 0, 0, 0, 0, -10},
            {-20, -10, -10, -5, -5, -10, -10, -20}
            };

    // King (opening/midgame)
    static constexpr int kingTable[8][8] = {
            {-30, -40, -40, -50, -50, -40, -40, -30},
            {-30, -40, -40, -50, -50, -40, -40, -30},
            {-30, -40, -40, -50, -50, -40, -40, -30},
            {-30, -40, -40, -50, -50, -40, -40, -30},
            {-20, -30, -30, -40, -40, -30, -30, -20},
            {-10, -20, -20, -20, -20, -20, -20, -10},
            {20, 20, 0, 0, 0, 0, 20, 20},
            {20, 30, 10, 0, 0, 10, 30, 20}
            };

    // King (endgame)
    static constexpr int kingEndgameTable[8][8] = {
            {-50, -40, -30, -20, -20, -30, -40, -50},
            {-40, -20, 0, 10, 10, 0, -20, -40},
            {-30, 0, 20, 30, 30, 20, 0, -30},
            {-20, 10, 30, 40, 40, 30, 10, -20},
            {-20, 10, 30, 40, 40, 30, 10, -20},
            {-30, 0, 20, 30, 30, 20, 0, -30},
            {-40, -20, 0, 10, 10, 0, -20, -40},
            {-50, -40, -30, -20, -20, -30, -40, -50}
            };

    //Piece Values
    static constexpr int PAWN_VALUE = 100;
    static constexpr int KNIGHT_VALUE = 300;
    static constexpr int BISHOP_VALUE = 300;
    static constexpr int ROOK_VALUE = 500;
    static constexpr int QUEEN_VALUE = 900;

    // Minimax function for interation through moves.
    // Includes Alpha-Beta pruning.
    int minimax(Game &game, int depth, int alpha, int beta);

    // Evaluation function for current board state.
    // Evaluates for material difference and piece positions.
    int evaluate(Game &game) const;

    // Defines when the endgame phase starts, so the king can become more aggressive.
    // Current implementation defines endgame when global material is low (both self and the opponent's).
    bool isEndgame(const Game &game) const;
};
