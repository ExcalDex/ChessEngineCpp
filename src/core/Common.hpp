#pragma once
#include <array>

enum class Color
{
    BLACK,
    WHITE
};

enum class PieceType
{
    PAWN,
    KING,
    QUEEN,
    KNIGHT,
    BISHOP,
    ROOK,
    BLANK,
    ERROR
};

struct Piece
{
    PieceType t = PieceType::BLANK;
    Color c = Color::BLACK;
    char id = 'b';
};

// All coordinates above 7 are out of bounds.
// The code uses {8, 8} as default for invalid or null coordinates.
struct Coord
{
    int row = 0, col = 0;
};

struct GameState
{
    Color turn = Color::BLACK;
    // [id: 0 = 1, 1 = 2][color: 0 = white, 1 = black]
    // id 1: Queen Side, id 2: King Side
    bool castling[2][2] = {{true, true}, {true, true}};
    Coord enPassant;
};

struct SnapShot
{
    std::array<std::array<Piece, 8>, 8> board;
    GameState state;
};

// Checks if Coordinate is not out of bounds.
inline bool isValidCoord(Coord c) {
  return c.row <= 7 && c.col <= 7 && c.row >= 0 && c.col >= 0;
}

inline bool operator==(const Coord& a, const Coord& b) {
  return a.row == b.row && a.col == b.col;
}

inline bool operator!=(const Coord& a, const Coord& b) {
  return !(a == b);
}

inline bool operator==(const Piece& a, const Piece& b) {
    return a.t == b.t && a.c == b.c && a.id == b.id;
}