#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <functional>
#include <map>
#include <numeric>
#include <optional>
#include <ranges>
#include <set>
#include <unordered_set>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>

#include "Vec2.h"
#include "Renderer.h"

namespace d16
{
	struct Data16
	{
		std::vector<std::vector<bool>> grid;
		Vec2 startPos;
		Vec2 endPos;
	};

	static auto loadData(const char* filename)
	{
		std::fstream s{ filename, s.in };
		assert(s.is_open());

		Data16 data;

		while (s.peek() != EOF)
		{
			std::string line;
			std::getline(s, line);
			assert(line.size() > 0);

			data.grid.push_back({});

			for (char c : line)
			{
				const bool wall = (c == '#');
				data.grid.back().push_back(wall);

				const Vec2 pos = {
					(int) data.grid.back().size() - 1,
					(int) data.grid.size() - 1
				};

				if (c == 'S')
					data.startPos = pos;
				else if (c == 'E')
					data.endPos = pos;
			}

			assert(data.grid.front().size() == data.grid.back().size());
		}

		return data;
	}

	struct Location
	{
		Vec2 pos;
		Direction dir = Direction::Count;
		uint64_t score = 0;

		Location getRotateLeft() const
		{
			Location loc = *this;
			loc.score += 1000;
			loc.dir = Direction(((int) dir + 3) % 4);
			return loc;
		}

		Location getRotateRight() const
		{
			Location loc = *this;
			loc.score += 1000;
			loc.dir = Direction(((int) dir + 1) % 4);
			return loc;
		}

		Location getForward() const
		{
			Location loc = *this;
			loc.score += 1;
			loc.pos += Vec2::getDirection(dir);
			return loc;
		}

		bool operator==(const Location& r) const { return pos == r.pos && dir == r.dir && score == r.score; }
		bool operator!=(const Location& r) const { return !(*this == r); }
		bool operator<(const Location& r) const
		{
			if (pos != r.pos) return pos < r.pos;
			if (dir != r.dir) return dir < r.dir;
			//if (score != r.score) return score < r.score;
			return false;
		}
	};

	struct MoreExpensive
	{
		bool operator()(const Location& l, const Location& r) const
		{
			return l.score > r.score;
		}
	};

	void renderGrid(Renderer& renderer, const Data16& data, const Location& loc)
	{
		renderer.clear();
		for (int y = 0; y < (int) data.grid.size(); ++y)
		{
			for (int x = 0; x < (int) data.grid.front().size(); ++x)
			{
				const Vec2 pos{x, y};
				if (pos == loc.pos)
				{
					if (loc.dir == Direction::Up)
						renderer.plot(pos, '^');
					else if (loc.dir == Direction::Left)
						renderer.plot(pos, '<');
					else if (loc.dir == Direction::Right)
						renderer.plot(pos, '>');
					else if (loc.dir == Direction::Down)
						renderer.plot(pos, 'v');
				}
				else if (pos == data.startPos)
					renderer.plot(pos, 'S');
				else if (pos == data.endPos)
					renderer.plot(pos, 'E');
				else if (data.grid[y][x])
					renderer.plot(pos, '#');
				else
					renderer.plot(pos, '.');
			}
		}
		renderer.render();
		renderer.waitForInput();
	}

	struct Edge
	{
		Vec2 from{-1,-1};
		Vec2 to{-1,-1};
		uint64_t score = 0;
	};

	struct Node
	{
		std::vector<Edge> inEdges;
		std::vector<Edge> outEdges;

		uint64_t getMaxOutVisitedScore(const std::set<Vec2>& visited) const
		{
			uint64_t score = 0;
			for (const Edge& e : outEdges)
				if (visited.find(e.to) != visited.end())
					score = std::max(score, e.score);

			return score;
		}

		uint64_t getMinInScore() const
		{
			uint64_t score = UINT64_MAX;
			for (const Edge& e : inEdges)
				score = std::min(score, e.score);

			return score;
		}

	};

	struct PathFinder
	{
		Location finalLocation;
		std::vector<std::vector<Node>> nodes;

		void pathFind(const Data16& data)
		{
			nodes.resize(data.grid.size(), std::vector<Node>(data.grid.front().size()));

			std::priority_queue<Location, std::vector<Location>, MoreExpensive> queue;

			queue.push({data.startPos, Direction::Right, 0});

			std::set<Location> visited;

			Renderer renderer(data.grid.front().size(), data.grid.size());

			while (true)
			{
				const Location loc = queue.top();
				queue.pop();

				if (finalLocation.dir != Direction::Count
					&& finalLocation.score < loc.score)
					break; // stop searching, paths are now longer.

				if (visited.insert(loc).second)
				{
					if (data.grid[loc.pos.y][loc.pos.x])
						continue; // wall

					if (loc.pos == data.endPos)
					{
						finalLocation = loc; // found end
						continue; // keep searching to find other paths to end.
					}

					// generate iterations
					const Location left = loc.getRotateLeft();
					const Location right = loc.getRotateRight();
					const Location forward = loc.getForward();

					queue.push(left);
					queue.push(right);

					// if forward is not a wall
					if (!data.grid[forward.pos.y][forward.pos.x])
					{
						queue.push(forward);

						Node& node = nodes[loc.pos.y][loc.pos.x];
						Node& nextNode = nodes[forward.pos.y][forward.pos.x];

						const Edge edge = {loc.pos, forward.pos, forward.score};
						node.outEdges.push_back(edge);
						nextNode.inEdges.push_back(edge);
					}
				}
				//renderGrid(renderer, data, loc);
			}
		}
	};

	static uint64_t partOne(const Data16& data)
	{
		PathFinder finder;
		finder.pathFind(data);
		return finder.finalLocation.score;
	}

	static uint64_t partTwo(const Data16& data)
	{
		PathFinder finder;
		finder.pathFind(data);

		const Bounds bounds{{0,0}, {(int) data.grid.front().size(), (int) data.grid.size()}};
		uint64_t numTiles = 0;

		std::vector<Vec2> searchQueue;
		std::set<Vec2> visited;
		searchQueue.push_back(finder.finalLocation.pos);

		Renderer renderer(bounds);
		const int scoreWidth = 6;
		Renderer rendererScores(bounds.botRght.x * scoreWidth, bounds.botRght.y);

		for (int y = 0; y < (int) data.grid.size(); ++y)
		{
			for (int x = 0; x < (int) data.grid.front().size(); ++x)
			{
				const Vec2 pos{x, y};

				{
					if (pos == data.startPos)
						renderer.plot(pos, 'S');
					else if (pos == data.endPos)
						renderer.plot(pos, 'E');
					else 
						if (data.grid[y][x])
						//renderer.plot(pos, '#');
						renderer.plot(pos, (int) 219 - 256);
					else
						renderer.plot(pos, ' ');
				}

				const uint64_t score = finder.nodes[pos.y][pos.x].getMinInScore();
				const bool isScoreSet = score != UINT64_MAX;
				const std::string scoreStr = isScoreSet ? std::to_string(score) : std::string();

				for (int i = 0; i < scoreWidth; ++i)
				{
					const Vec2 scorePos{x*scoreWidth + i, y};
					if (i == scoreWidth - 1)
						rendererScores.plot(scorePos, ' ');
					else if (pos == data.startPos)
						rendererScores.plot(scorePos, 'S');
					else if (pos == data.endPos)
						rendererScores.plot(scorePos, 'E');
					else if (data.grid[y][x])
						rendererScores.plot(scorePos, '#');
					else if (isScoreSet)
						rendererScores.plot(scorePos, i < (int) scoreStr.size() ? scoreStr[i] : ' ');
					else
						rendererScores.plot(scorePos, '.');
				}
			}
		}

		for (size_t i = 0; i < searchQueue.size(); ++i)
		{
			const Vec2 pos = searchQueue[i];

			if (!visited.insert(pos).second)
				continue;

			++numTiles;
			//renderer.plot(pos, 'O');
			renderer.plot(pos, (char) (250 - 256));
			
			const Node& node = finder.nodes[pos.y][pos.x];
			const uint64_t currScore = pos == finder.finalLocation.pos ? finder.finalLocation.score : node.getMaxOutVisitedScore(visited);
			assert(currScore != 0);

			for (const Edge& e : node.inEdges)
			{
				if (e.score <= currScore)
					searchQueue.push_back(e.from);
			}
		}

		renderer.clearScreen();
		renderer.render();
		rendererScores.render(false);

		if (numTiles == 505)
			std::cout << "BUG BUG BUG: An extra path near the start was chosen as a valid path but is incorrect." << '\n';

		renderer.waitForInput();

		return numTiles;
	}

	static std::pair<uint64_t, uint64_t> process(const char* filename)
	{
		const auto data = loadData(filename);
		return std::make_pair(partOne(data), partTwo(data));
	}

	static void processPrintAndAssert(const char* filename, std::pair<uint64_t, uint64_t> expected)
	{
		const auto result = process(filename);
		std::cout << "Part 1: " << result.first << " Part 2: " << result.second << std::endl;

		assert(expected.first == 0 || result.first == expected.first);
		assert(expected.second == 0 || result.second == expected.second);
	}
}

void day16()
{
	d16::processPrintAndAssert("../data/16/test.txt", std::make_pair(7036ull, 45ull));
	d16::processPrintAndAssert("../data/16/test2.txt", std::make_pair(11048ull, 64ull));
	d16::processPrintAndAssert("../data/16/real.txt", std::make_pair(99448ull, 498ull));
}
