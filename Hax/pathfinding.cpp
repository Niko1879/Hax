#include "pathfinding.h"

namespace Hax
{
	namespace Pathfinding
	{
		int _Row(int pos, const Board& board)
		{
			return pos / board.Length();
		}

		int _Column(int pos, const Board& board)
		{
			return pos % board.Length();
		}
		
		
		bool _IsWithinBounds(int xInc, int yInc, int pos, const Board& board)
		{
			int col = _Column(pos, board);
			int row = _Row(pos, board);
			return 	0 <= col + xInc && 
				    col + xInc < board.Length() &&
				    0 <= row + yInc &&
				    row + yInc < board.Length();
		}


		int _Traverse(int xInc, int yInc, int pos, const Board& board)
		{
			return pos + xInc + yInc * board.Length();
		}


		bool _DirectPathGoalState(int pos, const Board& board, bool white)
		{
			if (white) return _Row(pos, board) == board.Length() - 1;
			return _Column(pos, board) == board.Length() - 1;
		}


		bool _VirtualPathGoalState(int pos, const Board& board, bool white)
		{
			if (_DirectPathGoalState(pos, board, white)) 
				return true;

			if (white)
			{
				return
					_Row(pos, board) == board.Length() - 2 &&
					_Column(pos, board) > 0 &&
					board.IsLegalMove(_Traverse(0, 1, pos, board)) &&
					board.IsLegalMove(_Traverse(-1, 1, pos, board));
			}

			return
				_Column(pos, board) == board.Length() - 2 &&
				_Row(pos, board) > 0 &&
				board.IsLegalMove(_Traverse(1, 0, pos, board)) &&
				board.IsLegalMove(_Traverse(1, -1, pos, board));
		}


		bool _IsPlayerColor(bool white, Hexagon h)
		{
			return (white) ? h == Hexagon::White : h == Hexagon::Black;
		}


		bool _HasPath(int pos, const Board& board, bool white, bool includeVirtual, bool* visited)
		{
			if (visited[pos] || !_IsPlayerColor(white, board[pos])) return false;
			visited[pos] = true;
			
			bool (*goal)(int, const Board&, bool) =
				(includeVirtual) ? _VirtualPathGoalState : _DirectPathGoalState;

			if (goal(pos, board, white)) return true;

			const static int xIncs[] = { -1, -1, 0,  0, 1,  1 };
			const static int yIncs[] = { 0,  1, 1, -1, 0, -1 };
			const static int xVIncs[] = { 1,  2,  1, -1, -2, -1 };
			const static int yVIncs[] = { 1, -1, -2, -1,  1,  2 };
			const static int xBlockers1[] = { 0,  1,  1,  0, -1, -1 };
			const static int yBlockers1[] = { 1,  0, -1, -1,  0,  1 };
			const static int xBlockers2[] = { 1,  1,  0, -1, -1,  0 };
			const static int yBlockers2[] = { 0, -1, -1,  0,  1,  1 };

			int neighbours[12] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

			for (int i = 0; i < 6; ++i)
			{
				if (_IsWithinBounds(xIncs[i], yIncs[i], pos, board))
					neighbours[i] = (_Traverse(xIncs[i], yIncs[i], pos, board));
			}

			if (includeVirtual)
			{
				for (int i = 0; i < 6; ++i)
				{
					if (_IsWithinBounds(xVIncs[i], yVIncs[i], pos, board))
					{
						int blocker1 = _Traverse(xBlockers1[i], yBlockers1[i], pos, board);
						int blocker2 = _Traverse(xBlockers2[i], yBlockers2[i], pos, board);
						bool isBlocked = !board.IsLegalMove(blocker1) || !board.IsLegalMove(blocker2);
						if (!isBlocked) neighbours[i + 6] = (_Traverse(xVIncs[i], yVIncs[i], pos, board));
					}
				}
			}
			
			for (int i : neighbours)
			{
				if (i != -1 && _IsPlayerColor(white, board[i]) && _HasPath(i, board, white, includeVirtual, visited))
					return true;
			}

			return false;
		}


		bool _InitVirtualSearch(const Board& board, bool white, bool* visited)
		{	
			bool win = false;
		
			if (white)
			{
				for (int i = _Traverse(0, 1, 0, board); _Column(i, board) < board.Length() - 1; i = _Traverse(1, 0, i, board))
				{
					int blocker1 = _Traverse(0, -1, i, board);
					int blocker2 = _Traverse(1, -1, i, board);
					bool isBlocked = !board.IsLegalMove(blocker1) || !board.IsLegalMove(blocker2);
					if (!isBlocked && _HasPath(i, board, true, true, visited))
						return true;
				}
			}

			else
			{
				for (int i = _Traverse(1, 0, 0, board); _Row(i, board) < board.Length() - 1; i = _Traverse(0, 1, i, board))
				{
					int blocker1 = _Traverse(-1, 0, i, board);
					int blocker2 = _Traverse(-1, 1, i, board);
					bool isBlocked = !board.IsLegalMove(blocker1) || !board.IsLegalMove(blocker2);
					if (!isBlocked && _HasPath(i, board, false, true, visited))
						return true;
				}
			}

			return false;
		}


		bool _InitSearch(const Board& board, bool white, bool includeVirtual)
		{
			static thread_local bool visited[MAX_BOARD_SIZE * MAX_BOARD_SIZE] = { false };
			for (int i = 0; i < board.Area(); ++i) visited[i] = false;

			if (white)
			{
				for (int i = 0; _Row(i, board) == 0; i = _Traverse(1, 0, i, board))
				{
					if (_HasPath(i, board, true, includeVirtual, visited))
						return true;
				}
			}

			else
			{
				for (int i = 0; _Row(i, board) < board.Length(); i = _Traverse(0, 1, i, board))
				{
					if (_HasPath(i, board, false, includeVirtual, visited))
						return true;
				}
			}

			if (includeVirtual)
			{
				return _InitVirtualSearch(board, white, visited);
			}

			return false;
		}


		WinState CheckWinState(const Board& board, bool includeVirtual)
		{
			int minToCheck = 2 * board.Length() - 1;
			if (includeVirtual) minToCheck /= 2;
			if (board.CountOccupied() < minToCheck) return WinState::Ongoing;

			//if not white to move, then white just moved so check if he won.
			//otherwise check if black won.
			bool white = !board.WhiteToMove();
			bool hasWin = _InitSearch(board, white, includeVirtual);
			if (white && hasWin) return WinState::White;
			if (!white && hasWin) return WinState::Black;
			return WinState::Ongoing;
		}
	}
}