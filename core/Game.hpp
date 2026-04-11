#include <array>
#include <stack>
#include "Board.hpp"
#include "Common.hpp"

class Game
{
public:
    // Sets board and game state
    Game();

    bool makeMove(Coord from, Coord to);

    // Returns an array of all possible moves the piece can make from the given position.
    // Possible moves are the ones that follow chess rules and do not result on checks of oneself's king.
    // Returns an array of invalid positions if no moves are available for that piece.
    std::array<Coord, 27> possibleMoves(Coord piece);

    // True if king of color c is in check, False otherwise.
    bool isKingInCheck(Color c);

    // Calculates if the pieces of the matching `c` color have any possible moves.
    // Returns true if any move is found, false if not.
    bool hasMoves(Color c);

    // Reverts the board to its previous state in history.
    void undo();

private:
    std::stack<SnapShot> history;
    Board board;
    GameState gameState;
    Coord blackKing;
    Coord whiteKing;

    // Takes a snapshot of the board, copies the current game state and adds it to the stack.
    // Use only just before making a change on the board or game state.
    void setHistory();

    // Returns an array of possible pawn moves of the given piece.
    // Does not consider self king checks on the calculations.
    std::array<Coord, 27> pawnMoves(Coord piece);

    // Returns an array of possible rook moves of the given piece.
    // Does not consider self king checks on the calculations.
    std::array<Coord, 27> rookMoves(Coord piece);

    // Returns an array of possible knight moves of the given piece.
    // Does not consider self king checks on the calculations.
    std::array<Coord, 27> knightMoves(Coord piece);

    // Returns an array of possible bishop moves of the given piece.
    // Does not consider self king checks on the calculations.
    std::array<Coord, 27> bishopMoves(Coord piece);

    // Returns an array of possible queen moves of the given piece.
    // Does not consider self king checks on the calculations.
    std::array<Coord, 27> queenMoves(Coord piece);

    // Returns an array of possible king moves of the given piece.
    // Does not consider self king checks on the calculations.
    std::array<Coord, 27> kingMoves(Coord piece);

    // Execute a move on the board. ASSUMES move has been validated by caller.
    // Does not check legality or if move leaves king in check.
    bool move(Coord from, Coord to);

    // Calculates if the square in the coordinate sq is being attacked by a piece of the specified color.
    bool isControlledBy(Coord sq, Color attacker);

    // Returns true when move results in a check to the self king
    bool testForCheck(Coord from, Coord to);
};