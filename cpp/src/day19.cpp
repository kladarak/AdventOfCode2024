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

#include "String.h"

namespace d19
{
	struct Data19
	{
		std::vector<std::string> towels;
		std::vector<std::string> designs;
	};

	static auto loadData(const char* filename)
	{
		Data19 data;

		for (std::string& line : String::readLines(filename))
		{
			if (line.empty())
				continue;

			if (data.towels.empty())
				data.towels = String::delimit(line, ", ");
			else
				data.designs.push_back(std::move(line));
		}

		return data;
	}

	uint64_t solve(const Data19& data, const std::string& design, size_t pos)
	{
		using Key = std::pair<std::string, size_t>;
		static std::map<Key, uint64_t> memoized;

		if (pos == design.size())
			return 1;

		const Key key{design, pos};
		auto memoIter = memoized.find(key);
		if (memoIter != memoized.end())
			return memoIter->second;

		uint64_t numSolutions = 0;

		for (auto& towel : data.towels)
		{
			const size_t findPos = design.find(towel, pos);
			if (findPos == pos)
				numSolutions += solve(data, design, pos + towel.size());
		}

		memoized[key] = numSolutions;
		return numSolutions;
	}

	static uint64_t partOne(const Data19& data)
	{
		uint64_t numPossible = 0;

		for (const std::string& design : data.designs)
			if (solve(data, design, 0))
				numPossible++;

		return numPossible;
	}

	static uint64_t partTwo(const Data19& data)
	{
		uint64_t numSolutions = 0;

		for (const std::string& design : data.designs)
			numSolutions += solve(data, design, 0);

		return numSolutions;
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

void day19()
{
	d19::processPrintAndAssert("../data/19/test.txt", 6ull, 16ull);
	d19::processPrintAndAssert("../data/19/real.txt", 293ull, 623924810770264ull);
}
