/* 
 * This file is devoted to answering questions about either player's Hex path.
 * 
 * In particular, our algorithm uses a "two-bridge" heuristic, where we consider
 * An unblockable two-bridge path for either player a victory. 
 * 
 * See http://www.mseymour.ca/hex_book/hexstrat1.html for details.
 * 
 * Hex boards are preserved under rotations and flips, nonetheless
 * some tight assumptions are made about the orientation of the board
 * in these functions. 
 * We assume that the top-left corner of the board is (0, 0),
 * the bottom-right corner is (n-1, n-1), and that White plays
 * vertically, while Black plays horizontally.
 * 
 * It should be possible to reorient the board to comply with this
 * if necessary, rather than changing any logic here.
*/


#pragma once
#include <vector>
#include <set>
#include "board.h"
#include "debug.h"
#include "threadpool.h"


namespace Hax
{
	namespace Pathfinding
	{
		/*
		 * Determines whether the board has a winner.
		 * 
		 * If includeVirtual is set to true, two-bridge
		 * paths are counted as wins.
		*/
		WinState CheckWinState(const Board& board, bool includeVirtual = false);
	}
}

