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
#include <queue>

#include "Vec2.h"
#include "Renderer.h"

namespace d18
{
	struct Data18
	{
		std::vector<Vec2> corrupted;
		Bounds bounds;
		uint32_t partOneCorruptionCount = 0;
	};

	static auto loadData(const char* filename)
	{
		std::fstream s{ filename, s.in };
		assert(s.is_open());

		Data18 data;
		int row = 0;

		while (s.peek() != EOF)
		{
			std::string line;
			std::getline(s, line);
			assert(line.size() > 0);

			if (row == 0)
			{
				std::vector<int> coords;

				for (const auto word : std::views::split(line, '*'))
				{
					const std::string token(&*word.begin(), std::ranges::distance(word));
					coords.push_back(std::stol(token) + 1);
				}

				assert(coords.size() == 2);
				data.bounds.botRght = Vec2{coords[0], coords[1]};
			}
			else if (row == 1)
			{
				data.partOneCorruptionCount = std::stol(line);
			}
			else
			{
				std::vector<int> coords;

				for (const auto word : std::views::split(line, ','))
				{
					const std::string token(&*word.begin(), std::ranges::distance(word));
					coords.push_back(std::stol(token));
				}

				assert(coords.size() == 2);
				data.corrupted.emplace_back(Vec2{coords[0], coords[1]});
			}

			++row;
		}

		return data;
	}

	struct Grid
	{
		std::vector<std::vector<bool>> corrupted;
		std::vector<std::vector<int>> distances;
		Bounds bounds;
		int width;
		int height;

		explicit Grid(Bounds bounds)
			: bounds(bounds)
			, width(bounds.botRght.x)
			, height(bounds.botRght.y)
		{
			corrupted = std::vector<std::vector<bool>>(height, std::vector<bool>(width, false));
			distances = std::vector<std::vector<int>>(height, std::vector<int>(width, false));
		}

		void resetScores()
		{
			distances = std::vector<std::vector<int>>(height, std::vector<int>(width, false));
		}

		void corrupt(Vec2 pos)
		{
			corrupted[pos.y][pos.x] = true;
		}

		bool isCorrupted(Vec2 pos) const
		{
			return corrupted[pos.y][pos.x];
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
			for (int y = 0; y < height; ++y)
			{
				for (int x = 0; x < width; ++x)
				{
					const Vec2 pos{x, y};
					if (corrupted[y][x])
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
		bool operator<(const Location& r) const
		{
			if (pos != r.pos) return pos < r.pos;
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

	struct PathFinder
	{
		Location finalLocation;

		bool pathFind(Grid& grid)
		{
			std::priority_queue<Location, std::vector<Location>, MoreExpensive> queue;

			queue.push({{0,0}, 0});

			std::set<Vec2> visited;
			const Vec2 target = {grid.width - 1, grid.height - 1};

			while (!queue.empty())
			{
				const Location loc = queue.top();
				queue.pop();

				if (visited.insert(loc.pos).second)
				{
					assert(grid.getScore(loc.pos) == 0);
					grid.setScore(loc.pos, loc.score);

					if (loc.pos == target)
					{
						finalLocation = loc; // found end
						return true;
					}

					// generate iterations
					for (Vec2 dir : Vec2::directions)
					{
						const Location next = {loc.pos + dir, loc.score + 1};
						if (grid.bounds.isInBounds(next.pos)
							&& !grid.isCorrupted(next.pos)
							&& grid.getScore(next.pos) == 0)
							queue.push(next);
					}
				}
			}

			return false;
		}
	};

	void renderGridAndPath(const Grid& grid, Vec2 startPos, int score)
	{
		Renderer renderer(grid.width, grid.height);
		renderer.clearScreen();
		grid.render(renderer);

		Vec2 pos = startPos;
		renderer.plot(pos, 'O');
		for (int i = score - 1; i >= 0; --i)
		{
			Vec2 curr = pos;
			for (Vec2 dir : Vec2::directions)
			{
				const Vec2 next = pos + dir;
				if (grid.bounds.isInBounds(pos + dir)
					&& !grid.isCorrupted(next)
					&& grid.getScore(next) == i)
				{
					pos = next;
					renderer.plot(pos, 'O');
				}
			}
			assert(curr != pos);
		}
		renderer.render();
		renderer.waitForInput();
	}

	static uint64_t partOne(const Data18& data)
	{
		Grid grid(data.bounds);

		for (uint32_t i = 0; i < data.partOneCorruptionCount; ++i)
			grid.corrupt(data.corrupted[i]);

		PathFinder pathFinder;
		pathFinder.pathFind(grid);

		const Location finalLoc = pathFinder.finalLocation;
		//renderGridAndPath(grid, finalLoc.pos, finalLoc.score);

		return finalLoc.score;
	}

	static std::string partTwo(const Data18& data)
	{
		Grid grid(data.bounds);

		//int attempt = 0;
		for (Vec2 pos : data.corrupted)
		{
			//std::cout << "Attempt: " << attempt++ << std::endl;

			grid.corrupt(pos);
			grid.resetScores();

			if (!PathFinder().pathFind(grid))
				return std::to_string(pos.x) + "," + std::to_string(pos.y);
		}

		return "";
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

void day18()
{
	d18::processPrintAndAssert("../data/18/test.txt", 22ull, "6,1");
	d18::processPrintAndAssert("../data/18/real.txt", 280ull, "28,56");
}
