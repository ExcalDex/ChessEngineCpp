#include <array>
#include <climits>
#include <cmath>
#include "Game.hpp"
#include "Board.hpp"
#include "Common.hpp"
#include "Engine.hpp"

Engine::Engine(Color c)
{
    selfColor = c;
}

std::array<Coord, 2> Engine::getBestMove(Game game)
{
    if (game.getTurn() != selfColor)
        return {Coord{8, 8}, Coord{8, 8}};

    int max = INT_MIN;
    Coord fromMax = Coord{8, 8};
    Coord toMax = Coord{8, 8};

    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            Coord currentPiece = Coord{row, col};
            Piece p = game.getBoard().getPiece(currentPiece);

            if (p.t != PieceType::BLANK && p.c == selfColor)
            {
                std::array<Coord, 27> posb = game.possibleMoves(currentPiece);
                for (int k = 0; k < 27; k++)
                {
                    if (posb[k] == Coord{8, 8})
                        break;

                    SnapShot snap = game.getSnap();
                    game.applyMove(currentPiece, posb[k]);
                    int minimaxVal = minimax(game, SEARCH_DEPTH);
                    game.revertState(snap);

                    if (minimaxVal > max)
                    {
                        max = minimaxVal;
                        fromMax = currentPiece;
                        toMax = posb[k];
                    }
                }
            }
        }
    }

    return {fromMax, toMax};
}

int Engine::minimax(Game &game, int depth)
{
    if (depth == 0)
        return evaluate(game);

    bool isMaximizing = game.getTurn() == selfColor;
    int bestScore = isMaximizing ? INT_MIN : INT_MAX;
    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            Coord from = Coord{row, col};
            Piece p = game.getBoard().getPiece(from);
            if (p.t == PieceType::BLANK || p.c != game.getTurn())
                continue;

            std::array<Coord, 27> posb = game.possibleMoves(from);
            for (int k = 0; k < 27; k++)
            {
                if (posb[k] == Coord{8, 8})
                    break;

                SnapShot snap = game.getSnap();
                game.applyMove(from, posb[k]);
                int score = minimax(game, depth - 1);
                game.revertState(snap);

                if (isMaximizing)
                    bestScore = std::max(bestScore, score);
                else
                    bestScore = std::min(bestScore, score);
            }
        }
    }
    return bestScore;
}

bool Engine::isEndgame(const Game &game) const
{
    // Count non-pawn, non-king material
    int material = 0;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            Piece p = game.getBoard().getPiece(Coord{i, j});
            switch (p.t)
            {
                case PieceType::QUEEN:
                    material += 900;
                    break;
                case PieceType::ROOK:
                    material += 500;
                    break;
                case PieceType::BISHOP:
                case PieceType::KNIGHT:
                    material += 300;
                    break;
                default:
                    break;
            }
        }
    }

    // Endgame if material is low (no queens, few pieces)
    return material < 1000;
}

int Engine::evaluate(Game &game) const
{
    if (!game.hasMoves(game.getTurn()))
    {
        if (game.isKingInCheck(game.getTurn()))
            // Checkmate
            // Always avoids getting checkmated, always goes for checkmate on the enemy.
            return game.getTurn() == selfColor ? INT_MIN : INT_MAX;
        // Stalemate
        // Always discourage Stalemate, but will go for it if it's the only option
        return -50;
    }
    int score = 0;
    int whiteMultiplr = selfColor == Color::WHITE ? 1 : -1;
    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            Coord from = Coord{row, col};
            Piece p = game.getBoard().getPiece(from);
            if (p.t == PieceType::BLANK)
                continue;
            int val = 0;
            int fixedRow = p.c == Color::WHITE ? 7 - row: row;
            switch (p.t)
            {
                case PieceType::PAWN:

                    val += PAWN_VALUE;
                    val += pawnTable[fixedRow][col];

                    if (p.c == Color::WHITE)
                        val *= whiteMultiplr;
                    else
                        val *= -whiteMultiplr;
                    break;

                case PieceType::ROOK:

                    val += ROOK_VALUE;
                    val += rookTable[fixedRow][col];

                    if (p.c == Color::WHITE)
                        val *= whiteMultiplr;
                    else
                        val *= -whiteMultiplr;
                    break;
                case PieceType::KNIGHT:

                    val += KNIGHT_VALUE;
                    val += knightTable[fixedRow][col];

                    if (p.c == Color::WHITE)
                        val *= whiteMultiplr;
                    else
                        val *= -whiteMultiplr;
                    break;
                case PieceType::BISHOP:

                    val += BISHOP_VALUE;
                    val += bishopTable[fixedRow][col];

                    if (p.c == Color::WHITE)
                        val *= whiteMultiplr;
                    else
                        val *= -whiteMultiplr;
                    break;
                case PieceType::QUEEN:

                    val += QUEEN_VALUE;
                    val += queenTable[fixedRow][col];

                    if (p.c == Color::WHITE)
                        val *= whiteMultiplr;
                    else
                        val *= -whiteMultiplr;
                    break;
                case PieceType::KING:
                    val += kingTable[fixedRow][col];

                    if (p.c == Color::WHITE)
                        val *= whiteMultiplr;
                    else
                        val *= -whiteMultiplr;
                    break;
                default:
                    break;
            }

            score += val;
        }
    }

    return score;
}
