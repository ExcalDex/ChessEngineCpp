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
    unsigned int row = 0, col = 0;
};

struct GameState
{
    Color turn;
    bool castling[2][2] = {{true, true}, {true, true}}; // [color: 0 - white, 1 - black][id]
    Coord enPassant;
};

struct SnapShot
{
    std::array<std::array<Piece, 8>, 8> board;
    GameState state;
};

static bool isValidCoord(Coord c) {
  return c.row <= 7 && c.col <= 7;
}

inline bool operator==(const Coord& a, const Coord& b) {
  return a.row == b.row && a.col == b.col;
}

inline bool operator!=(const Coord& a, const Coord& b) {
  return !(a == b);
}