#include <algorithm>

#include "core/Game.hpp"
#include "core/Board.hpp"
#include "core/Common.hpp"
#include "ui/ChessUI.hpp"
#include "core/Engine.hpp"
#include <array>
#include <iostream>
#include <string>
#include <cctype>

// Code by André Santos.

// Transforms a board into a FEN chess notation string.
// Easier to use for tests, rather than accessing a matrix.
std::string FEN_notation_generator(const SnapShot &snap)
{
    std::string FEN;
    // Board conversion.
    Board b;
    b.setMatrix(snap.board);
    for (int row = 0; row < 8; row++)
    {
        int consecutiveBlank = 0;
        for (int col = 0; col < 8; col++)
        {
            Piece piece = b.getPiece(Coord{row, col});
            if (piece.t == PieceType::BLANK)
            {
                consecutiveBlank++;
                continue;
            }
            if (consecutiveBlank > 0)
            {
                char numberOfBlanks = '0' + consecutiveBlank;
                FEN += numberOfBlanks;
                consecutiveBlank = 0;
            }
            char pieceType = ' ';
            switch (piece.t)
            {
                case PieceType::PAWN:
                    pieceType = 'P';
                    break;
                case PieceType::KNIGHT:
                    pieceType = 'N';
                    break;
                case PieceType::ROOK:
                    pieceType = 'R';
                    break;
                case PieceType::BISHOP:
                    pieceType = 'B';
                    break;
                case PieceType::QUEEN:
                    pieceType = 'Q';
                    break;
                case PieceType::KING:
                    pieceType = 'K';
                    break;
                default:
                    break;
            }
            if (piece.c == Color::BLACK)
                pieceType = tolower(pieceType);
            FEN += pieceType;
        }
        if (consecutiveBlank > 0)
        {
            char numberOfBlanks = '0' + consecutiveBlank;
            FEN += numberOfBlanks;
        }
        if (row < 7)
            FEN += '/';
    }
    // Turn conversion.
    FEN += snap.state.turn == Color::WHITE ? " w " : " b ";

    // King side and Queen side castling check for both colors.
    if (snap.state.castling[1][0])
        FEN += "K";
    if (snap.state.castling[0][0])
        FEN += "Q";
    if (snap.state.castling[1][1])
        FEN += "k";
    if (snap.state.castling[0][1])
        FEN += "q";
    if (!snap.state.castling[0][0] && !snap.state.castling[1][0] && !snap.state.castling[1][1] && !snap.state.castling[0][1])
        FEN += " -";

    // En Passant conversion
    if (isValidCoord(snap.state.enPassant))
    {
        ChessUI ui;
        FEN += ' ' + ui.convertCoordToText(snap.state.enPassant);
    }
    else
    {
        FEN += " -";
    }

    return FEN;
}

// Returns the piece that matches the FEN char. Will always have id = 0.
Piece matchCharToPiece(char charPiece)
{
    Piece p = {PieceType::BLANK, Color::BLACK, '0'};
    if (std::islower(charPiece))
    {
        switch (charPiece)
        {
            case 'p':
                p.t = PieceType::PAWN;
                break;
            case 'r':
                p.t = PieceType::ROOK;
                break;
            case 'n':
                p.t = PieceType::KNIGHT;
                break;
            case 'b':
                p.t = PieceType::BISHOP;
                break;
            case 'q':
                p.t = PieceType::QUEEN;
                break;
            case 'k':
                p.t = PieceType::KING;
                break;
            default:
                break;
        }
    }
    else
    {
        p.c = Color::WHITE;
        switch (charPiece)
        {
            case 'P':
                p.t = PieceType::PAWN;
                break;
            case 'R':
                p.t = PieceType::ROOK;
                break;
            case 'N':
                p.t = PieceType::KNIGHT;
                break;
            case 'B':
                p.t = PieceType::BISHOP;
                break;
            case 'Q':
                p.t = PieceType::QUEEN;
                break;
            case 'K':
                p.t = PieceType::KING;
                break;
            default:
                break;
        }
    }
    return p;
}

// Converts a FEN string into a Piece matrix.
// Parses only the piece placement part of FEN (before the first space).
// Assigns all pieces default ID '0'.
std::array<std::array<Piece, 8>, 8> FEN_to_matrix(std::string fen)
{
    std::array<std::array<Piece, 8>, 8> matrix;
    Piece blank = {PieceType::BLANK, Color::BLACK, 'b'};

    int row = 0, col = 0;

    for (char c: fen)
    {
        if (c == '/')
        {
            row++;
            col = 0;
            continue;
        }

        if (std::isdigit(c))
        {
            int emptyCount = c - '0';
            for (int i = 0; i < emptyCount; i++)
            {
                matrix[row][col] = blank;
                col++;
            }
        }
        else
        {
            matrix[row][col] = matchCharToPiece(c);
            col++;
        }
    }

    return matrix;

    return matrix;
}

// Helper function to validate the array of possible moves the Game class returns.
// ASSUMES the number of valid moves in the `moves` array matches the size of the `expected` array.
// Returns true when the generated moves match the expected ones.
// Returns false otherwise.
template<size_t N>
bool validateMoves(const std::array<Coord, 27> &moves, const std::array<Coord, N> &expected)
{
    int countMoves = 0;
    for (int i = 0; i < 27; i++)
    {
        if (isValidCoord(moves[i]))
            countMoves++;
    }
    if (countMoves != N)
    {
        return false;
    }

    for (int i = 0; i < N; i++)
    {
        bool foundMatch = false;
        for (int j = 0; j < 27; j++)
        {
            if (moves[j] == expected[i])
            {
                foundMatch = true;
                break;
            }
        }
        if (!foundMatch)
            return false;
    }

    return true;
}

// Helper function to be able to see the moves generated with g.possibleMoves() better.
// Used only while building the test routine, for debugging. Isn't actually used for the test units.
void viewMoves(const std::array<Coord, 27> &posb)
{
    for (int i = 0; i < 27; i++)
    {
        if (posb[i] == Coord{8, 8})
            break;
        std::cout << "Coord{" << posb[i].row << ", " << posb[i].col << "}," << std::endl;
    }
}

int main()
{
    std::cout << "Starting test routine..." << std::endl;
    int ERROR_COUNT = 0;

    constexpr GameState defaultGameState = GameState{Color::WHITE, {{true, true}, {true, true}}, Coord{8, 8}};
    constexpr Coord defaultBlackKing = Coord{0, 4};
    constexpr Coord defaultWhiteKing = Coord{7, 4};

    std::cout << "Common Operations test routine:" << std::endl;
    std::cout << "    Coordinate validation basic function:" << std::endl;
    std::array<Coord, 8> commonCoordTest = {
            Coord{1, 2}, Coord{0, 0}, // Success cases
            Coord{8, 8}, Coord{-1, -1}, Coord{8, 0}, Coord{0, 8}, Coord{-1, 0}, Coord{0, -1} // Error Cases
            };
    for (int i = 0; i < 2; i++)
    {
        if (isValidCoord(commonCoordTest[i]))
        {
            std::cout << "        SUCCESS on case " << i + 1 << '.' << std::endl;
        }
        else
        {
            std::cout << "        ERROR on case " << i + 1 << '!' << std::endl;
            ERROR_COUNT++;
        }
    }
    for (int i = 2; i < 8; i++)
    {
        if (!isValidCoord(commonCoordTest[i]))
        {
            std::cout << "        SUCCESS on case " << i + 1 << '.' << std::endl;
        }
        else
        {
            std::cout << "        ERROR on case " << i + 1 << '!' << std::endl;
            ERROR_COUNT++;
        }
    }
    std::cout << "    Coordinate equality and inequality operators:" << std::endl;
    for (int i = 0; i < 8; i++)
    {
        if (commonCoordTest[i] == commonCoordTest[i])
        {
            std::cout << "        SUCCESS on case " << i + 1 << '.' << std::endl;
        }
        else
        {
            std::cout << "        ERROR on case " << i + 1 << '!' << std::endl;
            ERROR_COUNT++;
        }
    }
    for (int i = 0; i < 8; i++)
    {
        if (commonCoordTest[i] != Coord{9, 9})
        {
            std::cout << "        SUCCESS on case " << i + 9 << '.' << std::endl;
        }
        else
        {
            std::cout << "        ERROR on case " << i + 9 << '!' << std::endl;
            ERROR_COUNT++;
        }
    }

    std::cout << "    Piece equality operator:" << std::endl;
    std::array<Piece, 4> pieceComparisons = {
            Piece{PieceType::PAWN, Color::WHITE, '1'},
            Piece{PieceType::PAWN, Color::WHITE, '1'},
            Piece{PieceType::PAWN, Color::WHITE, '2'},
            Piece{PieceType::ROOK, Color::WHITE, '1'}
            };

    if (pieceComparisons[0] == pieceComparisons[1])
    {
        std::cout << "        SUCCESS on case 1" << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 1" << std::endl;
    }

    if (pieceComparisons[0] == pieceComparisons[2])
    {
        std::cout << "        ERROR on case 2" << std::endl;
    }
    else
    {
        std::cout << "        SUCCESS on case 2" << std::endl;
    }

    if (pieceComparisons[0] == pieceComparisons[3])
    {
        std::cout << "        ERROR on case 3" << std::endl;
    }
    else
    {
        std::cout << "        SUCCESS on case 3" << std::endl;
    }

    // Testing ChessUI class core functionalities.
    std::cout << "UI test routine:" << std::endl;
    ChessUI ui;
    std::cout << "    UI instance initialized." << std::endl;
    std::cout << "    Text to Piece transformation:" << std::endl;
    // Ids higher than 0 are permitted for all pieces, even kings, so WK8 isn't supposed to return an error.
    std::array<std::string, 10> strPieces = {
            "WP1", "BP1", "WR3", "BQ0", "WK8", // Success cases
            "wp1", "WP9", "YP1", "BC0", "FE9" // Error cases
            };
    constexpr std::array<const Piece, 5> pieceTestCases = {
            Piece{PieceType::PAWN, Color::WHITE, '1'},
            Piece{PieceType::PAWN, Color::BLACK, '1'},
            Piece{PieceType::ROOK, Color::WHITE, '3'},
            Piece{PieceType::QUEEN, Color::BLACK, '0'},
            Piece{PieceType::KING, Color::WHITE, '8'},
            };
    constexpr Piece perror = Piece{PieceType::ERROR};

    for (int i = 0; i < 5; i++)
    {
        const Piece p = ui.translatePiece(strPieces[i]);
        if (p == pieceTestCases[i])
        {
            std::cout << "        SUCCESS on case " << i + 1 << '.' << std::endl;
        }
        else
        {
            std::cout << "        ERROR on case " << i + 1 << '!' << std::endl;
            ERROR_COUNT++;
        }
    }
    for (int i = 5; i < 10; i++)
    {
        const Piece p = ui.translatePiece(strPieces[i]);
        if (p == perror)
        {
            std::cout << "        SUCCESS on case " << i + 1 << '.' << std::endl;
        }
        else
        {
            std::cout << "        ERROR on case " << i + 1 << '!' << std::endl;
            ERROR_COUNT++;
        }
    }

    std::cout << "    Text to Coordinate transformation:" << std::endl;

    const std::array<std::string, 10> strCoordinates = {
            "a1", "h8", "a8", "e5", "d4", // Success cases
            "a0", "h9", "i1", "i9", "i0" // Error cases
            };
    constexpr std::array<const Coord, 5> coordinatesTestCases = {
            Coord{7, 0},
            Coord{0, 7},
            Coord{0, 0},
            Coord{3, 4},
            Coord{4, 3}
            };
    constexpr Coord cerror = Coord{8, 8};

    for (int i = 0; i < 5; i++)
    {
        const Coord c = ui.translateCoordinates(strCoordinates[i]);
        if (c == coordinatesTestCases[i])
        {
            std::cout << "        SUCCESS on case " << i + 1 << '.' << std::endl;
        }
        else
        {
            std::cout << "        ERROR on case " << i + 1 << '!' << std::endl;
            ERROR_COUNT++;
        }
    }

    for (int i = 5; i < 10; ++i)
    {
        const Coord c = ui.translateCoordinates(strCoordinates[i]);
        if (c == cerror)
        {
            std::cout << "        SUCCESS on case " << i + 1 << '.' << std::endl;
        }
        else
        {
            std::cout << "        ERROR on case " << i + 1 << '!' << std::endl;
            ERROR_COUNT++;
        }
    }

    std::cout << "    Coordinate to Text transformation:" << std::endl;
    constexpr std::array<Coord, 11> coordinates = {
            Coord{7, 0}, Coord{0, 7}, Coord{0, 0}, Coord{3, 4}, Coord{4, 3}, // Success Cases
            Coord{-1, 0}, Coord{0, -1}, Coord{8, 0}, Coord{0, 8}, Coord{-1, -1}, Coord{8, 8} // Error Cases
            };
    const std::array<std::string, 5> strCoordinatesTestCases = {"a1", "h8", "a8", "e5", "d4"};
    const std::string strcerror = "ER";
    for (int i = 0; i < 5; i++)
    {
        std::string s = ui.convertCoordToText(coordinates[i]);
        if (s == strCoordinatesTestCases[i])
        {
            std::cout << "        SUCCESS on case " << i + 1 << '.' << std::endl;
        }
        else
        {
            std::cout << "        ERROR on case " << i + 1 << '!' << std::endl;
            ERROR_COUNT++;
        }
    }

    for (int i = 5; i < 11; ++i)
    {
        std::string s = ui.convertCoordToText(coordinates[i]);
        if (s == strcerror)
        {
            std::cout << "        SUCCESS on case " << i + 1 << '.' << std::endl;
        }
        else
        {
            std::cout << "        ERROR on case " << i + 1 << '!' << std::endl;
            ERROR_COUNT++;
        }
    }

    std::cout << "    Piece to Text transformation:" << std::endl;
    //
    constexpr std::array<Piece, 8> pieces = {
            Piece{PieceType::BLANK}, Piece{PieceType::PAWN, Color::WHITE, '1'}, // Success Cases
            Piece{PieceType::ROOK, Color::WHITE, '0'}, Piece{PieceType::KNIGHT, Color::BLACK, 'a'}, // Success Cases
            Piece{PieceType::QUEEN, Color::WHITE, 'q'}, Piece{PieceType::BISHOP}, Piece{PieceType::KING, Color::BLACK, '-'}, // Success Cases
            Piece{PieceType::ERROR} // Error Cases
            };
    const std::array<std::string, 7> strPiecesTestCases = {"   ", "WP1", "WR0", "BNa", "WQq", "BBb", "BK-"};
    const std::string strperror = "ERR";

    for (int i = 0; i < 7; i++)
    {
        std::string s = ui.convertPieceToText(pieces[i]);
        if (s == strPiecesTestCases[i])
        {
            std::cout << "        SUCCESS on case " << i + 1 << '.' << std::endl;
        }
        else
        {
            std::cout << "        ERROR on case " << i + 1 << '!' << std::endl;
            ERROR_COUNT++;
        }
    }

    if (ui.convertPieceToText(pieces[7]) == strperror)
    {
        std::cout << "        SUCCESS on case " << 8 << '.' << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case " << 8 << '!' << std::endl;
        ERROR_COUNT++;
    }

    std::cout << "Board test routine:" << std::endl;
    Board b;
    std::cout << "    Board instance initialized." << std::endl;
    std::cout << "    All blank initialization:" << std::endl;
    // ASSUMES b.getPiece() works. It's a get method that simply accesses the given coordinates in the array and returns the value found.
    bool foundNotBlank = false;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (b.getPiece(Coord{i, j}).t != PieceType::BLANK)
            {
                std::cout << "        ERROR on coordinate {row:" << i << " column:" << j << "}! Not a blank piece!" << std::endl;
                ERROR_COUNT++;
                foundNotBlank = true;
            }
        }
    }
    if (!foundNotBlank)
        std::cout << "        Blank squares correctly initialized." << std::endl;

    std::cout << "    Set Matrix:" << std::endl;

    std::array<std::array<Piece, 8>, 8> matrixSetText;

    // Setting matrix by hand because FEN creator does not support different IDs.
    // Different IDs are crucial to some of the following tests.

    // Row 0 (White back rank)
    matrixSetText[7][0] = Piece{PieceType::ROOK, Color::WHITE, '1'};
    matrixSetText[7][1] = Piece{PieceType::KNIGHT, Color::WHITE, '1'};
    matrixSetText[7][2] = Piece{PieceType::BISHOP, Color::WHITE, '1'};
    matrixSetText[7][3] = Piece{PieceType::QUEEN, Color::WHITE, '0'};
    matrixSetText[7][4] = Piece{PieceType::KING, Color::WHITE, '0'};
    matrixSetText[7][5] = Piece{PieceType::BISHOP, Color::WHITE, '2'};
    matrixSetText[7][6] = Piece{PieceType::KNIGHT, Color::WHITE, '2'};
    matrixSetText[7][7] = Piece{PieceType::ROOK, Color::WHITE, '2'};

    // Row 1 (White pawns)
    for (int col = 0; col < 8; col++)
    {
        matrixSetText[6][col] = Piece{PieceType::PAWN, Color::WHITE, static_cast<char>('1' + col)};
    }

    // Rows 5-2 (Empty - defaults are already BLANK)

    // Row 6 (Black pawns)
    for (int col = 0; col < 8; col++)
    {
        matrixSetText[1][col] = Piece{PieceType::PAWN, Color::BLACK, static_cast<char>('1' + col)};
    }

    // Row 7 (Black back rank)
    matrixSetText[0][0] = Piece{PieceType::ROOK, Color::BLACK, '1'};
    matrixSetText[0][1] = Piece{PieceType::KNIGHT, Color::BLACK, '1'};
    matrixSetText[0][2] = Piece{PieceType::BISHOP, Color::BLACK, '1'};
    matrixSetText[0][3] = Piece{PieceType::QUEEN, Color::BLACK, '0'};
    matrixSetText[0][4] = Piece{PieceType::KING, Color::BLACK, '0'};
    matrixSetText[0][5] = Piece{PieceType::BISHOP, Color::BLACK, '2'};
    matrixSetText[0][6] = Piece{PieceType::KNIGHT, Color::BLACK, '2'};
    matrixSetText[0][7] = Piece{PieceType::ROOK, Color::BLACK, '2'};

    b.setMatrix(matrixSetText);

    // ASSUMES b.snapshot() works. It's a base get method with a different name, returns a copy of the board matrix.
    SnapShot snapMatrixSet = SnapShot{b.snapshot(), defaultGameState, defaultBlackKing, defaultWhiteKing};

    std::string fenSetMatrix = FEN_notation_generator(snapMatrixSet);
    if (fenSetMatrix == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -")
        std::cout << "        Successfully copied the generated matrix." << std::endl;
    else
    {
        std::cout << "        Error! Matrix not copied correctly." << std::endl;
        ERROR_COUNT++;
    }


    std::cout << "    Find coordinate of piece:" << std::endl;

    std::array<Piece, 19> findPiece =
            {
            Piece{PieceType::ROOK, Color::BLACK, '1'}, // Success cases
            Piece{PieceType::KNIGHT, Color::BLACK, '1'}, // Success cases
            Piece{PieceType::BISHOP, Color::BLACK, '1'}, // Success cases
            Piece{PieceType::QUEEN, Color::BLACK, '0'}, // Success cases
            Piece{PieceType::KING, Color::BLACK, '0'}, // Success cases
            Piece{PieceType::BISHOP, Color::BLACK, '2'}, // Success cases
            Piece{PieceType::KNIGHT, Color::BLACK, '2'}, // Success cases
            Piece{PieceType::PAWN, Color::BLACK, '1'}, // Success cases
            Piece{PieceType::ROOK, Color::WHITE, '1'}, // Success cases
            Piece{PieceType::KNIGHT, Color::WHITE, '1'}, // Success cases
            Piece{PieceType::BISHOP, Color::WHITE, '1'}, // Success cases
            Piece{PieceType::QUEEN, Color::WHITE, '0'}, // Success cases
            Piece{PieceType::KING, Color::WHITE, '0'}, // Success cases
            Piece{PieceType::BISHOP, Color::WHITE, '2'}, // Success cases
            Piece{PieceType::KNIGHT, Color::WHITE, '2'}, // Success cases
            Piece{PieceType::PAWN, Color::WHITE, '1'}, // Success cases
            Piece{PieceType::ROOK, Color::WHITE, '4'}, // Error cases
            Piece{PieceType::ERROR, Color::WHITE, '0'}, // Error cases
            Piece{PieceType::BLANK, Color::BLACK, 'b'} // Error cases
            };
    std::array<Coord, 16> findPieceTestCases =
            {
            Coord{0, 0},
            Coord{0, 1},
            Coord{0, 2},
            Coord{0, 3},
            Coord{0, 4},
            Coord{0, 5},
            Coord{0, 6},
            Coord{1, 0},
            Coord{7, 0},
            Coord{7, 1},
            Coord{7, 2},
            Coord{7, 3},
            Coord{7, 4},
            Coord{7, 5},
            Coord{7, 6},
            Coord{6, 0},
            };

    for (int i = 0; i < 16; i++)
    {
        Coord c = b.getCoordinates(findPiece[i]);
        if (c == findPieceTestCases[i])
        {
            std::cout << "        SUCCESS on case " << i + 1 << '.' << std::endl;
        }
        else
        {
            std::cout << "        ERROR on case " << i + 1 << '!' << std::endl;
            ERROR_COUNT++;
        }
    }

    for (int i = 16; i < 19; i++)
    {
        Coord c = b.getCoordinates(findPiece[i]);
        if (c == cerror)
        {
            std::cout << "        SUCCESS on case " << i + 1 << '.' << std::endl;
        }
        else
        {
            std::cout << "        ERROR on case " << i + 1 << '!' << std::endl;
            ERROR_COUNT++;
        }
    }

    std::cout << "    Move piece:" << std::endl;
    // Making sure basic matrix is set.
    b.setMatrix(FEN_to_matrix("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"));
    // Moving e2 pawn to e4.
    b.move(Coord{6, 4}, Coord{4, 4});

    SnapShot movingOnBoard;

    GameState gameSMoveOnBoard = defaultGameState;
    gameSMoveOnBoard.enPassant = Coord{5, 4};
    gameSMoveOnBoard.turn = Color::BLACK;

    movingOnBoard.board = b.snapshot();
    movingOnBoard.state = gameSMoveOnBoard;

    movingOnBoard.blackKing = defaultBlackKing;
    movingOnBoard.whiteKing = defaultWhiteKing;

    std::string fenMove = FEN_notation_generator(movingOnBoard);
    if (fenMove == "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3")
    {
        std::cout << "        SUCCESS on case 1." << std::endl;
    }
    else
    {
        std::cout << "        ERROR unexpected movement behavior on case 1." << std::endl;
        ERROR_COUNT++;
    }

    // Undoing move.
    b.move(Coord{4, 4}, Coord{6, 4});
    movingOnBoard.board = b.snapshot();
    movingOnBoard.state = defaultGameState;
    fenMove = FEN_notation_generator(movingOnBoard);
    if (fenMove == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -")
    {
        std::cout << "        SUCCESS on case 2." << std::endl;
    }
    else
    {
        std::cout << "        ERROR unexpected movement behavior on case 2." << std::endl;
        ERROR_COUNT++;
    }

    // Making sure basic matrix is set.
    b.setMatrix(FEN_to_matrix("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"));
    // Moving g1 knight to f3.
    b.move(Coord{7, 6}, Coord{1, 7});
    movingOnBoard.board = b.snapshot();
    movingOnBoard.state = defaultGameState;
    movingOnBoard.state.turn = Color::BLACK;
    fenMove = FEN_notation_generator(movingOnBoard);
    if (fenMove == "rnbqkbnr/pppppppN/8/8/8/8/PPPPPPPP/RNBQKB1R b KQkq -")
    {
        std::cout << "        SUCCESS on case 3." << std::endl;
    }
    else
    {
        std::cout << "        ERROR unexpected movement behavior on case 3." << std::endl;
        ERROR_COUNT++;
    }

    // Undoing move.
    b.move(Coord{1, 7}, Coord{7, 6});
    movingOnBoard.board = b.snapshot();
    movingOnBoard.state = defaultGameState;
    fenMove = FEN_notation_generator(movingOnBoard);
    if (fenMove == "rnbqkbnr/ppppppp1/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -")
    {
        std::cout << "        SUCCESS on case 4." << std::endl;
    }
    else
    {
        std::cout << "        ERROR unexpected movement behavior on case 4." << std::endl;
        ERROR_COUNT++;
    }

    std::cout << "Game test routine:" << std::endl;
    Game g;
    std::cout << "    Game instance initialized." << std::endl;
    std::cout << "    SnapShot setter:" << std::endl;
    SnapShot testSnap;
    testSnap.state = defaultGameState;
    testSnap.board = FEN_to_matrix("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    g.revertState(testSnap);
    if (FEN_notation_generator(g.getSnap()) == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -")
    {
        std::cout << "        SUCCESS on case 1." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 1." << std::endl;
        ERROR_COUNT++;
    }

    testSnap.board = FEN_to_matrix("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    testSnap.state.turn = Color::BLACK;
    testSnap.state.enPassant = Coord{5, 4};
    testSnap.board = FEN_to_matrix("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR");
    g.revertState(testSnap);
    if (FEN_notation_generator(g.getSnap()) == "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3")
    {
        std::cout << "        SUCCESS on case 2." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 2." << std::endl;
        ERROR_COUNT++;
    }

    std::cout << "    Pawn movement generator:" << std::endl;
    testSnap.state = defaultGameState;
    testSnap.blackKing = Coord{8, 8};
    testSnap.whiteKing = Coord{8, 8};
    // Pawn Routine
    // Test for single movement in row.
    // White
    testSnap.board = FEN_to_matrix("8/8/8/8/4P3/8/8/8");
    testSnap.state.turn = Color::WHITE;
    g.revertState(testSnap);

    std::array<Coord, 1> singleMovePawn = {Coord{3, 4}};
    if (validateMoves(g.possibleMoves(Coord{4, 4}), singleMovePawn))
    {
        std::cout << "        SUCCESS on case 1." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 1." << std::endl;
        ERROR_COUNT++;
    }

    // Black
    testSnap.board = FEN_to_matrix("8/8/8/3p4/8/8/8/8");
    testSnap.state.turn = Color::BLACK;
    g.revertState(testSnap);

    singleMovePawn[0] = Coord{4, 3};
    if (validateMoves(g.possibleMoves(Coord{3, 3}), singleMovePawn))
    {
        std::cout << "        SUCCESS on case 2." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 2." << std::endl;
        ERROR_COUNT++;
    }

    // Test for double movement in row.
    // White
    testSnap.board = FEN_to_matrix("8/8/8/8/8/8/4P3/8");
    testSnap.state.turn = Color::WHITE;
    g.revertState(testSnap);
    std::array<Coord, 2> doubleMovePawn = {Coord{5, 4}, Coord{4, 4}};
    if (validateMoves(g.possibleMoves(Coord{6, 4}), doubleMovePawn))
    {
        std::cout << "        SUCCESS on case 3." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 3." << std::endl;
        ERROR_COUNT++;
    }
    // Black
    testSnap.board = FEN_to_matrix("8/4p3/8/8/8/8/8/8");
    testSnap.state.turn = Color::BLACK;
    doubleMovePawn = {Coord{2, 4}, Coord{3, 4}};
    g.revertState(testSnap);
    if (validateMoves(g.possibleMoves(Coord{1, 4}), doubleMovePawn))
    {
        std::cout << "        SUCCESS on case 4." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 4." << std::endl;
        ERROR_COUNT++;
    }

    // Test for capture in diagonal.
    // White
    testSnap.board = FEN_to_matrix("8/8/8/2p1p3/3P4/8/8/8");
    testSnap.state.turn = Color::WHITE;
    g.revertState(testSnap);
    std::array<Coord, 3> tripleMovePawn = {Coord{3, 2}, Coord{3, 3}, Coord{3, 4}};
    if (validateMoves(g.possibleMoves(Coord{4, 3}), tripleMovePawn))
    {
        std::cout << "        SUCCESS on case 5." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 5." << std::endl;
        ERROR_COUNT++;
    }
    // Black
    testSnap.board = FEN_to_matrix("8/8/8/3p4/2P1P3/8/8/8");
    testSnap.state.turn = Color::BLACK;
    g.revertState(testSnap);
    tripleMovePawn = {Coord{4, 2}, Coord{4, 3}, Coord{4, 4}};
    if (validateMoves(g.possibleMoves(Coord{3, 3}), tripleMovePawn))
    {
        std::cout << "        SUCCESS on case 6." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 6." << std::endl;
        ERROR_COUNT++;
    }

    // Test for En Passant.
    // White
    testSnap.board = FEN_to_matrix("8/8/8/3pP3/8/8/8/8");
    testSnap.state.turn = Color::WHITE;
    testSnap.state.enPassant = Coord{2, 3};
    g.revertState(testSnap);
    doubleMovePawn = {Coord{2, 4}, Coord{2, 3}};
    if (validateMoves(g.possibleMoves(Coord{3, 4}), doubleMovePawn))
    {
        std::cout << "        SUCCESS on case 7." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 7." << std::endl;
        ERROR_COUNT++;
    }
    // Black
    testSnap.board = FEN_to_matrix("8/8/8/8/3pP3/8/8/8");
    testSnap.state.turn = Color::BLACK;
    testSnap.state.enPassant = Coord{5, 4};
    g.revertState(testSnap);
    doubleMovePawn = {Coord{5, 3}, Coord{5, 4}};
    if (validateMoves(g.possibleMoves(Coord{4, 3}), doubleMovePawn))
    {
        std::cout << "        SUCCESS on case 8." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 8." << std::endl;
        ERROR_COUNT++;
    }

    // Knight Routine
    // White
    std::cout << "    Knight movement generator:" << std::endl;
    testSnap.state = defaultGameState;
    testSnap.board = FEN_to_matrix("N6N/8/8/8/3N4/8/8/N6N");
    testSnap.state.turn = Color::WHITE;
    g.revertState(testSnap);
    constexpr Coord upperLeftCorner = {0, 0};
    constexpr Coord upperRightCorner = {0, 7};
    constexpr Coord lowerLeftCorner = {7, 0};
    constexpr Coord lowerRightCorner = {7, 7};

    std::array<Coord, 2> expectedWhiteKnightCorners = {Coord{2, 1}, Coord{1, 2}};
    if (validateMoves(g.possibleMoves(upperLeftCorner), expectedWhiteKnightCorners))
    {
        std::cout << "        SUCCESS on case 1." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 1." << std::endl;
        ERROR_COUNT++;
    }
    expectedWhiteKnightCorners = {Coord{2, 6}, Coord{1, 5}};
    if (validateMoves(g.possibleMoves(upperRightCorner), expectedWhiteKnightCorners))
    {
        std::cout << "        SUCCESS on case 2." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 2." << std::endl;
        ERROR_COUNT++;
    }

    expectedWhiteKnightCorners = {Coord{5, 1}, Coord{6, 2}};
    if (validateMoves(g.possibleMoves(lowerLeftCorner), expectedWhiteKnightCorners))
    {
        std::cout << "        SUCCESS on case 3." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 3." << std::endl;
        ERROR_COUNT++;
    }

    expectedWhiteKnightCorners = {Coord{5, 6}, Coord{6, 5}};
    if (validateMoves(g.possibleMoves(lowerRightCorner), expectedWhiteKnightCorners))
    {
        std::cout << "        SUCCESS on case 4." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 4." << std::endl;
        ERROR_COUNT++;
    }

    std::array<Coord, 8> expectedWhiteKnightCenter = {
            Coord{2, 4}, Coord{6, 4},
            Coord{2, 2}, Coord{6, 2},
            Coord{5, 1}, Coord{5, 5},
            Coord{3, 1}, Coord{3, 5}
            };
    if (validateMoves(g.possibleMoves(Coord{4, 3}), expectedWhiteKnightCenter))
    {
        std::cout << "        SUCCESS on case 5." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 5." << std::endl;
        ERROR_COUNT++;
    }

    // Rook Routine
    // White
    std::cout << "    Rook movement generator:" << std::endl;
    testSnap.state = defaultGameState;
    testSnap.board = FEN_to_matrix("4n3/8/8/8/1q2RB2/8/8/4P3");
    testSnap.state.turn = Color::WHITE;
    g.revertState(testSnap);
    std::array<Coord, 9> expectedWhiteRook = {
            Coord{4, 3}, Coord{4, 2}, Coord{4, 1}, // Left
            Coord{3, 4}, Coord{2, 4}, Coord{1, 4}, Coord{0, 4}, // Up
            Coord{5, 4}, Coord{6, 4} // Down
            };
    if (validateMoves(g.possibleMoves(Coord{4, 4}), expectedWhiteRook))
    {
        std::cout << "        SUCCESS on case 1." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 1." << std::endl;
        ERROR_COUNT++;
    }
    // Black
    testSnap.state = defaultGameState;
    testSnap.board = FEN_to_matrix("4N3/8/8/8/1Q2rb2/8/8/4p3");
    testSnap.state.turn = Color::BLACK;
    g.revertState(testSnap);
    std::array<Coord, 9> expectedBlackRook = {
            Coord{4, 3}, Coord{4, 2}, Coord{4, 1}, // Left
            Coord{3, 4}, Coord{2, 4}, Coord{1, 4}, Coord{0, 4}, // Up
            Coord{5, 4}, Coord{6, 4} // Down
            };
    if (validateMoves(g.possibleMoves(Coord{4, 4}), expectedBlackRook))
    {
        std::cout << "        SUCCESS on case 2." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 2." << std::endl;
        ERROR_COUNT++;
    }

    // Bishop Routine
    // White
    std::cout << "    Bishop movement generator:" << std::endl;
    testSnap.state = defaultGameState;
    testSnap.board = FEN_to_matrix("8/8/1p5/4P3/3B4/2p5/8/6P1");
    testSnap.state.turn = Color::WHITE;
    g.revertState(testSnap);
    std::array<Coord, 5> expectedWhiteBishop = {
            Coord{3, 2}, Coord{2, 1}, // North-West
            Coord{5, 4}, Coord{6, 5}, // South-East
            Coord{5, 2} // South-WEST
            };
    if (validateMoves(g.possibleMoves(Coord{4, 3}), expectedWhiteBishop))
    {
        std::cout << "        SUCCESS on case 1." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 1." << std::endl;
        ERROR_COUNT++;
    }
    // Black
    testSnap.state = defaultGameState;
    testSnap.board = FEN_to_matrix("8/8/1P5/4p3/3b4/2P5/8/6p1");
    testSnap.state.turn = Color::BLACK;
    g.revertState(testSnap);
    std::array<Coord, 5> expectedBlackBishop = {
            Coord{3, 2}, Coord{2, 1}, // North-West
            Coord{5, 4}, Coord{6, 5}, // South-East
            Coord{5, 2} // South-WEST
            };
    if (validateMoves(g.possibleMoves(Coord{4, 3}), expectedBlackBishop))
    {
        std::cout << "        SUCCESS on case 2." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 2." << std::endl;
        ERROR_COUNT++;
    }

    // Queen routine
    // White
    std::cout << "    Queen movement generator:" << std::endl;
    testSnap.state = defaultGameState;
    testSnap.board = FEN_to_matrix("8/8/1p5/4P3/3Q4/2p5/8/6P1");
    testSnap.state.turn = Color::WHITE;
    g.revertState(testSnap);
    std::array<Coord, 19> expectedWhiteQueenDiagonal = {
            Coord{5, 3}, Coord{6, 3}, Coord{7, 3},
            Coord{3, 3}, Coord{2, 3}, Coord{1, 3},
            Coord{0, 3}, Coord{4, 4}, Coord{4, 5},
            Coord{4, 6}, Coord{4, 7}, Coord{4, 2},
            Coord{4, 1}, Coord{4, 0}, Coord{5, 4},
            Coord{6, 5}, Coord{5, 2}, Coord{3, 2},
            Coord{2, 1}
            };
    if (validateMoves(g.possibleMoves(Coord{4, 3}), expectedWhiteQueenDiagonal))
    {
        std::cout << "        SUCCESS on case 1." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 1." << std::endl;
        ERROR_COUNT++;
    }

    testSnap.state = defaultGameState;
    testSnap.board = FEN_to_matrix("8/8/1P5/4p3/3q4/2P5/8/6p1");
    testSnap.state.turn = Color::BLACK;
    g.revertState(testSnap);
    std::array<Coord, 19> expectedBlackQueenDiagonal = {
            Coord{5, 3}, Coord{6, 3}, Coord{7, 3},
            Coord{3, 3}, Coord{2, 3}, Coord{1, 3},
            Coord{0, 3}, Coord{4, 4}, Coord{4, 5},
            Coord{4, 6}, Coord{4, 7}, Coord{4, 2},
            Coord{4, 1}, Coord{4, 0}, Coord{5, 4},
            Coord{6, 5}, Coord{5, 2}, Coord{3, 2},
            Coord{2, 1}
            };
    if (validateMoves(g.possibleMoves(Coord{4, 3}), expectedBlackQueenDiagonal))
    {
        std::cout << "        SUCCESS on case 2." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 2." << std::endl;
        ERROR_COUNT++;
    }

    testSnap.state = defaultGameState;
    testSnap.board = FEN_to_matrix("4n3/8/8/8/1q2QB2/8/8/4P3");
    testSnap.state.turn = Color::WHITE;
    g.revertState(testSnap);
    std::array<Coord, 22> expectedWhiteQueenOrthogonal = {
            Coord{5, 4}, Coord{6, 4}, Coord{3, 4}, Coord{2, 4},
            Coord{1, 4}, Coord{0, 4}, Coord{4, 3}, Coord{4, 2},
            Coord{4, 1}, Coord{5, 5}, Coord{6, 6}, Coord{7, 7},
            Coord{3, 5}, Coord{2, 6}, Coord{1, 7}, Coord{5, 3},
            Coord{6, 2}, Coord{7, 1}, Coord{3, 3}, Coord{2, 2},
            Coord{1, 1}, Coord{0, 0}
            };
    if (validateMoves(g.possibleMoves(Coord{4, 4}), expectedWhiteQueenOrthogonal))
    {
        std::cout << "        SUCCESS on case 3." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 3." << std::endl;
        ERROR_COUNT++;
    }

    testSnap.state = defaultGameState;
    testSnap.board = FEN_to_matrix("4N3/8/8/8/1Q2qb2/8/8/4p3");
    testSnap.state.turn = Color::BLACK;
    g.revertState(testSnap);
    std::array<Coord, 22> expectedBlackQueenOrthogonal = {
            Coord{5, 4}, Coord{6, 4}, Coord{3, 4}, Coord{2, 4},
            Coord{1, 4}, Coord{0, 4}, Coord{4, 3}, Coord{4, 2},
            Coord{4, 1}, Coord{5, 5}, Coord{6, 6}, Coord{7, 7},
            Coord{3, 5}, Coord{2, 6}, Coord{1, 7}, Coord{5, 3},
            Coord{6, 2}, Coord{7, 1}, Coord{3, 3}, Coord{2, 2},
            Coord{1, 1}, Coord{0, 0}
            };
    if (validateMoves(g.possibleMoves(Coord{4, 4}), expectedWhiteQueenOrthogonal))
    {
        std::cout << "        SUCCESS on case 4." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 4." << std::endl;
        ERROR_COUNT++;
    }

    // King routine
    // White
    std::cout << "    King movement generator:" << std::endl;
    testSnap.state = defaultGameState;
    testSnap.board = FEN_to_matrix("8/8/8/8/3K4/4P3/8/8");
    testSnap.state.turn = Color::WHITE;
    g.revertState(testSnap);
    std::array<Coord, 9> expectedWhiteKing = {
            Coord{5, 3}, Coord{5, 2}, Coord{3, 3}, Coord{3, 4},
            Coord{3, 2}, Coord{4, 4}, Coord{4, 2}, Coord{4, 1}, Coord{4, 5}
            };
    if (validateMoves(g.possibleMoves(Coord{4, 3}), expectedWhiteKing))
    {
        std::cout << "        SUCCESS on case 1." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 1." << std::endl;
        ERROR_COUNT++;
    }

    testSnap.state = defaultGameState;
    testSnap.board = FEN_to_matrix("8/8/8/8/3k4/4p3/8/8");
    testSnap.state.turn = Color::BLACK;
    g.revertState(testSnap);
    std::array<Coord, 9> expectedBlackKing = {
        Coord{5, 3}, Coord{5, 2}, Coord{3, 3}, Coord{3, 4},
        Coord{3, 2}, Coord{4, 4}, Coord{4, 2}, Coord{4, 1}, Coord{4, 5}
    };
    if (validateMoves(g.possibleMoves(Coord{4, 3}), expectedBlackKing))
    {
        std::cout << "        SUCCESS on case 2." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 2." << std::endl;
        ERROR_COUNT++;
    }

    // Check routine
    std::cout << "    King in check:" << std::endl;
    //  White
    testSnap.state = defaultGameState;
    testSnap.blackKing = Coord{8, 8};
    testSnap.whiteKing = Coord{4, 3};
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            testSnap.state.castling[i][j] = false;
    // Test for false positive
    testSnap.board = FEN_to_matrix("8/8/8/8/3K4/8/8/8");
    g.revertState(testSnap);
    if (!g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 1." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 1." << std::endl;
        ERROR_COUNT++;
    }
    // Rook
    testSnap.board = FEN_to_matrix("8/8/8/8/3K4/8/8/3r4");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 2." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 2." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("8/8/8/8/3K3r/8/8/8");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 3." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 3." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("8/8/8/8/r2K4/8/8/8");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 4." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 4." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("3r4/8/8/8/3K4/8/8/8");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 5." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 5." << std::endl;
        ERROR_COUNT++;
    }
    // Testing for false positive
    testSnap.board = FEN_to_matrix("3r4/8/8/3R4/r1RKR2r/3R4/8/3r4");
    g.revertState(testSnap);
    if (!g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 6." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 6." << std::endl;
        ERROR_COUNT++;
    }
    // Bishop
    testSnap.board = FEN_to_matrix("8/8/8/8/3K4/8/8/6b1");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 7." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 7." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("8/8/8/8/3K4/8/8/b7");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 8." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 8." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("8/b7/8/8/3K4/8/8/8");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 9." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 9." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("8/6b1/8/8/3K4/8/8/8");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 10." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 10." << std::endl;
        ERROR_COUNT++;
    }
    // Testing for false positive
    testSnap.board = FEN_to_matrix("8/b5b1/8/2B1B3/3K4/2B1B3/8/b5b1");
    g.revertState(testSnap);
    if (!g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 11." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 11." << std::endl;
        ERROR_COUNT++;
    }

    // Queen
    testSnap.board = FEN_to_matrix("8/8/8/8/3K4/8/8/3q4");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 12." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 12." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("8/8/8/8/3K3q/8/8/8");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 13." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 13." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("8/8/8/8/q2K4/8/8/8");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 14." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 14." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("3q4/8/8/8/3K4/8/8/8");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 15." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 15." << std::endl;
        ERROR_COUNT++;
    }
    // Testing for false positive
    testSnap.board = FEN_to_matrix("3q4/8/8/3Q4/q1QKQ2q/3Q4/8/3q4");
    g.revertState(testSnap);
    if (!g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 16." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 16." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("8/8/8/8/3K4/8/8/6q1");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 17." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 17." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("8/8/8/8/3K4/8/8/q7");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 18." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 18." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("8/q7/8/8/3K4/8/8/8");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 19." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 19." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("8/6q1/8/8/3K4/8/8/8");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 20." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 20." << std::endl;
        ERROR_COUNT++;
    }
    // Testing for false positive
    testSnap.board = FEN_to_matrix("8/q5q1/8/2Q1Q3/3K4/2Q1Q3/8/q5q1");
    g.revertState(testSnap);
    if (!g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 21." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 21." << std::endl;
        ERROR_COUNT++;
    }
    // Knight
    testSnap.board = FEN_to_matrix("8/8/8/8/3K4/8/4n3/8");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 22." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 22." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("8/8/8/8/3K4/5n2/8/8");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 23." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 23." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("8/8/8/8/3K4/8/2n5/8");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 24." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 24." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("8/8/8/8/3K4/1n6/8/8");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 25." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 25." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("8/8/8/1n6/3K4/8/8/8");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 26." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 26." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("8/8/2n5/8/3K4/8/8/8");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 27." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 27." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("8/8/4n3/8/3K4/8/8/8");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 28." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 28." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("8/8/8/5n2/3K4/8/8/8");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 29." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 29." << std::endl;
        ERROR_COUNT++;
    }
    // Pawn
    testSnap.board = FEN_to_matrix("8/8/8/4p3/3K4/8/8/8");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 30." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 30." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("8/8/8/2p5/3K4/8/8/8");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 31." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 31." << std::endl;
        ERROR_COUNT++;
    }
    // King
    testSnap.board = FEN_to_matrix("8/8/8/8/3K4/4k3/8/8");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 32." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 32." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("8/8/8/8/3K4/3k4/8/8");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 33." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 33." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("8/8/8/8/3K4/2k5/8/8");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 34." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 34." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("8/8/8/8/2kK4/8/8/8");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 35." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 35." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("8/8/8/2k5/3K4/8/8/8");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 36." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 36." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("8/8/8/3k4/3K4/8/8/8");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 37." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 37." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("8/8/8/4k3/3K4/8/8/8");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 38." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 38." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("8/8/8/8/3Kk3/8/8/8");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 39." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 39." << std::endl;
        ERROR_COUNT++;
    }
    // Black
    testSnap.whiteKing = Coord{8, 8};
    testSnap.blackKing = Coord{4, 3};
    testSnap.board = FEN_to_matrix("8/8/8/8/3k4/4P3/8/8");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::BLACK))
    {
        std::cout << "        SUCCESS on case 40." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 40." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("8/8/8/8/3k4/2P5/8/8");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::BLACK))
    {
        std::cout << "        SUCCESS on case 41." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 41." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("8/8/8/8/3k4/8/8/3Q4");
    g.revertState(testSnap);
    if (g.isKingInCheck(Color::BLACK))
    {
        std::cout << "        SUCCESS on case 42." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 42." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("8/8/8/8/3k4/3q4/8/3Q4");
    g.revertState(testSnap);
    if (!g.isKingInCheck(Color::BLACK))
    {
        std::cout << "        SUCCESS on case 43." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 43." << std::endl;
        ERROR_COUNT++;
    }

    // Executing moves
    std::cout << "    Move applier:" << std::endl;
    testSnap.state = defaultGameState;
    testSnap.board = FEN_to_matrix("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    g.revertState(testSnap);
    g.applyMove(Coord{6, 4}, Coord{4, 4});
    // General test + en passant
    if (FEN_notation_generator(g.getSnap()) == "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3")
    {
        std::cout << "        SUCCESS on case 1." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 1." << std::endl;
        ERROR_COUNT++;
    }
    g.revertState(testSnap);
    g.makeMove(Coord{6, 4}, Coord{4, 4});
    // Testing if makeMove() has same result
    if (FEN_notation_generator(g.getSnap()) == "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3")
    {
        std::cout << "        SUCCESS on case 2." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 2." << std::endl;
        ERROR_COUNT++;
    }
    g.undo();
    // Testing if undo() works after makeMove()
    if (FEN_notation_generator(g.getSnap()) == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -")
    {
        std::cout << "        SUCCESS on case 3." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 3." << std::endl;
        ERROR_COUNT++;
    }
    // Testing for castling queen side for white
    testSnap.board = FEN_to_matrix("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R3KBNR");
    g.revertState(testSnap);
    g.applyMove(Coord{7, 4}, Coord{7, 2});
    if (FEN_notation_generator(g.getSnap()) == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/2KR1BNR b kq -")
    {
        std::cout << "        SUCCESS on case 4." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 4." << std::endl;
        ERROR_COUNT++;
    }
    // King side
    testSnap.board = FEN_to_matrix("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQK2R");
    g.revertState(testSnap);
    g.applyMove(Coord{7, 4}, Coord{7, 6});
    if (FEN_notation_generator(g.getSnap()) == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQ1RK1 b kq -")
    {
        std::cout << "        SUCCESS on case 5." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 5." << std::endl;
        ERROR_COUNT++;
    }
    // Testing for castling queen side for black
    testSnap.state.turn = Color::BLACK;
    testSnap.board = FEN_to_matrix("r3kbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    g.revertState(testSnap);
    g.applyMove(Coord{0, 4}, Coord{0, 2});
    if (FEN_notation_generator(g.getSnap()) == "2kr1bnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQ -")
    {
        std::cout << "        SUCCESS on case 6." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 6." << std::endl;
        ERROR_COUNT++;
    }
    // King side
    testSnap.board = FEN_to_matrix("rnbqk2r/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    g.revertState(testSnap);
    g.applyMove(Coord{0, 4}, Coord{0, 6});
    if (FEN_notation_generator(g.getSnap()) == "rnbq1rk1/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQ -")
    {
        std::cout << "        SUCCESS on case 7." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 7." << std::endl;
        ERROR_COUNT++;
    }

    // Color has moves
    std::cout << "    Color has moves:" << std::endl;
    testSnap.board = FEN_to_matrix("8/8/8/8/8/8/8/8");
    testSnap.state.turn = Color::WHITE;
    g.revertState(testSnap);
    if (!g.hasMoves(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 1." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 1." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.state.turn = Color::BLACK;
    g.revertState(testSnap);
    if (!g.hasMoves(Color::BLACK))
    {
        std::cout << "        SUCCESS on case 2." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 2." << std::endl;
        ERROR_COUNT++;
    }

    testSnap.board = FEN_to_matrix("4k3/8/8/8/8/8/8/4K3");
    testSnap.state.turn = Color::WHITE;
    g.revertState(testSnap);
    if (g.hasMoves(Color::WHITE))
    {
        std::cout << "        SUCCESS on case 3." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 3." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.state.turn = Color::BLACK;
    g.revertState(testSnap);
    if (g.hasMoves(Color::BLACK))
    {
        std::cout << "        SUCCESS on case 4." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 4." << std::endl;
        ERROR_COUNT++;
    }

    // Stalemate
    std::cout << "    Stalemate by material:" << std::endl;
    testSnap.board = FEN_to_matrix("4k3/8/8/8/8/8/8/4K3");
    g.revertState(testSnap);
    if (g.staleMateByMaterial())
    {
        std::cout << "        SUCCESS on case 1." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 1." << std::endl;
        ERROR_COUNT++;
    }
    testSnap.board = FEN_to_matrix("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    g.revertState(testSnap);
    if (!g.staleMateByMaterial())
    {
        std::cout << "        SUCCESS on case 2." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 2." << std::endl;
        ERROR_COUNT++;
    }

    std::cout << "Engine test routine:" << std::endl;
    Engine wai(Color::WHITE);
    Engine bai(Color::BLACK);
    std::cout << "    Engine instances initialized." << std::endl;
    testSnap.board = FEN_to_matrix("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    testSnap.state = defaultGameState;
    testSnap.blackKing = defaultBlackKing;
    testSnap.whiteKing =defaultWhiteKing;
    g.revertState(testSnap);
    std::array<Coord, 2> testEngine = wai.getBestMove(g);
    if (g.makeMove(testEngine[0], testEngine[1]))
    {
        std::cout << "        SUCCESS on case 1." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 1." << std::endl;
        ERROR_COUNT++;
    }
    testEngine = bai.getBestMove(g);
    if (g.makeMove(testEngine[0], testEngine[1]))
    {
        std::cout << "        SUCCESS on case 2." << std::endl;
    }
    else
    {
        std::cout << "        ERROR on case 2." << std::endl;
        ERROR_COUNT++;
    }


    std::cout << "================================" << std::endl;
    std::cout << "Test Summary: " << ERROR_COUNT << " error(s) out of 174 tests" << std::endl;
    if (ERROR_COUNT == 0)
        std::cout << "All systems nominal. You don't need to thank me." << std::endl;
    else
    {
        std::cout << "If multiple errors occurred on the same section, they may be related." << std::endl;
        std::cout << "The code tests cases from most basic operations (base for all others) to most complex operations." << std::endl;
    }
    std::cout << "================================" << std::endl;

    return ERROR_COUNT > 0 ? 1 : 0;
}
