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

namespace d11
{
	struct Data11
	{
		std::vector<uint64_t> stones;
	};

	static auto loadData(const char* filename)
	{
		std::fstream s{ filename, s.in };
		assert(s.is_open());

		Data11 data;

		while (s.peek() != EOF)
		{
			std::string line;
			std::getline(s, line);

			for (const auto word : std::views::split(line, ' '))
			{
				const std::string token(&*word.begin(), std::ranges::distance(word));
				data.stones.push_back(std::stoull(token));
			}
		}

		return data;
	}

	static uint64_t getNumStonesAfterNBlinks(const uint64_t stoneId, const uint64_t n)
	{
		if (n == 0)
			return 1;

		using Key = std::pair<uint64_t, uint64_t>;

		static std::map<Key, uint64_t> memoized;

		const Key key{stoneId, n};
		auto iter = memoized.find(key);
		if (iter != memoized.end())
			return iter->second;

		std::optional<uint64_t> leftId;
		std::optional<uint64_t> rightId;

		if (stoneId == 0)
		{
			leftId = 1;
		}
		else
		{
			const std::string asStr = std::to_string(stoneId);
			if (asStr.size() % 2 == 0)
			{
				const size_t splitPos = asStr.size() / 2;
				const std::string left = asStr.substr(0, splitPos);
				const std::string right = asStr.substr(splitPos, splitPos);
				leftId = std::stoull(left);
				rightId = std::stoull(right);
			}
			else
			{
				leftId = stoneId * 2024;
				assert(leftId.value() > stoneId); // catch overflow
			}
		}

		assert(leftId);
		uint64_t count = getNumStonesAfterNBlinks(leftId.value(), n - 1);
		if (rightId)
			count += getNumStonesAfterNBlinks(rightId.value(), n - 1);

		memoized[key] = count;
		return count;
	}

	static uint64_t partOne(const Data11& data)
	{
		uint64_t sum = 0;
		
		for (uint64_t stone : data.stones)
			sum += getNumStonesAfterNBlinks(stone, 25);

		return sum;
	}

	static uint64_t partTwo(const Data11& data)
	{
		uint64_t sum = 0;

		for (uint64_t stone : data.stones)
			sum += getNumStonesAfterNBlinks(stone, 75);

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

void day11()
{
	d11::processPrintAndAssert("../data/11/test.txt", std::make_pair(55312ull, 65601038650482ull));
	d11::processPrintAndAssert("../data/11/real.txt", std::make_pair(186203ull, 221291560078593ull));
}
