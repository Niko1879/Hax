#include "pch.h"
#include "pathfinding.h"

TEST(TestPathfinding, TestCheckWinState)
{
	Hax::Board board(10);
	EXPECT_EQ(Hax::Pathfinding::CheckWinState(board), Hax::WinState::Ongoing);

	//simple example
	for (int i = 0; i < 10; ++i)
	{
		board.MakeMove(99 - i);
		board.MakeMove(i);
	}

	EXPECT_EQ(Hax::Pathfinding::CheckWinState(board), Hax::WinState::Black);

	//trickier one
	int whitemoves[] = {
		0, 1, 2, 3, 4, 13, 16, 17, 22, 25, 27, 32, 33, 34, 36, 37, 45,
		51, 52, 53, 54, 56, 57, 58, 61, 65, 68, 71, 72, 73, 74, 76, 77, 78,
		85, 95
	};

	int blackmoves[] = {
		5, 6, 7, 8, 9, 10, 11, 12, 14, 15, 18, 19, 20, 21, 23, 24, 26, 
		28, 29, 30, 31, 35, 38, 39, 40, 41, 42, 43, 44, 46, 47, 48, 49,
		97, 98, 99
	};

	board = Hax::Board(10);
	for (int i = 0; i < 36; ++i)
	{
		board.MakeMove(whitemoves[i]);
		board.MakeMove(blackmoves[i]);
	}

	board.MakeMove(96);

	EXPECT_EQ(Hax::Pathfinding::CheckWinState(board), Hax::WinState::White);
}


TEST(TestPathfinding, TestCheckWinStateVirtual)
{
	//easy one
	Hax::Board board(10);
	int whitemoves[] = {
		4, 23, 42, 61, 72
	};

	int blackmoves[] = {
		9, 19, 29, 39, 49
	};

	for (int i = 0; i < 5; ++i)
	{
		board.MakeMove(whitemoves[i]);
		board.MakeMove(blackmoves[i]);
	}

	board.MakeMove(91); //finish white's path
	EXPECT_EQ(Hax::Pathfinding::CheckWinState(board, true), Hax::WinState::White);
	//block the path
	board.MakeMove(13);
	board.MakeMove(16);
	EXPECT_EQ(Hax::Pathfinding::CheckWinState(board, true), Hax::WinState::Ongoing);
}


TEST(TestPathfinding, TestCheckWinStateVirtualDisconnected)
{
	Hax::Board board(10);
	int whitemoves[] = {
		14, 33, 52, 71
	};

	int blackmoves[] = {
		9, 19, 29, 39
	};

	//one where the edges aren't directly connected
	board = Hax::Board(10);
	for (int i = 0; i < 4; ++i)
	{
		board.MakeMove(whitemoves[i]);
		board.MakeMove(blackmoves[i]);
	}
	board.MakeMove(82);
	EXPECT_EQ(Hax::Pathfinding::CheckWinState(board, true), Hax::WinState::White);

	//block the path
	board.MakeMove(24);
	board.MakeMove(16);
	EXPECT_EQ(Hax::Pathfinding::CheckWinState(board, true), Hax::WinState::Ongoing);
}


TEST(TestPathfinding, TestCheckWinStateGeneral1)
{
	Hax::Board board(10);
	int whitemoves[] = {
		13, 18, 26, 32, 34, 36, 43, 47, 48, 54, 58, 74, 76, 78
	};

	int blackmoves[] = {
		7, 15, 24, 25, 33, 35, 42, 45, 51, 56, 57, 67, 68, 77
	};

	for (int i = 0; i < 14; ++i)
	{
		board.MakeMove(whitemoves[i]);
		board.MakeMove(blackmoves[i]);
	}

	EXPECT_EQ(Hax::Pathfinding::CheckWinState(board, true), Hax::WinState::Black);
	EXPECT_EQ(Hax::Pathfinding::CheckWinState(board), Hax::WinState::Ongoing);
}


TEST(TestPathfinding, TestCheckWinStateGeneral2)
{
	Hax::Board board(10);
	int whitemoves[] = {
		10, 19, 29, 42, 44, 45, 46, 52, 55, 57, 71, 74, 76, 82
	};

	int blackmoves[] = {
		9, 18, 34, 35, 36, 37, 43, 53, 54, 65, 72, 81, 83, 84
	};

	for (int i = 0; i < 14; ++i)
	{
		board.MakeMove(whitemoves[i]);
		board.MakeMove(blackmoves[i]);
	}

	EXPECT_EQ(Hax::Pathfinding::CheckWinState(board, true), Hax::WinState::Black);
	EXPECT_EQ(Hax::Pathfinding::CheckWinState(board), Hax::WinState::Ongoing);

	//block the path
	board.MakeMove(28);
	board.MakeMove(99);
	EXPECT_EQ(Hax::Pathfinding::CheckWinState(board, true), Hax::WinState::Ongoing);

	//unblock the path
	board.MakeMove(98);
	board.MakeMove(27);
	EXPECT_EQ(Hax::Pathfinding::CheckWinState(board, true), Hax::WinState::Black);
}