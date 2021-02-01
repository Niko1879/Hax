/*
 * Functinons to search the game tree for the next best move.
 *
 * Currently only MonteCarlo tree search with the AMAF heuristic
 * is implemented.
 * 
 */


#pragma once
#include "tree.h"
#include "board.h"
#include "pathfinding.h"
#include "threadpool.h"
#include <time.h>
#include <chrono>
#include <random>
#include <set>
#include <algorithm>


namespace Hax
{
	namespace Search
	{
		/* 
		 * Returns the next AI move for the given board state.
		 * 
		 * Board: Instance of Board class representing the current board state
		 * 
		 * maxTime: Max time allowed in milliseconds.
		 * 
		 * nthread: Number of parallel executions. For best results set to maximum
		 *          cpu cores. If you set it any higher, the algorithm will likely
		 *          overrun the maximum time allotted. 
		 * 
		 * expBias: MonteCarlo tuning parameter. A higher value will favour exploring
		 *          a variety of moves, while a lower value hones in on the most 
		 *          promising moves. The AMAF heuristic takes care of most of the
		 *          exploration so you probably want to keep this at 0.
		 * 
		 * b: MonteCarlo tuning parameter. Lower values favour the AMAF heuristic 
		 *    while higher values favour the pure MonteCarlo statistics.
		 *    If you desire more exploration of different moves, tune this rather
		 *    than expBias.
		*/
		int MonteCarloSearch(Board board, long long maxTime, int nthread, float expBias = 0.0f, float b = 1.0f);
	}
}

