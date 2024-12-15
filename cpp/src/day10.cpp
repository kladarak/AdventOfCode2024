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

namespace d10
{
	struct Data10
	{
		std::vector<std::vector<uint8_t>> grid;
		Bounds bounds;
	};

	static auto loadData(const char* filename)
	{
		std::fstream s{ filename, s.in };
		assert(s.is_open());

		Data10 data;

		while (s.peek() != EOF)
		{
			std::string line;
			std::getline(s, line);

			std::vector<uint8_t> row;
			for (char c : line)
				row.push_back(c - '0');

			data.grid.push_back(std::move(row));
			assert(data.grid.front().size() == data.grid.back().size());
		}

		data.bounds.botRght.x = (int) data.grid.front().size();
		data.bounds.botRght.y = (int) data.grid.size();

		return data;
	}

	static uint64_t walkTrails(const Data10& data, const Vec2 pos, const uint8_t height, std::set<Vec2>& peaks)
	{
		if (height == 9)
		{
			peaks.insert(pos);
			return 1;
		}

		const uint8_t next = height + 1;
		uint64_t rating = 0;

		for (const Vec2 delta : Vec2::directions)
		{
			const Vec2 neighbour = pos + delta;
			if (data.bounds.isInBounds(neighbour))
				if (data.grid[neighbour.y][neighbour.x] == next)
					rating += walkTrails(data, neighbour, next, peaks);
		}

		return rating;
	}

	static uint64_t partOne(const Data10& data)
	{
		uint64_t sum = 0;
		for (int row = 0; row < data.bounds.botRght.y; ++row)
		{
			for (int col = 0; col < data.bounds.botRght.x; ++col)
			{
				if (data.grid[row][col] == 0)
				{
					std::set<Vec2> peaks;
					walkTrails(data, {col, row}, 0, peaks);
					sum += peaks.size();
				}
			}
		}

		return sum;
	}

	static uint64_t partTwo(const Data10& data)
	{
		uint64_t sum = 0;
		for (int row = 0; row < data.bounds.botRght.y; ++row)
		{
			for (int col = 0; col < data.bounds.botRght.x; ++col)
			{
				if (data.grid[row][col] == 0)
				{
					std::set<Vec2> peaks;
					sum += walkTrails(data, {col, row}, 0, peaks);
				}
			}
		}

		return sum;
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

void day10()
{
	d10::processPrintAndAssert("../data/10/test.txt", std::make_pair(36ull, 81ull));
	d10::processPrintAndAssert("../data/10/real.txt", std::make_pair(535ull, 1186ull));
}
