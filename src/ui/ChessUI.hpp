#pragma once
#include <array>
#include <iostream>
#include "core/Board.hpp"
#include <cstdlib>

class ChessUI
{
public:
    // Prints the board based on the color orientation.
    void printBoard(const Board &b, Color orientation) const;
    // Wrapper for a getline().
    std::string getInput() const;
    // Matches the input text to a possible piece.
    // Returns a Piece with a PieceType::ERROR if no matches are possible.
    Piece translatePiece( std::string p) const;
    // Matches the input text to a possible coordinate on the board.
    // Returns an Out of Bounds piece {8, 8} if no matches are possible.
    Coord translateCoordinates(std::string coord) const;
    // Wrapper for a simple cout statement with an endl.
    void println(std::string s) const;
    // Prints all possible moves inside a Coord[27] array.
    void printMoves(const std::array<Coord, 27> &posb) const;
    // Clear Screen.
    void cls() const;

private:
    // Converts a coordinate into algebraic chess form.
    // Returns an empty string if the given coordinates are invalid.
    std::string convertCoordToText(Coord c) const;
};