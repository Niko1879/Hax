#include "board.h"

namespace Hax
{
	bool operator==(const Board& l, const Board& r)
	{
		return l.board == r.board;
	}

	Board::Board(int length) : length(length), area(length * length), numOccupied(0), whiteToMove(true)
	{
		if (length <= 0 || length > MAX_BOARD_SIZE) throw std::invalid_argument("Board length must be positive");
		
		board.resize((size_t)area);
		for (int i = 0; i < area; ++i)
		{
			board[i] = Hexagon::Unoccupied;
		}
	}

	const Hexagon& Board::operator[](int index) const
	{
		return board[index];
	}

	int Board::Length() const
	{
		return length;
	}

	int Board::Area() const
	{
		return area;
	}

	int Board::CountOccupied() const
	{
		return numOccupied;
	}

	int Board::CountUnoccupied() const
	{
		return area - numOccupied;
	}

	bool Board::WhiteToMove() const
	{
		return whiteToMove;
	}

	bool Board::IsLegalMove(int move) const
	{
		return board[move] == Hexagon::Unoccupied;
	}

	void Board::MakeMove(int move)
	{
		D(if (!IsLegalMove(move)) throw std::logic_error("Illegal move"));
		board[move] = (whiteToMove) ? Hexagon::White : Hexagon::Black;
		whiteToMove = !whiteToMove;
		++numOccupied;
	}

	void Board::UndoMove(int move)
	{
		D(if (IsLegalMove(move)) throw std::logic_error("No move here to undo"));
		board[move] = Hexagon::Unoccupied;
		whiteToMove = !whiteToMove;
		--numOccupied;
	}
}