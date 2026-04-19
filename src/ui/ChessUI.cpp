#include <array>
#include <iostream>
#include "ChessUI.hpp"
#include "core/Board.hpp"

void ChessUI::printBoard(const Board &b, Color orientation) const
{
    for (int i = 0; i < 8; i++)
    {
        // Offsets the row rendering based on the orientation color
        const int row = orientation == Color::WHITE ? i : (7 - i);
        // Printing the number of the row, to help the user find the coordinates
        const int numberOfRow = orientation == Color::WHITE ? (7 - i) : i;
        std::cout << numberOfRow + 1 << " ";
        for (int j = 0; j < 8; j++)
        {
            // Offsets the collumn rendering based on the orientation color
            int col = orientation == Color::WHITE ? j : (7 - j);
            Piece p = b.getPiece({row, col});
            std::string piecePrint;
            switch (p.c)
            {
            case Color::WHITE:
                piecePrint.append("W");
                break;

            case Color::BLACK:
                piecePrint.append("B");
                break;

            default:
                break;
            }

            switch (p.t)
            {
            case PieceType::PAWN:
                piecePrint.append("P");
                break;

            case PieceType::ROOK:
                piecePrint.append("R");
                break;

            case PieceType::KNIGHT:
                piecePrint.append("N");
                break;

            case PieceType::BISHOP:
                piecePrint.append("B");
                break;

            case PieceType::QUEEN:
                piecePrint.append("Q");
                break;

            case PieceType::KING:
                piecePrint.append("K");
                break;

            case PieceType::BLANK:
                piecePrint = "   ";
                break;

            default:
                break;
            }

            switch (p.id)
            {
            case 'b':
                break;

            default:
                piecePrint += p.id;
                break;
            }

            std::cout << "[" << piecePrint << "]";
        }
        std::cout << std::endl;
    }
    // Adds extra row to help the user find the coordinates
    std::cout << "  ";
    for (int j = 0; j < 8; j++)
    {
        int col = orientation == Color::WHITE ? j : (7 - j);
        std::cout << "  " << static_cast<char>('a' + col) << "  ";
    }
    std::cout << std::endl;
}

std::string ChessUI::getInput() const
{
    std::string input = "";
    do
    {
        getline(std::cin, input);
    } while (input == "");

    return input;
}

Piece ChessUI::translatePiece(std::string p) const
{
    const Piece error = Piece{PieceType::ERROR};
    if (p.length() != 3)
        return error;

    Piece piece;

    switch (p.at(0))
    {
    case 'W':
        piece.c = Color::WHITE;
        break;

    case 'B':
        piece.c = Color::BLACK;
        break;

    default:
        return error;
    }

    switch (p.at(1))
    {
    case 'P':
        piece.t = PieceType::PAWN;
        break;

    case 'R':
        piece.t = PieceType::ROOK;
        break;

    case 'N':
        piece.t = PieceType::KNIGHT;
        break;

    case 'B':
        piece.t = PieceType::BISHOP;
        break;

    case 'Q':
        piece.t = PieceType::QUEEN;
        break;

    case 'K':
        piece.t = PieceType::KING;
        break;

    default:
        return error;
    }

    if (p.at(2) < '0' || p.at(2) > '8')
        return error;
    piece.id = p.at(2);

    return piece;
}

Coord ChessUI::translateCoordinates(std::string coord) const
{
    Coord error = {8, 8};
    if (coord.length() != 2)
        return error;

    int col = coord.at(0) - 'a';
    if (col < 0 || col > 7)
        return error;

    int row = '8' - coord.at(1);
    if (row < 0 || row > 7)
        return error;

    return Coord{row, col};
}

void ChessUI::println(std::string s) const
{
    std::cout << s << std::endl;
}

std::string ChessUI::convertCoordToText(Coord c) const
{
    std::string coordText;
    if (isValidCoord(c))
    {
        coordText += static_cast<char>(c.col + 'a');
        coordText += static_cast<char>('8' - c.row);
    }
    return coordText;
}

void ChessUI::printMoves(const std::array<Coord, 27> &posb) const
{
    std::cout << "Possible moves: ";
    for (int i = 0; i < 27; i++)
    {
        if (isValidCoord(posb[i]))
        {
            std::cout << convertCoordToText(posb[i]) << " ";
        }
        else
        {
            if (i > 0)
                std::cout << std::endl;
            return;
        }
    }
}

void ChessUI::cls() const
{
    system("cls");
}