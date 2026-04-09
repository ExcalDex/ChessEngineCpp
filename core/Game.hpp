#include <array>
#include <stack>
#include "Board.hpp"
#include "Common.hpp"

class Game
{
public:
    // Sets board and game state
    Game();

    // Checks if your move is valid and calls Board.move() to change positions.
    // Updates the game state for each move.
    // Returns true if moved successfully, false if move was illegal or impossible.
    bool move(Coord from, Coord to);

    // Returns an array of all possible moves the piece can make from the given position.
    // Possible moves are the ones that follow chess rules and do not result on checks of oneself's king.
    // Returns an empty array of the position is not valid or if the piece does not have any possible moves.
    std::array<Coord, 27> possibleMoves(Coord piece);

    // Calculates if the king of the matching `c` color is in check.
    // Returns true if it is, false if not.
    bool inCheck(Color c);

    // Calculates if the pieces of the matching `c` color have any possible moves.
    // Returns true if any move is found, false if not.
    bool hasMoves(Color c);

    // Returns a SnapShot of the board and game state.
    SnapShot getGame();

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
};