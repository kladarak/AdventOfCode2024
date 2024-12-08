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
#include <string>
#include <vector>
#include <unordered_map>
#include "Vec2.h"

namespace d06
{
	enum class Direction
	{
		Up,
		Right,
		Down,
		Left,
	};

	static const Vec2 deltas[] =
	{
		{0, -1},
		{1, 0},
		{0, 1},
		{-1, 0},
	};

	struct Grid
	{
		std::vector<std::vector<bool>> cells;

		bool isInBounds(Vec2 pos) const
		{
			constexpr int minX = 0;
			constexpr int minY = 0;
			const int maxX = (int) cells.front().size();
			const int maxY = (int) cells.size();

			return minX <= pos.x
				&& minY <= pos.y
				&& pos.x < maxX
				&& pos.y < maxY;
		}

		bool isObstacle(Vec2 pos) const
		{
			return cells[pos.y][pos.x];
		}
	};

	struct Data06
	{
		Grid grid;
		Vec2 start;
	};

	static auto loadData(const char* filename)
	{
		std::fstream s{ filename, s.in };
		assert(s.is_open());

		Data06 data;

		while (s.peek() != EOF)
		{
			std::string line;
			std::getline(s, line);
			assert(line.size() > 0);

			data.grid.cells.push_back({});
			std::vector<bool>& row = data.grid.cells.back();

			for (char c : line)
			{
				if (c == '.')
					row.push_back(false);
				else if (c == '#')
					row.push_back(true);
				else if (c == '^')
				{
					row.push_back(false);
					data.start = {(int) row.size() - 1, (int) data.grid.cells.size() - 1};
				}
				else
					assert(false);
			}

			assert(data.grid.cells.front().size() == row.size());
		}

		return data;
	}

	std::optional<size_t> walkUntilLoopOrExit(const Grid& grid, const Vec2 start, std::set<Vec2>& visited)
	{
		std::set<std::pair<Vec2, Direction>> loopDetect;

		Direction dir = Direction::Up;
		Vec2 delta = deltas[(int) dir];
		Vec2 pos = start;

		visited.insert(pos);
		loopDetect.insert({pos, dir});

		while (true)
		{
			const Vec2 next = pos + delta;
			if (!grid.isInBounds(next))
			{
				pos = next;
				break;
			}
			else if (loopDetect.contains({next, dir}))
			{
				break;
			}

			if (!grid.isObstacle(next))
			{
				pos = next;
				visited.insert(pos);
				loopDetect.insert({pos, dir});
			}
			else
			{
				dir = Direction(((int) dir + 1) % 4);
				delta = deltas[(int) dir];
			}
		}

		if (grid.isInBounds(pos))
			return {};
		else
			return visited.size();
	}

	static uint64_t partOne(const Data06& data)
	{
		std::set<Vec2> visited;
		return walkUntilLoopOrExit(data.grid, data.start, visited).value();
	}

	static uint64_t partTwo(const Data06& data)
	{
		std::set<Vec2> possObsLocs;
		walkUntilLoopOrExit(data.grid, data.start, possObsLocs);

		uint64_t numPossibleNewObstacleLocs = 0;

		for (const Vec2 loc : possObsLocs)
		{
			if (loc == data.start)
				continue;

			Grid grid = data.grid;
			grid.cells[loc.y][loc.x] = true;

			std::set<Vec2> visited;
			if (!walkUntilLoopOrExit(grid, data.start, visited))
				++numPossibleNewObstacleLocs;
		}

		return numPossibleNewObstacleLocs;
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

void day06()
{
	d06::processPrintAndAssert("../data/06/test.txt", std::make_pair(41ull, 6ull));
	d06::processPrintAndAssert("../data/06/real.txt", std::make_pair(4665ull, 1688ull));
}
