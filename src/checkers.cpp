#include "checkers.hpp"
#include <iostream>
#include <stdexcept>
#include <cmath>

CheckersBoard::CheckersBoard() : whiteTurn(true), isMultiCaptureMode(false), activePieceCurrentPos({-1, -1}) {
    board.resize(8, std::vector<Piece>(8, Piece::Empty));
    
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 8; ++c) {
            if ((r + c) % 2 != 0) {
                board[r][c] = Piece::Black;
            }
        }
    }
    
    for (int r = 5; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            if ((r + c) % 2 != 0) {
                board[r][c] = Piece::White;
            }
        }
    }
}

Piece CheckersBoard::getPiece(int row, int col) const {
    if (row < 0 || row >= 8 || col < 0 || col >= 8) {
        throw std::out_of_range("Координаты вышли за пределы доски (0-7).");
    }
    return board[row][col];
}

bool CheckersBoard::isWhiteTurn() const {
    return whiteTurn;
}

void CheckersBoard::displayBoard() const {
    std::cout << "\n    0  1  2  3  4  5  6  7\n";
    std::cout << "  --------------------------\n";
    for (int r = 0; r < 8; ++r) {
        std::cout << r << " |";
        for (int c = 0; c < 8; ++c) {
            switch (board[r][c]) {
                case Piece::Empty:     std::cout << " . "; break;
                case Piece::White:     std::cout << " w "; break;
                case Piece::Black:     std::cout << " b "; break;
                case Piece::WhiteKing: std::cout << " W "; break;
                case Piece::BlackKing: std::cout << " B "; break;
            }
        }
        std::cout << "|\n";
    }
    std::cout << "  --------------------------\n";
    std::cout << "Съедено БЕЛЫХ: " << getCapturedCount(true) 
              << " | Съедено ЧЕРНЫХ: " << getCapturedCount(false) << "\n";
}

int CheckersBoard::getCapturedCount(bool ofWhite) const {
    int currentOnBoard = 0;
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Piece p = board[r][c];
            if (ofWhite && (p == Piece::White || p == Piece::WhiteKing)) currentOnBoard++;
            if (!ofWhite && (p == Piece::Black || p == Piece::BlackKing)) currentOnBoard++;
        }
    }
    return 12 - currentOnBoard;
}

bool CheckersBoard::canPieceCapture(int row, int col) const {
    Piece p = board[row][col];
    if (p == Piece::Empty) return false;

    bool isPieceWhite = (p == Piece::White || p == Piece::WhiteKing);
    
    int dRow[] = {-1, -1, 1, 1};
    int dCol[] = {-1, 1, -1, 1};

    if (p == Piece::White || p == Piece::Black) {
        for (int i = 0; i < 4; ++i) {
            int midR = row + dRow[i];
            int midC = col + dCol[i];
            int toR = row + dRow[i] * 2;
            int toC = col + dCol[i] * 2;

            if (toR >= 0 && toR < 8 && toC >= 0 && toC < 8) {
                Piece midPiece = board[midR][midC];
                Piece toPiece = board[toR][toC];

                if (midPiece != Piece::Empty && toPiece == Piece::Empty) {
                    bool isMidWhite = (midPiece == Piece::White || midPiece == Piece::WhiteKing);
                    if (isPieceWhite != isMidWhite) {
                        return true; 
                    }
                }
            }
        }
    }
    else if (p == Piece::WhiteKing || p == Piece::BlackKing) {
        for (int i = 0; i < 4; ++i) {
            int r = row + dRow[i];
            int c = col + dCol[i];
            bool foundEnemy = false;

            while (r >= 0 && r < 8 && c >= 0 && c < 8) {
                if (board[r][c] == Piece::Empty) {
                    if (foundEnemy) return true;
                } else {
                    bool isEnemyWhite = (board[r][c] == Piece::White || board[r][c] == Piece::WhiteKing);
                    if (isPieceWhite == isEnemyWhite || foundEnemy) {
                        break;
                    }
                    foundEnemy = true;
                }
                r += dRow[i];
                c += dCol[i];
            }
        }
    }
    return false;
}

bool CheckersBoard::hasObligatoryCaptures() const {
    if (isMultiCaptureMode) {
        return true;
    }

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Piece p = board[r][c];
            if (p != Piece::Empty) {
                bool isPieceWhite = (p == Piece::White || p == Piece::WhiteKing);
                if (isPieceWhite == whiteTurn && canPieceCapture(r, c)) {
                    return true;
                }
            }
        }
    }
    return false;
}

void CheckersBoard::makeMove(int fromRow, int fromCol, int toRow, int toCol) {
    Piece p = getPiece(fromRow, fromCol);
    getPiece(toRow, toCol);
    if (p == Piece::Empty) throw std::invalid_argument("В начальной клетке нет фигуры.");
    
    bool isPieceWhite = (p == Piece::White || p == Piece::WhiteKing);
    if (isPieceWhite != whiteTurn) throw std::invalid_argument("Сейчас ход другого игрока.");
    if (board[toRow][toCol] != Piece::Empty) throw std::invalid_argument("Целевая клетка занята.");
    if (isMultiCaptureMode) {
        if (fromRow != activePieceCurrentPos.first || fromCol != activePieceCurrentPos.second) {
            throw std::invalid_argument("Вы обязаны бить той же шашкой.");
        }
    }

    int rowDiff = toRow - fromRow;
    int colDiff = toCol - fromCol;

    if (std::abs(rowDiff) != std::abs(colDiff)) {
        throw std::invalid_argument("Ход должен быть по диагонали.");
    }

    bool isCapture = false;
    int enemyRow = -1;
    int enemyCol = -1;

    if (p == Piece::White || p == Piece::Black) {
        if (std::abs(rowDiff) == 1) {
            if (hasObligatoryCaptures()) throw std::invalid_argument("Вы обязаны рубить фигуру противника.");
            if (p == Piece::White && rowDiff > 0) throw std::invalid_argument("Белая шашка не умеет ходить назад.");
            if (p == Piece::Black && rowDiff < 0) throw std::invalid_argument("Черная шашка не умеет ходить назад.");
        } 
        else if (std::abs(rowDiff) == 2) {
            enemyRow = fromRow + rowDiff / 2;
            enemyCol = fromCol + colDiff / 2;
            if (board[enemyRow][enemyCol] == Piece::Empty) throw std::invalid_argument("Некого рубить.");
            
            bool isEnemyWhite = (board[enemyRow][enemyCol] == Piece::White || board[enemyRow][enemyCol] == Piece::WhiteKing);
            if (isEnemyWhite == whiteTurn) throw std::invalid_argument("Нельзя рубить свои фигуры.");
            isCapture = true;
        } 
        else {
            throw std::invalid_argument("Шашка ходит только на 1-2 клетки.");
        }
    }

    else {
        int stepR = (rowDiff > 0) ? 1 : -1;
        int stepC = (colDiff > 0) ? 1 : -1;
        int r = fromRow + stepR;
        int c = fromCol + stepC;
        
        int enemyCount = 0;

        while (r != toRow && c != toCol) {
            if (board[r][c] != Piece::Empty) {
                bool isEnemyWhite = (board[r][c] == Piece::White || board[r][c] == Piece::WhiteKing);
                if (isEnemyWhite == whiteTurn) throw std::invalid_argument("Дамка не может перелетать через свои фигуры.");
                
                enemyCount++;
                enemyRow = r;
                enemyCol = c;
            }
            r += stepR;
            c += stepC;
        }

        if (enemyCount > 1) throw std::invalid_argument("Дамка не может рубить более одной фигуры за один прыжок.");
        if (enemyCount == 0 && hasObligatoryCaptures()) throw std::invalid_argument("Вы обязаны рубить фигуру противника!");
        
        if (enemyCount == 1) isCapture = true;
    }

    board[toRow][toCol] = p;
    board[fromRow][fromCol] = Piece::Empty;

    if (isCapture) {
        board[enemyRow][enemyCol] = Piece::Empty;

        if (board[toRow][toCol] == Piece::White && toRow == 0) board[toRow][toCol] = Piece::WhiteKing;
        if (board[toRow][toCol] == Piece::Black && toRow == 7) board[toRow][toCol] = Piece::BlackKing;

        if (canPieceCapture(toRow, toCol)) {
            isMultiCaptureMode = true;
            activePieceCurrentPos = {toRow, toCol};
            return; 
        }
    }

    isMultiCaptureMode = false;
    activePieceCurrentPos = {-1, -1};
    whiteTurn = !whiteTurn;
}

bool CheckersBoard::hasPieces(bool checkWhite) const {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Piece p = board[r][c];
            if (checkWhite && (p == Piece::White || p == Piece::WhiteKing)) return true;
            if (!checkWhite && (p == Piece::Black || p == Piece::BlackKing)) return true;
        }
    }
    return false;
}