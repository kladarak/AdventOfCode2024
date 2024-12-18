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

namespace d_X_
{
	struct Data_X_
	{
	};

	static auto loadData(const char* filename)
	{
		std::fstream s{ filename, s.in };
		assert(s.is_open());

		Data_X_ data;

		while (s.peek() != EOF)
		{
			std::string line;
			std::getline(s, line);
			assert(line.size() > 0);

			for (const auto word : std::views::split(line, ' '))
			{
				[[maybe_unused]] const std::string token(&*word.begin(), std::ranges::distance(word));
			}
		}

		return data;
	}

	static uint64_t partOne(const Data_X_& )
	{
		return 0;
	}

	static uint64_t partTwo(const Data_X_& )
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

void day_X_()
{
	d_X_::processPrintAndAssert("../data/_X_/test.txt", 0ull, 0ull);
	d_X_::processPrintAndAssert("../data/_X_/real.txt", 0ull, 0ull);
}
