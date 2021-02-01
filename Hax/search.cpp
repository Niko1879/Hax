#include "search.h"

namespace Hax
{
	namespace Search
	{
		float _Beta(float n_i, float n_hat_i, float b)
		{
			D(if (n_i == 0.0f) throw std::invalid_argument("n_i must be greater than 0"));
			float beta = n_hat_i / (n_i + n_hat_i + 4.0f * b * b * n_i * n_hat_i);
			return beta;
		}


		float _Ucb(float w_i, float n_i, float w_hat_i, float n_hat_i, float N_i, float expBias, float b)
		{
			D(if (N_i == 0.0f || n_i == 0.0f) throw std::invalid_argument("n_i must be greater than 0"));
			float beta = (n_hat_i > 0.0f) ? _Beta(n_i, n_hat_i, b) : 0.0f;
			float mc = (1.0f - beta) * (w_i / n_i) + expBias * sqrt(log(N_i) / n_i);
			float rave = (n_hat_i > 0.0f) ? (beta * (w_hat_i / n_hat_i)) : 0.0f;
			float ucb = mc + rave;
			return ucb;
		}


		struct Node
		{
			Node() : n(0.0f), nr(0.0f), w(0.0f), wr(0.0f) {}
			float n;
			float nr;
			float w;
			float wr;
		};


		using GameTree = Tree<int, Node>;


		void _MonteCarloSearch(GameTree& tree, Board board, long long maxTime, float expBias, float b)
		{
			std::random_device rd;
			std::mt19937 e{ rd() };
			maxTime *= 1000;
			long long elapsed = 0;
			std::set<int> legalMoves;
			for (int i = 0; i < board.Area(); ++i)
			{
				if (board.IsLegalMove(i))
				{
					legalMoves.insert(i);
				}
			}
			std::set<int> moveHist;
			while (elapsed < maxTime)
			{
				D(Board cpy(board));
				D(if (legalMoves.size() != board.CountUnoccupied()) throw std::logic_error("missing legal moves"));
				auto start = std::chrono::high_resolution_clock::now();
				bool hasUnvisited = false;
				while (legalMoves.size() > 0 && !hasUnvisited)
				{
					float best = -999.0f;
					int bestMove = -1;
					for (int i : legalMoves)
					{
						if (!tree.HasChild(i))
						{
							hasUnvisited = true;
							break;
						}

						Node& node = tree.Child(i);
						float ucb = _Ucb(node.w, node.n, node.wr, node.nr, tree.Data().n, expBias, b);
						if (ucb > best)
						{
							best = ucb;
							bestMove = i;
						}
					}

					if (!hasUnvisited)
					{
						tree.Descend(bestMove);
						board.MakeMove(bestMove);
						legalMoves.erase(bestMove);
						moveHist.insert(bestMove);
					}
				}

				std::vector<int> unvisited(legalMoves.size(), -1);
				size_t idxMax = 0;
				for (int i : legalMoves)
				{
					if (!tree.HasChild(i))
					{
						unvisited[idxMax++] = i;
					}
				}

				std::uniform_int_distribution<size_t> dist{ 0,  idxMax - 1 };
				if (unvisited.size() > 0)
				{
					int nextMove = unvisited[dist(e)];
					Node newNode;
					tree.Insert(nextMove, std::move(newNode));
					tree.Descend(nextMove);
					D(if (!board.IsLegalMove(nextMove)) throw std::logic_error("Overwriting board state"));
					board.MakeMove(nextMove);
					legalMoves.erase(nextMove);
					moveHist.insert(nextMove);
				}

				std::vector<int> moveOrder(legalMoves.begin(), legalMoves.end());
				std::shuffle(moveOrder.begin(), moveOrder.end(), e);
				size_t idx = 0;
				bool whiteToMove = board.WhiteToMove();
				WinState wState;

				while ((wState = Pathfinding::CheckWinState(board, true)) == WinState::Ongoing)
				{
					D(if (idx == moveOrder.size()) throw std::logic_error("Impossible state. All legal moves exhausted and no winner"));
					int next = moveOrder[idx++];
					board.MakeMove(next);
					moveHist.insert(next);
					legalMoves.erase(next);
				}

				bool isWinForNode = ((whiteToMove && wState == WinState::Black) || (!whiteToMove && wState == WinState::White));
				
				while (!tree.IsRoot())
				{
					Node& node = tree.Data();
					node.n += 1.0f;
					if (isWinForNode) node.w += 1.0f;

					tree.Ascend();
					for (int i : moveHist)
					{
						if ((!whiteToMove && board[i] == Hexagon::White) || (whiteToMove && board[i] == Hexagon::Black))
						{
							if (tree.HasChild(i))
							{
								Node& child = tree.Child(i);
								child.nr += 1.0f;
								if (isWinForNode) child.wr += 1.0f;
							}
						}
					}
					isWinForNode = !isWinForNode;
					whiteToMove = !whiteToMove;
				}

				tree.Data().n += 1.0f;

				for (int i : moveHist)
				{
					board.UndoMove(i);
					legalMoves.insert(i);
				}
				moveHist.clear();

				auto end = std::chrono::high_resolution_clock::now();
				auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
				elapsed += duration;
				D(if (!(cpy == board)) throw std::logic_error("Board should remain constant through iterations"));
			}	
		}


		int MonteCarloSearch(Board board, long long maxTime, int nthread, float expBias, float b)
		{
			Threadpool threadpool(nthread);

			std::vector<GameTree> gameTrees;
			for (int i = 0; i < nthread; ++i)
			{
				Node root;
				gameTrees.push_back(GameTree(root));
			}

			for (GameTree& tree : gameTrees)
			{
				threadpool.Submit([&tree, board, maxTime, expBias, b]()
					{
						_MonteCarloSearch(std::ref(tree), board, maxTime, expBias, b);
					});
			}

			threadpool.WaitAll();

			std::unordered_map<int, float> scores;
			for (int i = 0; i < board.Area(); ++i)
				if (board.IsLegalMove(i)) scores[i] = 0.0f;

			for (GameTree& tree : gameTrees)
			{
				for (auto& pair : scores)
				{
					if (tree.HasChild(pair.first))
					{
						pair.second += tree.Child(pair.first).n;
					}
				}
			}

			float bestScore = -999.0f;
			int bestMove = -1;
			for (auto& pair : scores)
			{
				if (pair.second > bestScore)
				{
					bestMove = pair.first;
					bestScore = pair.second;
				}
			}

			return bestMove;
		}
	}
}