#include <array>
#include <stack>
#include "Game.hpp"
#include "Board.hpp"
#include "Common.hpp"

Game::Game()
{
    gameState.turn = Color::WHITE;
    gameState.enPassant = Coord{8, 8};
    blackKing = Coord{7, 4};
    whiteKing = Coord{0, 4};

    std::array<std::array<Piece, 8>, 8> matrix;

    // Row 7 (Black back rank)
    matrix[7][0] = Piece{PieceType::ROOK, Color::BLACK, '1'};
    matrix[7][1] = Piece{PieceType::KNIGHT, Color::BLACK, '1'};
    matrix[7][2] = Piece{PieceType::BISHOP, Color::BLACK, '1'};
    matrix[7][3] = Piece{PieceType::QUEEN, Color::BLACK, '0'};
    matrix[7][4] = Piece{PieceType::KING, Color::BLACK, '0'};
    matrix[7][5] = Piece{PieceType::BISHOP, Color::BLACK, '2'};
    matrix[7][6] = Piece{PieceType::KNIGHT, Color::BLACK, '2'};
    matrix[7][7] = Piece{PieceType::ROOK, Color::BLACK, '2'};

    // Row 6 (Black pawns)
    for (int col = 0; col < 8; col++)
    {
        matrix[6][col] = Piece{PieceType::PAWN, Color::BLACK, static_cast<char>('1' + col)};
    }

    // Rows 5-2 (Empty - defaults are already BLANK)

    // Row 1 (White pawns)
    for (int col = 0; col < 8; col++)
    {
        matrix[1][col] = Piece{PieceType::PAWN, Color::WHITE, static_cast<char>('1' + col)};
    }

    // Row 0 (White back rank)
    matrix[0][0] = Piece{PieceType::ROOK, Color::WHITE, '1'};
    matrix[0][1] = Piece{PieceType::KNIGHT, Color::WHITE, '1'};
    matrix[0][2] = Piece{PieceType::BISHOP, Color::WHITE, '1'};
    matrix[0][3] = Piece{PieceType::QUEEN, Color::WHITE, '0'};
    matrix[0][4] = Piece{PieceType::KING, Color::WHITE, '0'};
    matrix[0][5] = Piece{PieceType::BISHOP, Color::WHITE, '2'};
    matrix[0][6] = Piece{PieceType::KNIGHT, Color::WHITE, '2'};
    matrix[0][7] = Piece{PieceType::ROOK, Color::WHITE, '2'};

    board.setMatrix(matrix);
}

std::array<Coord, 27> Game::pawnMoves(Coord p)
{
    std::array<Coord, 27> arr;

    // Setting sentinel values - Remember to check
    for (int i = 0; i < 27; i++)
        arr[i] = Coord{8, 8};

    Piece pawn = board.getPiece(p);
    if (pawn.t != PieceType::PAWN)
        return arr;

    int moves[4][2] = {{1, 0}, {2, 0}, {1, 1}, {1, -1}};
    ;
    if (pawn.c == Color::BLACK)
    {
        for (int i = 0; i < 4; i++)
        {
            moves[i][0] *= -1;
            moves[i][1] *= -1;
        }
    }

    int k = 0;
    for (int i = 0; i < 4; i++)
    {
        Coord currentMove = Coord{p.row + moves[i][0], p.col + moves[i][1]};
        if (isValidCoord(currentMove))
        {
            // Diagonals
            if (moves[i][1] == 1 || moves[i][1] == -1)
            {
                if ((board.getPiece(currentMove).c != pawn.c &&
                     board.getPiece(currentMove).t != PieceType::BLANK) ||
                    gameState.enPassant == currentMove)
                {
                    arr[k] = currentMove;
                    k++;
                }
            }
            // Move 1 to the front
            else if (moves[i][0] == 1 || moves[i][0] == -1)
            {
                if (board.getPiece(currentMove).t == PieceType::BLANK)
                {
                    arr[k] = currentMove;
                    k++;
                }
            }
            // Move 2 to the front
            else if (moves[i][0] == 2 || moves[i][0] == -2)
            {
                if (pawn.c == Color::WHITE &&
                    board.getPiece(currentMove).t == PieceType::BLANK &&
                    board.getPiece(Coord{p.row + 1, p.col}).t == PieceType::BLANK &&
                    p.row == 1)
                {
                    arr[k] = currentMove;
                    k++;
                }
                if (pawn.c == Color::BLACK &&
                    board.getPiece(currentMove).t == PieceType::BLANK &&
                    board.getPiece(Coord{p.row - 1, p.col}).t == PieceType::BLANK &&
                    p.row == 6)
                {
                    arr[k] = currentMove;
                    k++;
                }
            }
        }
    }

    return arr;
}
