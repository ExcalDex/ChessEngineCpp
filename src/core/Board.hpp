#pragma once
#include <array>
#include "Common.hpp"

// Model Class.
// Wrapper for accessing the 8x8 Matrix.
// Only operates on the matrix, has no actual chess logic.
class Board
{
public:
    // Moves piece at `from` to `to`.
    void move(Coord from, Coord to);

    // Returns a copy of the current board matrix.
    std::array<std::array<Piece, 8>, 8> snapshot() const;

    // Sets matrix values.
    // This method will ignore any movement logic for the previous pieces. It's the caller's responsibility to track that logic if using this method.
    void setMatrix(const std::array<std::array<Piece, 8>, 8> &newMatrix);

    // Returns piece at the given position.
    // Returns a piece with `PieceType::ERROR` when `Coord at` is out of bounds
    Piece getPiece(Coord at) const;

    // Sets piece at given position.
    void setPiece(Coord at, Piece p);

    // Finds the coordinates of a piece that matches the given `Piece p`
    // Returns {8, 8} if it doesn't find matches
    Coord getCoordinates(Piece p) const;

private:
    std::array<std::array<Piece, 8>, 8> matrix;
};
