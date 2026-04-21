#include <array>
#include "Common.hpp"
#include "Board.hpp"

void Board::move(const Coord from, const Coord to)
{
    matrix[to.row][to.col] = matrix[from.row][from.col];
    matrix[from.row][from.col] = Piece{PieceType::BLANK, Color::BLACK, 'b'};
}

std::array<std::array<Piece, 8>, 8> Board::snapshot() const
{
    return matrix;
}

void Board::setMatrix(const std::array<std::array<Piece, 8>, 8> &newMatrix)
{
    matrix = newMatrix;
}

Piece Board::getPiece(const Coord at) const
{
    if (isValidCoord(at))
    {
        return matrix[at.row][at.col];
    }

    return Piece{PieceType::ERROR};
}

void Board::setPiece(const Coord at, const Piece p)
{
    if (isValidCoord(at))
    {
        matrix[at.row][at.col] = p;
    }
}

Coord Board::getCoordinates(const Piece p) const
{
    if (p.t == PieceType::ERROR || p.t == PieceType::BLANK)
        return Coord{8, 8};

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (getPiece(Coord{i, j}) == p)
                return Coord{i, j};
        }
    }

    return Coord{8, 8};
}
