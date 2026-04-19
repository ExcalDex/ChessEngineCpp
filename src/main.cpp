#include "core/Game.hpp"
#include "core/Board.hpp"
#include "core/Common.hpp"
#include "ui/ChessUI.hpp"
#include "core/Engine.hpp"
#include <array>

int main()
{
    Game g;
    ChessUI ui;
    Engine engine(Color::BLACK);
    bool running = true;
    while (running)
    {
        // ui.cls(); Uncomment before pushing! (Commented because CLion terminal does not support it very well)
        ui.printBoard(g.getBoard(), g.getTurn());
        if (g.hasMoves(g.getTurn()))
        {
            if (g.getTurn() == Color::BLACK)
            {
                std::array<Coord, 2> bestMove = engine.getBestMove(g);
                g.makeMove(bestMove[0], bestMove[1]);
            }
            else
            {
                ui.println("What piece do you want to move?");
                std::string input;
                Coord pieceCoord = {8, 8};
                std::array<Coord, 27> posb;
                do
                {
                    input = ui.getInput();
                    Piece piece = ui.translatePiece(input);
                    if (piece.t != PieceType::ERROR)
                    {
                        pieceCoord = g.getBoard().getCoordinates(piece);
                        posb = g.possibleMoves(pieceCoord);
                    }
                } while (!isValidCoord(posb[0]));
                ui.println("Where do you want to move it to?");
                ui.printMoves(posb);
                Coord to;
                bool isPossible = false;
                do
                {
                    input = ui.getInput();
                    to = ui.translateCoordinates(input);
                    if (isValidCoord(to))
                    {
                        for (int i = 0; i < 27; i++)
                        {
                            if (posb[i] != Coord{8, 8})
                            {
                                if (to == posb[i])
                                {
                                    isPossible = true;
                                    break;
                                }
                            }
                            else
                            {
                                break;
                            }
                        }
                    }
                } while (!isPossible);

                g.makeMove(pieceCoord, to);
            }
        }
        else
        {
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
