#include <gtest/gtest.h>
#include "checkers.hpp"
 
//
TEST(CheckersBoardTest, ScoreAndSetup) {
    CheckersBoard board;
    EXPECT_EQ(board.getCapturedCount(true), 0);
    EXPECT_EQ(board.getCapturedCount(false), 0);
    EXPECT_TRUE(board.hasPieces(true));
}

// Обязательный ход
TEST(CheckersBoardTest, ObligatoryCaptureRules) {
    CheckersBoard board;
    board.makeMove(5, 2, 4, 3);
    board.makeMove(2, 1, 3, 2);
    EXPECT_TRUE(board.hasObligatoryCaptures());
    EXPECT_THROW(board.makeMove(5, 0, 4, 1), std::invalid_argument);
    EXPECT_NO_THROW(board.makeMove(4, 3, 2, 1));
    EXPECT_EQ(board.getCapturedCount(false), 1);
}

// Проверка траекторий ходов
TEST(CheckersBoardTest, InvalidTrajectories) {
    CheckersBoard board;
    EXPECT_THROW(board.makeMove(5, 2, 4, 2), std::invalid_argument);
}
 
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}