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

namespace d22
{
	struct Data22
	{
	};

	static auto loadData(const char* filename)
	{
		Data22 data;

		for (std::string& line : String::readLines(filename))
		{
			if (line.empty())
				continue;

		}

		return data;
	}

	static uint64_t partOne([[maybe_unused]] const Data22& data)
	{
		return 0;
	}

	static uint64_t partTwo([[maybe_unused]] const Data22& data)
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

void day22()
{
	d22::processPrintAndAssert("../data/22/test.txt", 0ull, 0ull);
	d22::processPrintAndAssert("../data/22/real.txt", 0ull, 0ull);
}
