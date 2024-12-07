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

namespace d07
{
	struct Equation
	{
		uint64_t total = 0;
		std::vector<uint64_t> operands;
	};

	struct Data07
	{
		std::vector<Equation> equations;
	};

	static auto loadData(const char* filename)
	{
		std::fstream s{ filename, s.in };
		assert(s.is_open());

		Data07 data;

		while (s.peek() != EOF)
		{
			std::string line;
			std::getline(s, line);
			assert(line.size() > 0);

			Equation eq;

			for (const auto word : std::views::split(line, ' '))
			{
				const std::string token(&*word.begin(), std::ranges::distance(word));

				const uint64_t num = std::stoull(token);
				if (eq.total == 0)
					eq.total = num;
				else
					eq.operands.push_back(num);
			}

			data.equations.push_back(eq);
		}

		return data;
	}

	static bool canEvaluate(const Equation& eq, size_t numOpTypes)
	{
		const uint64_t numOperators = eq.operands.size() - 1;
		const uint64_t numCombos = static_cast<uint64_t>(pow(numOpTypes, numOperators));

		for (uint64_t c = 0; c < numCombos; ++c)
		{
			uint64_t trial = eq.operands.front();

			for (size_t op = 0; op < numOperators; ++op)
			{
				const size_t opSelect = (c / static_cast<size_t>(pow(numOpTypes, op))) % numOpTypes;
				const uint64_t operand = eq.operands[op + 1];

				if (opSelect == 0)
					trial += operand;
				else if (opSelect == 1)
					trial *= operand;
				else if (opSelect == 2)
					trial = std::stoull(std::to_string(trial) + std::to_string(operand));
				else
					assert(false);
			}

			if (trial == eq.total)
				return true;
		}

		return false;
	}

	static uint64_t partOne(const Data07& data)
	{
		return std::accumulate(
			begin(data.equations),
			end(data.equations),
			0ull,
			[] (uint64_t sum, const Equation& eq)
			{
				return sum + (canEvaluate(eq, 2) ? eq.total : 0);
			}
		);
	}

	static uint64_t partTwo(const auto& data)
	{
		return std::accumulate(
			begin(data.equations),
			end(data.equations),
			0ull,
			[] (uint64_t sum, const Equation& eq)
			{
				return sum + (canEvaluate(eq, 3) ? eq.total : 0);
			}
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

void day07()
{
	d07::processPrintAndAssert("../data/07/test.txt", std::make_pair(3749ull, 11387ull));
	d07::processPrintAndAssert("../data/07/real.txt", std::make_pair(3245122495150ull, 105517128211543ull));
}
