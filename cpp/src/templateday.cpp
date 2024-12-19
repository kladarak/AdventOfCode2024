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

#include "String.h"
#include "Renderer.h"
#include "Vec2.h"

namespace d_X_
{
	struct Data_X_
	{
	};

	static auto loadData(const char* filename)
	{
		Data_X_ data;

		for (std::string& line : String::readLines(filename))
		{
			if (line.empty())
				continue;

		}

		return data;
	}

	static uint64_t partOne([[maybe_unused]] const Data_X_& data)
	{
		return 0;
	}

	static uint64_t partTwo([[maybe_unused]] const Data_X_& data)
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
