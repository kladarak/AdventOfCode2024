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

namespace d03
{
	static auto loadData(const char* filename)
	{
		std::fstream s{ filename, s.in };
		assert(s.is_open());

		std::string line;

		while (s.peek() != EOF)
		{
			std::string next;
			std::getline(s, next);
			line += next;
		}

		assert(line.size() > 0);

		return line;
	}

	std::optional<uint64_t> tryMul(const char* data)
	{
		assert(data[0] == 'm');
		assert(data[1] == 'u');
		assert(data[2] == 'l');

		size_t pos = 3;
		auto isEnd = [&] () { return data[pos] == 0; };

		if (isEnd())
			return {};

		if (data[pos] != '(')
			return {};

		++pos;
		if (isEnd())
			return {};

		if (!std::isdigit(data[pos]))
			return {};

		const uint64_t first = std::stoull(&data[pos]);
		while (std::isdigit(data[pos]))
			++pos;

		if (isEnd())
			return {};

		if (data[pos] != ',')
			return {};

		++pos;
		if (isEnd())
			return {};

		if (!std::isdigit(data[pos]))
			return {};

		const uint64_t second = std::stoull(&data[pos]);
		while (std::isdigit(data[pos]))
			++pos;

		if (isEnd())
			return {};

		if (data[pos] != ')')
			return {};

		return first * second;
	}

	static uint64_t partOne(const std::string& data)
	{
		size_t pos = 0;
		uint64_t sum = 0;

		while ((pos = data.find("mul", pos), (pos != std::string::npos)))
		{
			if (std::optional<uint64_t> result = tryMul(&data[pos]))
				sum += result.value();

			++pos;
		}

		return sum;
	}

	static uint64_t partTwo(const std::string& data)
	{
		size_t pos = 0;
		uint64_t sum = 0;
		bool doMul = true;

		while (true)
		{
			const size_t mulPos = data.find("mul", pos);
			const size_t doPos = data.find("do()", pos); 
			const size_t dontPos = data.find("don't()", pos); 

			const size_t earliest = std::min(std::min(mulPos, doPos), dontPos);
			if (earliest == std::string::npos)
			{
				break;
			}
			else if (earliest == dontPos)
			{
				doMul = false;
			}
			else if (earliest == doPos)
			{
				doMul = true;
			}
			else if (earliest == mulPos)
			{
				if (doMul)
					if (std::optional<uint64_t> result = tryMul(&data[earliest]))
						sum += result.value();
			}

			pos = earliest + 1;
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

void day03()
{
	d03::processPrintAndAssert("../data/03/test.txt", std::make_pair(161ull, 161ull));
	d03::processPrintAndAssert("../data/03/test2.txt", std::make_pair(161ull, 48ull));
	d03::processPrintAndAssert("../data/03/real.txt", std::make_pair(160672468ull, 84893551ull));
}
