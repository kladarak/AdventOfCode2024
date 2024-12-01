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
	static auto loadData(const char* filename)
	{
		std::fstream s{ filename, s.in };
		assert(s.is_open());

		std::vector<std::string> lines;

		while (s.peek() != EOF)
		{
			std::string line;
			std::getline(s, line);
			assert(line.size() > 0);
			lines.push_back(line);
		}

		return lines;
	}

	static uint64_t partOne(const auto& data)
	{
		return std::accumulate(
			begin(data),
			end(data),
			0ull,
			[] (uint64_t sum, const auto& line) { return sum + line.size(); }
		);
	}

	static uint64_t partTwo(const auto& data)
	{
		return std::accumulate(
			begin(data),
			end(data),
			0ull,
			[] (uint64_t sum, const auto& line) { return sum + line.size(); }
		);
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

void day_X_()
{
	d_X_::processPrintAndAssert("../data/_X_/test.txt", std::make_pair(0ull, 0ull));
	d_X_::processPrintAndAssert("../data/_X_/real.txt", std::make_pair(0ull, 0ull));
}
