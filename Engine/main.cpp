#include <iostream>
#include "board.h"
#include "search.h"
#include "pathfinding.h"


int main()
{
	//Demo
	Hax::Board b(11);
	while (Hax::Pathfinding::CheckWinState(b) == Hax::WinState::Ongoing)
	{
		int mov = Hax::Search::MonteCarloSearch(b, 5000, 6, 0.0f, 0.012f);
		std::cout << "Computer plays: " << mov << std::endl;
		b.MakeMove(mov);

		int x = 9999;
		while (!b.IsLegalMove(x) || x > b.Area() || x < 0)
		{
			std::cout << "Enter a move: ";
			std::cin >> x;
			std::cout << std::endl;
			b.MakeMove(x);
		}
	}

	std::cout << "Game over!" << std::endl;
}