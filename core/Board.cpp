#include <array>
#include "Common.hpp"
#include "Board.hpp"

void Board::move(Coord from, Coord to)
{
    matrix[to.row][to.col] = matrix[from.row][from.col];
    matrix[from.row][from.col] = Piece{PieceType::BLANK, Color::BLACK, 'b'};
}

std::array<std::array<Piece, 8>, 8> Board::snapshot() const
{
    return matrix;
}

void Board::setMatrix(std::array<std::array<Piece, 8>, 8> newMatrix)
{
    matrix = newMatrix;
}

Piece Board::getPiece(Coord c)
{
    if (isValidCoord(c))
    {
        return matrix[c.row][c.col];
    }
    else
        return Piece{PieceType::ERROR};
}

void Board::setPiece(Coord c, Piece p)
{
    if (isValidCoord(c))
    {
        matrix[c.row][c.col] = p;
    }
}