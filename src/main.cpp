#include "core/Game.hpp"
#include "core/Board.hpp"
#include "core/Common.hpp"
#include "ui/ChessUI.hpp"
#include "core/Engine.hpp"
#include <array>

// Code by André Santos
// Main game instance holds a history stack, accessible through g.getHistory().
// Ui does not give option to change colors, but it's fairly easy to do so by altering the code. Check line 23.

int main()
{
    Game g;
    ChessUI ui;
    Engine engine(Color::BLACK);
    bool running = true;
    while (running)
    {
        // Looking for checkmate or stalemate.
        if (g.hasMoves(g.getTurn()) && !g.staleMateByMaterial())
        {
            // Change this color value to WHITE to play as black.
            if (g.getTurn() == Color::BLACK)
            {
                // Depth higher than 3 is too slow as of now. Will implement better search in the future.
                // Depth lower than 3 is instant, but plays pretty badly.
                std::array<Coord, 2> bestMove = engine.getBestMove(g);
                g.makeMove(bestMove[0], bestMove[1]);
            }
            else
            {
                ui.printBoard(g.getBoard(), g.getTurn());
                ui.println("What piece do you want to move?");
                std::string input;
                Coord pieceCoord = {8, 8};
                // Array of possible moves
                std::array<Coord, 27> posb;
                // Making sure the user inputs a valid piece that is on the board.
                do
                {
                    input = ui.getInput();
                    Piece piece = ui.translatePiece(input);
                    if (piece.t != PieceType::ERROR)
                    {
                        pieceCoord = g.getBoard().getCoordinates(piece);
                        // Stops the user from selecting a piece that can't move (Detects pins).
                        // Not checking for validity before calling because the function will always return sentinel values for invalid input.
                        posb = g.possibleMoves(pieceCoord);
                    }
                } while (!isValidCoord(posb[0]));

                ui.println("Where do you want to move it to?");
                ui.printMoves(posb);
                Coord to;
                bool isPossible = false;
                // Validating user coordinate input. Must be a valid coordinate and be in the possible moves array.
                do
                {
                    input = ui.getInput();
                    to = ui.translateCoordinates(input);
                    if (!isValidCoord(to))
                        continue;
                    for (int i = 0; i < 27; i++)
                    {
                        if (!isValidCoord(posb[i]))
                            break;

                        if (to == posb[i])
                        {
                            isPossible = true;
                            break;
                        }
                    }
                } while (!isPossible);
                // Though makeMove returns true or false depending on if the move was possible to be done, but doing a check
                // here is redundant due to previous input validations.
                g.makeMove(pieceCoord, to);
            }
        }
        // Found checkmate or stalemate.
        else
        {
            ui.printBoard(g.getBoard(), g.getTurn());
            // Looking if in check.
            if (g.isKingInCheck(g.getTurn()))
            {
                std::string pieceWin = g.getTurn() == Color::WHITE ? "Black" : "White";
                std::string winningText = "The " + pieceWin + " pieces have won by checkmate!";
                ui.println(winningText);
                running = false;
            }
            else
            {
                ui.println("Draw by stalemate!");
            }
        }
    }
}
