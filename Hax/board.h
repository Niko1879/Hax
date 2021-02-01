
/* Class modelling a n by n grid of hexagons (Hex board).
 * 
 * Everything is int type because we often need to perform
 * arithmetic on board properties.
 * 
 * Throughout we denote White to be the first player to move.
 * 
 * Board is modelled as a 1D array for memory/cpu efficiency.
*/


#pragma once
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include "debug.h"


namespace Hax
{
	const int MAX_BOARD_SIZE = 20;

		enum class Hexagon
		{
			Unoccupied,
			White,
			Black
		};


		enum class WinState
		{
			Ongoing,
			White,
			Black
		};


		class Board
		{
		public:
			Board(int length);

			friend bool operator==(const Board& l, const Board& r);

			const Hexagon& operator[](int index) const;

			int Length() const;
			
			int Area() const;

			//Returns the total number of hexagons placed by both players inclusive
			int CountOccupied() const;

			//Returns the total number of legal moves
			int CountUnoccupied() const;

			//Returns true if it is White's turn to move
			bool WhiteToMove() const;

			//If it is White's turn to move, places a White hexagon at board index
			//Otherwise places a Black hexagon at board index
			void MakeMove(int move);

			void UndoMove(int move);

			bool IsLegalMove(int move) const;

		private:
			std::vector<Hexagon> board;
			int length;
			int area;
			int numOccupied;
			bool whiteToMove;
		};



}

