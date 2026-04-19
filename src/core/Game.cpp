#include <array>
#include <stack>
#include "Game.hpp"
#include "Board.hpp"
#include "Common.hpp"

Game::Game() {
    gameState.turn = Color::WHITE;
    gameState.enPassant = Coord{8, 8};
    blackKing = Coord{0, 4};
    whiteKing = Coord{7, 4};

    std::array<std::array<Piece, 8>, 8> matrix;

    // Row 0 (White back rank)
    matrix[7][0] = Piece{PieceType::ROOK, Color::WHITE, '1'};
    matrix[7][1] = Piece{PieceType::KNIGHT, Color::WHITE, '1'};
    matrix[7][2] = Piece{PieceType::BISHOP, Color::WHITE, '1'};
    matrix[7][3] = Piece{PieceType::QUEEN, Color::WHITE, '0'};
    matrix[7][4] = Piece{PieceType::KING, Color::WHITE, '0'};
    matrix[7][5] = Piece{PieceType::BISHOP, Color::WHITE, '2'};
    matrix[7][6] = Piece{PieceType::KNIGHT, Color::WHITE, '2'};
    matrix[7][7] = Piece{PieceType::ROOK, Color::WHITE, '2'};

    // Row 1 (White pawns)
    for (int col = 0; col < 8; col++) {
        matrix[6][col] = Piece{PieceType::PAWN, Color::WHITE, static_cast<char>('1' + col)};
    }

    // Row 7 (Black back rank)
    matrix[0][0] = Piece{PieceType::ROOK, Color::BLACK, '1'};
    matrix[0][1] = Piece{PieceType::KNIGHT, Color::BLACK, '1'};
    matrix[0][2] = Piece{PieceType::BISHOP, Color::BLACK, '1'};
    matrix[0][3] = Piece{PieceType::QUEEN, Color::BLACK, '0'};
    matrix[0][4] = Piece{PieceType::KING, Color::BLACK, '0'};
    matrix[0][5] = Piece{PieceType::BISHOP, Color::BLACK, '2'};
    matrix[0][6] = Piece{PieceType::KNIGHT, Color::BLACK, '2'};
    matrix[0][7] = Piece{PieceType::ROOK, Color::BLACK, '2'};

    // Row 6 (Black pawns)
    for (int col = 0; col < 8; col++) {
        matrix[1][col] = Piece{PieceType::PAWN, Color::BLACK, static_cast<char>('1' + col)};
    }

    // Rows 5-2 (Empty - defaults are already BLANK)

    board.setMatrix(matrix);
}

Board Game::getBoard() {
    return board;
}

Color Game::getTurn() {
    return gameState.turn;
}

void Game::changeTurn() {
    gameState.turn = gameState.turn == Color::BLACK ? Color::WHITE : Color::BLACK;
}

std::array<Coord, 27> Game::pawnMoves(Coord p) {
    std::array<Coord, 27> arr;

    // Setting sentinel values - Remember to check
    for (int i = 0; i < 27; i++)
        arr[i] = Coord{8, 8};

    Piece pawn = board.getPiece(p);
    if (pawn.t != PieceType::PAWN)
        return arr;

    int moves[4][2] = {{1, 0}, {2, 0}, {1, 1}, {1, -1}};;
    if (pawn.c == Color::WHITE) {
        for (int i = 0; i < 4; i++) {
            moves[i][0] *= -1;
            moves[i][1] *= -1;
        }
    }

    int k = 0;
    for (int i = 0; i < 4; i++) {
        Coord currentMove = Coord{p.row + moves[i][0], p.col + moves[i][1]};
        if (isValidCoord(currentMove)) {
            // Diagonals
            if (moves[i][1] == 1 || moves[i][1] == -1) {
                if ((board.getPiece(currentMove).c != pawn.c &&
                     board.getPiece(currentMove).t != PieceType::BLANK)) {
                    arr[k] = currentMove;
                    k++;
                }
                // En passant check, making sure it is only allowed when the space is blank and when it is the piece's turn (to avoid misscalculations in finding the control areas)
                if ((gameState.enPassant == currentMove &&
                     board.getPiece(currentMove).t == PieceType::BLANK &&
                     gameState.turn == pawn.c)) {
                    arr[k] = currentMove;
                    k++;
                }
            }
            // Move 1 to the front
            else if (moves[i][0] == 1 || moves[i][0] == -1) {
                if (board.getPiece(currentMove).t == PieceType::BLANK) {
                    arr[k] = currentMove;
                    k++;
                }
            }
            // Move 2 to the front
            else if (moves[i][0] == 2 || moves[i][0] == -2) {
                if (pawn.c == Color::BLACK &&
                    board.getPiece(currentMove).t == PieceType::BLANK &&
                    board.getPiece(Coord{p.row + 1, p.col}).t == PieceType::BLANK &&
                    p.row == 1) {
                    arr[k] = currentMove;
                    k++;
                }
                if (pawn.c == Color::WHITE &&
                    board.getPiece(currentMove).t == PieceType::BLANK &&
                    board.getPiece(Coord{p.row - 1, p.col}).t == PieceType::BLANK &&
                    p.row == 6) {
                    arr[k] = currentMove;
                    k++;
                }
            }
        }
    }

    return arr;
}

std::array<Coord, 27> Game::knightMoves(Coord p) {
    std::array<Coord, 27> arr;

    // Setting sentinel values - Remember to check
    for (int i = 0; i < 27; i++)
        arr[i] = Coord{8, 8};

    Piece knight = board.getPiece(p);
    if (knight.t != PieceType::KNIGHT)
        return arr;

    // "L" moves for all directions
    int moves[8][2] = {{-2, 1}, {2, 1}, {-2, -1}, {2, -1}, {1, -2}, {1, 2}, {-1, -2}, {-1, 2}};

    int k = 0;
    for (int i = 0; i < 8; i++) {
        Coord currentMove = Coord{p.row + moves[i][0], p.col + moves[i][1]};
        if (isValidCoord(currentMove)) {
            if (board.getPiece(currentMove).t == PieceType::BLANK || board.getPiece(currentMove).c != knight.c) {
                arr[k] = currentMove;
                k++;
            }
        }
    }

    return arr;
}

std::array<Coord, 27> Game::rookMoves(Coord p) {
    std::array<Coord, 27> arr;

    // Setting sentinel values - Remember to check
    for (int i = 0; i < 27; i++)
        arr[i] = Coord{8, 8};

    Piece rook = board.getPiece(p);
    if (rook.t != PieceType::ROOK && rook.t != PieceType::QUEEN)
        return arr;

    int moves[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    int k = 0;
    for (int i = 0; i < 4; i++) {
        // Slide orthogonally in each direction until hitting a piece or board edge
        for (int j = 1; j < 8; j++) {
            Coord currentMove = Coord{p.row + (j * moves[i][0]), p.col + (j * moves[i][1])};
            if (isValidCoord(currentMove)) {
                if (board.getPiece(currentMove).t == PieceType::BLANK) {
                    arr[k] = currentMove;
                    k++;
                } else if (board.getPiece(currentMove).c != rook.c) {
                    arr[k] = currentMove;
                    k++;
                } else
                    break;
            } else
                break;
        }
    }

    return arr;
}

std::array<Coord, 27> Game::bishopMoves(Coord p) {
    std::array<Coord, 27> arr;

    // Setting sentinel values - Remember to check
    for (int i = 0; i < 27; i++)
        arr[i] = Coord{8, 8};

    Piece bishop = board.getPiece(p);
    if (bishop.t != PieceType::BISHOP && bishop.t != PieceType::QUEEN)
        return arr;

    int moves[4][2] = {{1, 1}, {-1, 1}, {1, -1}, {-1, -1}};
    int k = 0;
    for (int i = 0; i < 4; i++) {
        // Slide diagonally in each direction until hitting a piece or board edge
        for (int j = 1; j < 8; j++) {
            Coord currentMove = Coord{p.row + (j * moves[i][0]), p.col + (j * moves[i][1])};
            if (isValidCoord(currentMove)) {
                if (board.getPiece(currentMove).t == PieceType::BLANK) {
                    arr[k] = currentMove;
                    k++;
                } else if (board.getPiece(currentMove).c != bishop.c) {
                    arr[k] = currentMove;
                    k++;
                } else
                    break;
            } else
                break;
        }
    }

    return arr;
}

std::array<Coord, 27> Game::queenMoves(Coord p) {
    std::array<Coord, 27> arr;

    // Setting sentinel values - Remember to check
    for (int i = 0; i < 27; i++)
        arr[i] = Coord{8, 8};

    Piece queen = board.getPiece(p);
    if (queen.t != PieceType::QUEEN)
        return arr;

    // Perpendicular moves
    arr = rookMoves(p);
    int k = 0;
    for (int i = 0; i < 27; i++) {
        if (arr[i] == Coord{8, 8}) {
            k = i;
            break;
        }
    }

    // Diagonal moves
    std::array<Coord, 27> bishop = bishopMoves(p);
    for (int i = k; i < 27; i++) {
        arr[i] = bishop[i - k];
    }

    return arr;
}

std::array<Coord, 27> Game::kingMoves(Coord p) {
    std::array<Coord, 27> arr;

    // Setting sentinel values - Remember to check
    for (int i = 0; i < 27; i++)
        arr[i] = Coord{8, 8};

    Piece king = board.getPiece(p);
    if (king.t != PieceType::KING)
        return arr;

    // Areas around the king's square
    int moves[8][2] = {{1, 0}, {1, 1}, {1, -1}, {-1, 0}, {-1, 1}, {-1, -1}, {0, 1}, {0, -1}};

    int k = 0;
    for (int i = 0; i < 8; i++) {
        Coord currentMove = Coord{p.row + moves[i][0], p.col + moves[i][1]};
        if (isValidCoord(currentMove)) {
            if (board.getPiece(currentMove).t == PieceType::BLANK || board.getPiece(currentMove).c != king.c) {
                arr[k] = currentMove;
                k++;
            }
        }
    }

    int colorIndex = king.c == Color::BLACK ? 1 : 0;
    // Castling queen side
    if (gameState.castling[0][colorIndex]) {
        Coord currentMove = Coord{p.row, p.col - 2};
        Coord id1RookToKingSpaces[3] = {Coord{p.row, p.col - 1}, Coord{p.row, p.col - 2}, Coord{p.row, p.col - 3}};
        if (isValidCoord(id1RookToKingSpaces[0]) && isValidCoord(id1RookToKingSpaces[1])) {
            bool allBlankAndNotInControl = true;
            for (int i = 0; i < 3; i++) {
                if (board.getPiece(id1RookToKingSpaces[i]).t != PieceType::BLANK ||
                    isControlledBy(id1RookToKingSpaces[i], king.c == Color::BLACK ? Color::WHITE : Color::BLACK)) {
                    allBlankAndNotInControl = false;
                    break;
                }
            }
            if (allBlankAndNotInControl) {
                arr[k] = currentMove;
                k++;
            }
        }
    }
    // Castling king side
    if (gameState.castling[1][colorIndex]) {
        Coord currentMove = Coord{p.row, p.col + 2};
        Coord id1RookToKingSpaces[2] = {Coord{p.row, p.col + 1}, Coord{p.row, p.col + 2}};
        if (isValidCoord(id1RookToKingSpaces[0]) && isValidCoord(id1RookToKingSpaces[1])) {
            bool allBlankAndNotInControl = true;
            for (int i = 0; i < 2; i++) {
                if (board.getPiece(id1RookToKingSpaces[i]).t != PieceType::BLANK ||
                    isControlledBy(id1RookToKingSpaces[i], king.c == Color::BLACK ? Color::WHITE : Color::BLACK)) {
                    allBlankAndNotInControl = false;
                    break;
                }
            }
            if (allBlankAndNotInControl) {
                arr[k] = currentMove;
                k++;
            }
        }
    }

    return arr;
}

bool Game::isControlledBy(Coord sq, Color attacker) {
    // Check for Knights
    int knightMoves[8][2] = {{-2, 1}, {-2, -1}, {2, 1}, {2, -1}, {1, -2}, {1, 2}, {-1, -2}, {-1, 2}};
    for (auto &m: knightMoves) {
        Coord target = {sq.row + m[0], sq.col + m[1]};
        if (isValidCoord(target)) {
            Piece p = board.getPiece(target);
            if (p.t == PieceType::KNIGHT && p.c == attacker)
                return true;
        }
    }

    // Check for Sliders (Rooks, Bishops, Queens)
    int directions[8][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    for (int i = 0; i < 8; i++) {
        for (int d = 1; d < 8; d++) {
            Coord target = {sq.row + (directions[i][0] * d), sq.col + (directions[i][1] * d)};
            if (!isValidCoord(target))
                break;

            Piece p = board.getPiece(target);
            if (p.t != PieceType::BLANK) {
                if (p.c == attacker) {
                    // Check if piece type matches the direction (Orthogonal vs Diagonal)
                    if (i < 4) {
                        // Orthogonal (Rook/Queen)
                        if (p.t == PieceType::ROOK || p.t == PieceType::QUEEN)
                            return true;
                    } else {
                        // Diagonal (Bishop/Queen)
                        if (p.t == PieceType::BISHOP || p.t == PieceType::QUEEN)
                            return true;
                    }
                }
                break;
            }
        }
    }

    // Check for Pawns
    // If we are looking for a WHITE attacker, the pawn must be BELOW the square (row - 1)
    // If we are looking for a BLACK attacker, the pawn must be ABOVE the square (row + 1)
    int pawnRowDiff = (attacker == Color::WHITE) ? -1 : 1;
    int pawnCols[2] = {sq.col - 1, sq.col + 1};

    for (int c: pawnCols) {
        Coord target = {sq.row + pawnRowDiff, c};
        if (isValidCoord(target)) {
            Piece p = board.getPiece(target);
            if (p.t == PieceType::PAWN && p.c == attacker)
                return true;
        }
    }

    // 4. Check for King (to prevent Kings from standing next to each other)
    int kingMoves[8][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    for (auto &m: kingMoves) {
        Coord target = {sq.row + m[0], sq.col + m[1]};
        if (isValidCoord(target)) {
            Piece p = board.getPiece(target);
            if (p.t == PieceType::KING && p.c == attacker)
                return true;
        }
    }

    return false;
}

bool Game::move(Coord from, Coord to) {
    Piece movingPiece = board.getPiece(from);

    // Making sure all arguments are valid
    if (movingPiece.t == PieceType::BLANK || movingPiece.t == PieceType::ERROR || !isValidCoord(to))
        return false;

    if (movingPiece.t == PieceType::PAWN) {
        // Updating last en passant coordinates
        if (to == Coord{from.row + 2, from.col} || to == Coord{from.row - 2, from.col}) {
            if (movingPiece.c == Color::BLACK)
                gameState.enPassant = Coord{from.row + 1, from.col};
            else
                gameState.enPassant = Coord{from.row - 1, from.col};
        } else {
            if (to == gameState.enPassant) {
                Coord removePawn = Coord{from.row, gameState.enPassant.col};
                board.setPiece(removePawn, Piece{PieceType::BLANK, Color::BLACK, 'b'});
            }
            // Clearing last en passant coordinates, since it now has been 2 moves since the pawn moved
            gameState.enPassant = Coord{8, 8};
        }
        if ((movingPiece.c == Color::WHITE && to.row == 0) ||
            (movingPiece.c == Color::BLACK && to.row == 7)) {
            Piece promoted = movingPiece;
            promoted.t = PieceType::QUEEN;
            board.setPiece(to, promoted);
        }
    } else {
        // Clearing last en passant coordinates, since it now has been 2 moves since the pawn moved
        gameState.enPassant = Coord{8, 8};

        // Updating castling logic and moving the rook
        if (movingPiece.t == PieceType::ROOK) {
            // Removing castling possibility after rook move
            int colorIndex = movingPiece.c == Color::BLACK ? 1 : 0;
            gameState.castling[movingPiece.id - '1'][colorIndex] = false;
        }
        if (movingPiece.t == PieceType::KING) {
            // Setting king side rook to the right of the king
            if (to == Coord{from.row, from.col + 2}) {
                Piece rookKingSide = Piece{PieceType::ROOK, movingPiece.c, '2'};
                board.setPiece(Coord{from.row, from.col + 1}, rookKingSide);

                // Clearing the old rook
                board.setPiece(Coord{from.row, from.col + 3}, Piece{PieceType::BLANK, Color::BLACK, 'b'});
            }
            // Setting queen side rook to the left of the king
            else if (to == Coord{from.row, from.col - 2}) {
                Piece rookKingSide = Piece{PieceType::ROOK, movingPiece.c, '1'};
                board.setPiece(Coord{from.row, from.col - 1}, rookKingSide);

                // Clearing the old rook
                board.setPiece(Coord{from.row, from.col - 4}, Piece{PieceType::BLANK, Color::BLACK, 'b'});
            }

            int colorIndex = movingPiece.c == Color::BLACK ? 1 : 0;
            if (gameState.castling[0][colorIndex] || gameState.castling[1][colorIndex]) {
                gameState.castling[0][colorIndex] = false;
                gameState.castling[1][colorIndex] = false;
            }
        }
    }

    board.setPiece(to, movingPiece);
    board.setPiece(from, Piece{PieceType::BLANK, Color::BLACK, 'b'});

    // Fixing king tracking
    if (movingPiece.t == PieceType::KING) {
        if (movingPiece.c == Color::BLACK)
            blackKing = to;
        else
            whiteKing = to;
    }

    return true;
}

void Game::setHistory() {
    SnapShot snap;
    snap.board = board.snapshot();
    snap.state = gameState;
    history.push(snap);
}

void Game::revertState(SnapShot &snap) {
    gameState = snap.state;
    board.setMatrix(snap.board);
}

bool Game::isKingInCheck(Color c) {
    if (c == Color::BLACK)
        return isControlledBy(blackKing, Color::WHITE);
    else
        return isControlledBy(whiteKing, Color::BLACK);
}

std::array<Coord, 27> Game::possibleMoves(Coord piece) {
    std::array<Coord, 27> arr;

    // Setting sentinel values - Remember to check
    for (int i = 0; i < 27; i++)
        arr[i] = Coord{8, 8};

    Piece p = board.getPiece(piece);

    if (p.c != gameState.turn)
        return arr;

    // Finding possible moves without checking for validity
    switch (p.t) {
        case PieceType::PAWN:
            arr = pawnMoves(piece);
            break;

        case PieceType::ROOK:
            arr = rookMoves(piece);
            break;

        case PieceType::KNIGHT:
            arr = knightMoves(piece);
            break;

        case PieceType::BISHOP:
            arr = bishopMoves(piece);
            break;

        case PieceType::QUEEN:
            arr = queenMoves(piece);
            break;

        case PieceType::KING:
            arr = kingMoves(piece);
            break;

        default:
            break;
    }

    std::array<Coord, 27> possible;
    // Setting sentinel values - Remember to check
    for (int i = 0; i < 27; i++)
        possible[i] = Coord{8, 8};

    int k = 0;
    // Making sure the obtained moves don't result in check on self king
    for (int i = 0; i < 27; i++) {
        if (arr[i] == Coord{8, 8})
            break;

        if (!testForCheck(piece, arr[i])) {
            possible[k] = arr[i];
            k++;
        }
    }

    return possible;
}

bool Game::hasMoves(Color c) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board.getPiece(Coord{i, j}).c == c && board.getPiece(Coord{i, j}).t != PieceType::BLANK) {
                if (possibleMoves(Coord{i, j})[0] != Coord{8, 8})
                    return true;
            }
        }
    }

    return false;
}

bool Game::testForCheck(Coord from, Coord to) {
    // Saving current Board and Game State
    SnapShot snap;
    snap.board = board.snapshot();
    snap.state = gameState;

    Piece movingPiece = board.getPiece(from);

    // Making the move
    move(from, to);

    // Finding if in check
    bool check = isKingInCheck(movingPiece.c);

    // Undoing the move
    board.setMatrix(snap.board);
    gameState = snap.state;

    return check;
}

bool Game::makeMove(Coord from, Coord to) {
    setHistory();
    return move(from, to);
}
