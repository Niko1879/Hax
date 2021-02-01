#include "pch.h"
#include "board.h"


TEST(TestBoard, TestConstruct)
{
	EXPECT_NO_THROW(Hax::Board board(10));
	EXPECT_NO_THROW(Hax::Board board(1));
	EXPECT_NO_THROW(Hax::Board board(20));
	EXPECT_THROW(Hax::Board board(0), std::invalid_argument);
	EXPECT_THROW(Hax::Board(Hax::MAX_BOARD_SIZE + 1), std::invalid_argument);
}


TEST(TestBoard, TestAccessOperator)
{
	Hax::Board board(10);
	EXPECT_NO_THROW(board[2]);
	EXPECT_EQ(board[2], Hax::Hexagon::Unoccupied);
}


TEST(TestBoard, TestLength)
{
	Hax::Board board(10);
	EXPECT_EQ(board.Length(), 10);

	Hax::Board board2(1);
	EXPECT_EQ(board2.Length(), 1);
}


TEST(TestBoard, TestArea)
{
	Hax::Board board(10);
	EXPECT_EQ(board.Area(), 100);

	Hax::Board board2(11);
	EXPECT_EQ(board2.Area(), 121);
}


TEST(TestBoard, TestCountOccupied)
{
	Hax::Board board(10);
	board.MakeMove(1);
	EXPECT_EQ(board.CountOccupied(), 1);
	board.MakeMove(2);
	board.MakeMove(3);
	EXPECT_EQ(board.CountOccupied(), 3);
}


TEST(TestBoard, TestCountUnoccupied)
{
	Hax::Board board(10);
	board.MakeMove(1);
	EXPECT_EQ(board.CountUnoccupied(), 99);
	board.MakeMove(2);
	board.MakeMove(3);
	EXPECT_EQ(board.CountUnoccupied(), 97);
}


TEST(TestBoard, TestWhiteToMove)
{
	Hax::Board board(10);
	EXPECT_TRUE(board.WhiteToMove());
	board.MakeMove(1);
	EXPECT_FALSE(board.WhiteToMove());
}


TEST(TestBoard, TestMakeMove)
{
	Hax::Board board(10);
	board.MakeMove(0);
	EXPECT_EQ(board[0], Hax::Hexagon::White);

	board.MakeMove(1);
	EXPECT_EQ(board[1], Hax::Hexagon::Black);
}


TEST(TestBoard, TestUndoMove)
{
	Hax::Board board(10);
	board.MakeMove(0);
	board.MakeMove(1);
	board.UndoMove(0);
	EXPECT_EQ(board[0], Hax::Hexagon::Unoccupied);
	board.UndoMove(1);
	EXPECT_EQ(board[1], Hax::Hexagon::Unoccupied);
}


TEST(TestBoard, TestIsLegalMove)
{
	Hax::Board board(10);
	EXPECT_TRUE(board.IsLegalMove(0));
	board.MakeMove(0);
	EXPECT_FALSE(board.IsLegalMove(0));
}