#include <array>
#include "Common.hpp"

// Matrix template 
// Made for the extra layer of abstraction and simplifying readability for the rest of the code
class Board
{
public:

    // Moves piece at `from` to `to`.
    void move(Coord from, Coord to);

    // Returns a copy of the current board matrix.
    std::array<std::array<Piece, 8>, 8> snapshot() const;

    // Sets matrix values.
    void setMatrix(std::array<std::array<Piece, 8>, 8> newMatrix);

    // Returns piece at the given position.
    Piece getPiece(Coord at);

    // Sets piece at given position.
    void setPiece(Coord at, Piece p);

private:
    std::array<std::array<Piece, 8>, 8> matrix;
};