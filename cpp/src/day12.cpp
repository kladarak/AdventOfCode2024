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

namespace d12
{
	struct Data12
	{
		std::vector<std::string> grid;
		Bounds bounds;
	};

	struct Edge
	{
		Vec2 from;
		Vec2 to;
		bool inside = false;

		bool operator==(const Edge& r) const
		{
			return from == r.from && to == r.to && inside == r.inside;
		}

		bool operator!=(const Edge& r) const
		{
			return !(*this == r);
		}
	};

	struct Region
	{
		char type = 0;
		std::vector<std::pair<Vec2, Vec2>> perimeter;
		uint64_t area = 0;

		size_t numSides(const Data12& data) const
		{
			std::vector<Edge> horizontal;
			std::vector<Edge> vertical;

			for (const auto& fence : perimeter)
			{
				if (fence.first.x == fence.second.x)
				{
					Edge e;
					e.from.x = fence.first.x;
					e.from.y = std::max(fence.first.y, fence.second.y);
					e.to.x = e.from.x + 1;
					e.to.y = e.from.y;
					e.inside = data.bounds.isInBounds(e.from) && (data.grid[e.from.y][e.from.x] == type);
					horizontal.push_back(e);
				}
				else
				{
					Edge e;
					e.from.x = std::max(fence.first.x, fence.second.x);
					e.from.y = fence.first.y;
					e.to.x = e.from.x;
					e.to.y = e.from.y + 1;
					e.inside = data.bounds.isInBounds(e.from) && (data.grid[e.from.y][e.from.x] == type);
					vertical.push_back(e);
				}
			}

			std::sort(horizontal.begin(), horizontal.end(), [] (const Edge& l, const Edge& r)
			{
				if (l.from.y != r.from.y)
					return l.from.y < r.from.y;
				else
					return l.from.x < r.from.x;
			});

			std::sort(vertical.begin(), vertical.end(), [] (const Edge& l, const Edge& r)
			{
				if (l.from.x != r.from.x)
					return l.from.x < r.from.x;
				else
					return l.from.y < r.from.y;
			});

			size_t sides = 0;

			{
				++sides;

				for (size_t i = 0; i < horizontal.size() - 1; ++i)
				{
					const Edge& curr = horizontal[i];
					const Edge& next = horizontal[i+1];
					if (curr.to != next.from || curr.inside != next.inside)
						sides++;
				}
			}

			{
				++sides;

				for (size_t i = 0; i < vertical.size() - 1; ++i)
				{
					const Edge curr = vertical[i];
					const Edge next = vertical[i+1];
					if (curr.to != next.from || curr.inside != next.inside)
						sides++;
				}
			}

			return sides;
		}
	};

	struct Grid
	{
		const Data12& data;
		std::vector<std::vector<Region*>> grid;
		std::vector<std::unique_ptr<Region>> regions;

		explicit Grid(const Data12& data)
			: data(data)
		{
			std::vector<Region*> protoRow(data.bounds.botRght.x, nullptr);
			grid = std::vector<std::vector<Region*>>(data.bounds.botRght.y, protoRow);

			fill();
		}

		void fill()
		{
			for (int row = 0; row < data.bounds.botRght.y; ++row)
			{
				for (int col = 0; col < data.bounds.botRght.x; ++col)
				{
					if (grid[row][col])
						continue;

					regions.push_back(std::make_unique<Region>());
					Region& region = *regions.back();
					region.type = data.grid[row][col];
					floodFill(region, {col, row});
				}
			}
		}

		void floodFill(Region& region, Vec2 pos)
		{
			assert(grid[pos.y][pos.x] == nullptr);
			grid[pos.y][pos.x] = &region;
			++region.area;

			for (Vec2 dir : Vec2::directions)
			{
				const Vec2 next = pos + dir;
				if (!data.bounds.isInBounds(next))
				{
					region.perimeter.emplace_back(pos, next);
				}
				else if (Region* r = grid[next.y][next.x])
				{
					if (r->type != region.type)
					{
						r->perimeter.emplace_back(pos, next);
						region.perimeter.emplace_back(pos, next);
					}
				}
				else if (data.grid[next.y][next.x] == region.type)
				{
					floodFill(region, next);
				}
			}
		}
	};

	static auto loadData(const char* filename)
	{
		std::fstream s{ filename, s.in };
		assert(s.is_open());

		Data12 data;

		while (s.peek() != EOF)
		{
			std::string line;
			std::getline(s, line);
			assert(line.size() > 0);

			data.grid.push_back(line);
			assert(data.grid.front().size() == data.grid.back().size());
		}

		data.bounds.botRght.x = (int) data.grid.front().size();
		data.bounds.botRght.y = (int) data.grid.size();

		return data;
	}

	static uint64_t partOne(const Data12& data)
	{
		const Grid grid(data);

		uint64_t cost = 0;

		for (auto& region : grid.regions)
			cost += region->area * region->perimeter.size();

		return cost;
	}

	static uint64_t partTwo(const Data12& data)
	{
		const Grid grid(data);

		uint64_t cost = 0;

		for (auto& region : grid.regions)
			cost += region->area * region->numSides(data);

		return cost;
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

void day12()
{
	d12::processPrintAndAssert("../data/12/test.txt", std::make_pair(140ull, 80ull));
	d12::processPrintAndAssert("../data/12/test2.txt", std::make_pair(772ull, 436ull));
	d12::processPrintAndAssert("../data/12/test3.txt", std::make_pair(1930ull, 1206ull));
	d12::processPrintAndAssert("../data/12/test4.txt", std::make_pair(692ull, 236ull));
	d12::processPrintAndAssert("../data/12/test5.txt", std::make_pair(1184ull, 368ull));
	d12::processPrintAndAssert("../data/12/real.txt", std::make_pair(1434856ull, 891106ull));
}
