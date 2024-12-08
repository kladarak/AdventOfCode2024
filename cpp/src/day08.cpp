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

namespace d08
{
	struct Bounds
	{
		Vec2 topLeft;
		Vec2 botRght;

		bool isInBounds(Vec2 pos) const
		{
			return topLeft.x <= pos.x
				&& topLeft.y <= pos.y
				&& pos.x < botRght.x
				&& pos.y < botRght.y;
		}
	};

	struct Data08
	{
		std::map<char, std::set<Vec2>> antennaPositions;
		Bounds bounds;
	};

	static auto loadData(const char* filename)
	{
		std::fstream s{ filename, s.in };
		assert(s.is_open());

		Data08 data;
		data.bounds.topLeft = {0,0};

		int y = 0;

		while (s.peek() != EOF)
		{
			std::string line;
			std::getline(s, line);

			assert(line.size() > 0);
			assert(data.bounds.botRght.x == 0 || data.bounds.botRght.x == (int) line.size());

			data.bounds.botRght.x = (int) line.size();

			for (int x = 0; x < (int)line.size(); ++x)
			{
				if (line[x] != '.')
					data.antennaPositions[line[x]].insert({x, y});
			}

			++y;
		}

		data.bounds.botRght.y = y;

		return data;
	}

	static void forEachNodePair(const Data08& data, auto&& functor)
	{
		for (auto antennaType : data.antennaPositions)
		{
			const std::set<Vec2>& antennaPositions = antennaType.second;

			for (auto posIter = antennaPositions.begin(); posIter != antennaPositions.end(); ++posIter)
			{
				auto posIter2 = posIter;
				++posIter2;

				for (; posIter2 != antennaPositions.end(); ++posIter2)
				{
					functor(*posIter, *posIter2);
				}
			}
		}
	}

	static uint64_t partOne(const Data08& data)
	{
		std::set<Vec2> antiNodes;

		forEachNodePair(data, [&] (Vec2 pos1, Vec2 pos2)
		{
			const Vec2 delta = pos2 - pos1;
			const Vec2 antiNode1 = pos1 - delta;
			const Vec2 antiNode2 = pos2 + delta;

			if (data.bounds.isInBounds(antiNode1))
				antiNodes.insert(antiNode1);

			if (data.bounds.isInBounds(antiNode2))
				antiNodes.insert(antiNode2);
		});

		return antiNodes.size();
	}

	static uint64_t partTwo(const Data08& data)
	{
		std::set<Vec2> antiNodes;

		forEachNodePair(data, [&] (Vec2 pos1, Vec2 pos2)
		{
			const Vec2 delta = pos2 - pos1;

			{
				Vec2 antiNode1 = pos1;
				while (data.bounds.isInBounds(antiNode1))
				{
					antiNodes.insert(antiNode1);
					antiNode1 -= delta;
				}
			}

			{
				Vec2 antiNode2 = pos2;
				while (data.bounds.isInBounds(antiNode2))
				{
					antiNodes.insert(antiNode2);
					antiNode2 += delta;
				}
			}
		});

		std::vector<bool> rowProto(data.bounds.botRght.x, false);
		std::vector<std::vector<bool>> grid(data.bounds.botRght.y, rowProto);

		for (Vec2 pos : antiNodes)
			grid[pos.y][pos.x] = true;

		for (size_t y = 0; y < grid.size(); ++y)
		{
			for (size_t x = 0; x < grid.front().size(); ++x)
			{
				std::cout << (grid[y][x] ? 'X' : '.');
			}

			std::cout << std::endl;
		}

		return antiNodes.size();
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

void day08()
{
	d08::processPrintAndAssert("../data/08/test.txt", std::make_pair(14ull, 34ull));
	d08::processPrintAndAssert("../data/08/real.txt", std::make_pair(344ull, 1182ull));
}
