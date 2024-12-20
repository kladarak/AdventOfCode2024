#include <algorithm>
#include <cassert>
#include <iostream>
#include <fstream>
#include <functional>
#include <map>
#include <numeric>
#include <optional>
#include <ranges>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>

#include "String.h"
#include "Renderer.h"
#include "Vec2.h"

namespace d20
{
	struct Data20
	{
		std::vector<Vec2> walls;
		Vec2 startPos;
		Vec2 endPos;
		EZBounds bounds;
	};

	static auto loadData(const char* filename)
	{
		Data20 data;

		int y = 0;
		for (const std::string& line : String::readLines(filename))
		{
			for (int x = 0; x < (int) line.size(); ++x)
			{
				const char c = line[x];
				if (c == '#')
					data.walls.push_back({x,y});
				else if (c == 'S')
					data.startPos = {x,y};
				else if (c == 'E')
					data.endPos = {x,y};
			}
			++y;

			if (data.bounds.width == 0)
				data.bounds.width = (int) line.size();
			else
				assert(data.bounds.width == (int) line.size());
		}

		data.bounds.height = y;

		return data;
	}

	struct Grid
	{
		std::vector<std::vector<bool>> walls;
		std::vector<std::vector<int>> distances;
		EZBounds bounds;

		Grid(const Data20& data)
			: bounds(data.bounds)
		{
			walls = std::vector<std::vector<bool>>(bounds.height, std::vector<bool>(bounds.width, false));
			distances = std::vector<std::vector<int>>(bounds.height, std::vector<int>(bounds.width, 0));

			for (const Vec2& w : data.walls)
				setWall(w);
		}

		void setWall(Vec2 pos)
		{
			walls[pos.y][pos.x] = true;
		}

		bool isWall(Vec2 pos) const
		{
			return walls[pos.y][pos.x];
		}

		void setScore(Vec2 pos, int score)
		{
			distances[pos.y][pos.x] = score;
		}

		int getScore(Vec2 pos) const
		{
			return distances[pos.y][pos.x];
		}

		void render(Renderer& renderer) const
		{
			for (int y = 0; y < bounds.height; ++y)
			{
				for (int x = 0; x < bounds.width; ++x)
				{
					const Vec2 pos{x, y};
					if (walls[y][x])
						renderer.plot(pos, '#');
					else
						renderer.plot(pos, '.');
				}
			}
		}
	};

	struct Location
	{
		Vec2 pos;
		int score = 0;

		bool operator==(const Location& r) const { return pos == r.pos && score == r.score; }
		bool operator!=(const Location& r) const { return !(*this == r); }
		bool operator<(const Location& r) const { return pos < r.pos; } // for map
	};

	struct MoreExpensive
	{
		bool operator()(const Location& l, const Location& r) const
		{
			return l.score > r.score;
		}
	};

	struct PathFinder
	{
		Location finalLocation;

		bool pathFind(Grid& grid, Vec2 startPos, Vec2 endPos)
		{
			std::priority_queue<Location, std::vector<Location>, MoreExpensive> queue;

			queue.push({startPos, 0});

			std::set<Vec2> visited;

			while (!queue.empty())
			{
				const Location loc = queue.top();
				queue.pop();

				if (visited.insert(loc.pos).second)
				{
					assert(grid.getScore(loc.pos) == 0);
					grid.setScore(loc.pos, loc.score);

					if (loc.pos == endPos)
					{
						finalLocation = loc; // found end
						return true;
					}

					// generate iterations
					for (Vec2 dir : Vec2::directions)
					{
						const Location next = {loc.pos + dir, loc.score + 1};
						if (grid.bounds.isInBounds(next.pos)
							&& !grid.isWall(next.pos)
							&& grid.getScore(next.pos) == 0)
							queue.push(next);
					}
				}
			}

			return false;
		}
	};

	static uint64_t partOne([[maybe_unused]] const Data20& data)
	{
		Grid grid(data);
		PathFinder pathFinder;
		const bool success = pathFinder.pathFind(grid, data.startPos, data.endPos);
		assert(success);

		uint64_t cheatsOver100 = 0;
		std::map<int, int> cheatCounts;

		for (Vec2 wallPos : data.walls)
		{
			for (int i = 0; i < (int) Direction::Count - 1; ++i)
			{
				const Vec2 n1 = wallPos + Vec2::directions[i];
				if (!grid.bounds.isInBounds(n1) || grid.isWall(n1))
					continue;

				for (int j = i + 1; j < (int) Direction::Count; ++j)
				{
					const Vec2 n2 = wallPos + Vec2::directions[j];
					if (!grid.bounds.isInBounds(n2) || grid.isWall(n2))
						continue;

					const int score1 = grid.getScore(n1);
					const int score2 = grid.getScore(n2);
					const int diff = abs(score1 - score2);
					const int saving = diff - 2; // takes 2 picoseconds to cheat
					cheatCounts[saving]++;
					if (saving >= 100)
						++cheatsOver100;
				}
			}
		}

		return cheatsOver100;
	}

	static uint64_t partTwo([[maybe_unused]] const Data20& data)
	{
		Grid grid(data);
		PathFinder pathFinder;
		const bool success = pathFinder.pathFind(grid, data.startPos, data.endPos);
		assert(success);

		uint64_t cheatsOver100 = 0;

		// Does a kernel search around each position.
		for (int py = 0; py < grid.bounds.height; ++py)
		{
			for (int px = 0; px < grid.bounds.width; ++px)
			{
				const Vec2 pos = {px, py};
				if (grid.isWall(pos))
					continue;

				const int kyMin = py; // earlier rows searched for us
				const int kyMax = std::min(py + 20, grid.bounds.height - 1);
				for (int ky = kyMin; ky <= kyMax; ++ky)
				{
					const int kxMin = std::max(px - 20, 0);
					const int kxMax = std::min(px + 20, grid.bounds.width - 1);
					for (int kx = kxMin; kx <= kxMax; ++kx)
					{
						if (ky == py && kx <= px)
							continue; // these first few cells are earlier and have already searched this pos.

						const Vec2 target = {kx, ky};
						assert(grid.bounds.isInBounds(target));
						if (grid.isWall(target))
							continue;

						const int cheatCost = (pos - target).manhattan();
						if (cheatCost > 20)
							continue; // only consider circular kernel

						const int posScore = grid.getScore(pos);
						const int targetScore = grid.getScore(target);
						const int saving = abs(posScore - targetScore) - cheatCost;
						if (saving >= 100)
							++cheatsOver100;
					}
				}
			}
		}

		return cheatsOver100;
	}

	static void processPrintAndAssert(const char* filename, auto expected1, auto expected2)
	{
		const auto data = loadData(filename);
		const auto result1 = partOne(data);
		const auto result2 = partTwo(data);

		std::cout << "Part 1: " << result1 << " Part 2: " << result2 << std::endl;

		assert(expected1 == decltype(expected1){} || result1 == expected1);
		assert(expected2 == decltype(expected2){} || result2 == expected2);
	}
}

void day20()
{
	d20::processPrintAndAssert("../data/20/test.txt", 0ull, 0ull);
	d20::processPrintAndAssert("../data/20/real.txt", 1459ull, 1016066ull);
}
