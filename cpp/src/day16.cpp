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

namespace d_16
{
	struct Data_16
	{
	};

	static auto loadData(const char* filename)
	{
		std::fstream s{ filename, s.in };
		assert(s.is_open());

		Data_16 data;

		while (s.peek() != EOF)
		{
			std::string line;
			std::getline(s, line);
			assert(line.size() > 0);
		}

		return data;
	}

	static uint64_t partOne(const Data_16& )
	{
		return 0;
	}

	static uint64_t partTwo(const Data_16& )
	{
		return 0;
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

void day_16()
{
	d_16::processPrintAndAssert("../data/_16/test.txt", std::make_pair(0ull, 0ull));
	d_16::processPrintAndAssert("../data/_16/real.txt", std::make_pair(0ull, 0ull));
}
