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
#include <array>

#include "String.h"
#include "Renderer.h"
#include "Vec2.h"

namespace d25
{
	struct KeyOrLock
	{
		std::array<uint8_t, 5> heights;

		KeyOrLock()
		{
			heights.fill(0);
		}

		bool canFit(const KeyOrLock& anOther) const
		{
			for (int i = 0; i < heights.size(); ++i)
			{
				if (heights[i] + anOther.heights[i] > 5)
					return false;
			}

			return true;
		}
	};

	struct Data25
	{
		std::vector<KeyOrLock> keys;
		std::vector<KeyOrLock> locks;
	};

	static auto loadData(const char* filename)
	{
		Data25 data;

		std::vector<std::vector<std::string>> groups;
		std::vector<std::string>* currentGroup = nullptr;

		for (const std::string& line : String::readLines(filename))
		{
			if (line.empty())
			{
				currentGroup = nullptr;
				continue;
			}
			else if (!currentGroup)
			{
				groups.push_back({});
				currentGroup = &groups.back();
			}

			currentGroup->push_back(line);
		}

		for (auto& group : groups)
		{
			KeyOrLock* target = nullptr;

			if (group.front().front() == '#')
			{
				data.locks.push_back({});
				target = &data.locks.back();
			}
			else
			{
				data.keys.push_back({});
				target = &data.keys.back();
			}

			for (size_t i = 1; i < group.size() - 1; ++i)
			{
				const std::string& line = group[i];

				for (size_t j = 0; j < line.size(); ++j)
					if (line[j] == '#')
						target->heights[j]++;
			}
		}

		return data;
	}

	static uint64_t partOne(const Data25& data)
	{
		uint64_t sum = 0;

		for (const auto& key : data.keys)
			for (const auto& lock : data.locks)
				if (key.canFit(lock))
					++sum;

		return sum;
	}

	static uint64_t partTwo([[maybe_unused]] const Data25& data)
	{
		return 0;
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

void day25()
{
	d25::processPrintAndAssert("../data/25/test.txt", 3ull, 0ull);
	d25::processPrintAndAssert("../data/25/real.txt", 3508ull, 0ull);
}
