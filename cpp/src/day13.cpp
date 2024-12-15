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

#include "Vec2.h"

namespace d13
{
	struct Machine
	{
		Vec2i64 buttonA;
		Vec2i64 buttonB;
		Vec2i64 prize;

		std::optional<uint64_t> calculateWinCost() const
		{
			/*
			* y = ax + c
			* y = bx + d
			* 
			* a = A.y/A.x
			* b = B.y/B.x
			* 
			* ax + c = bx + d
			* ax - bx = d - c
			* x(a - b) = d - c
			* x = (d - c) / (a - b)
			* 
			* Make A start from origin and B start from prize.
			* So c = 0.
			* d may be decimal... P - nB where x = 0.
			* d = P.y - B.y*(P.x / B.x)
			* d = P.y - b*P.x
			* 
			* So, plugging substitutions in for a, b, c and d:
			* 
			* x = (P.y - ((B.y/B.x)*P.x)) / ((A.y/A.x) - (B.y/B.x)) 
			* x = (P.y - P.x*(B.y/B.x)) / ((A.y*B.x/A.x*B.x) - (A.x*B.y/A.x*B.x)) 
			* x = (P.y - P.x*(B.y/B.x)) / ((A.y*B.x - A.x*B.y) / (A.x*B.x))
			* x = ((P.y - P.x*(B.y/B.x)) * (A.x*B.x)) / (A.y*B.x - A.x*B.y)
			* x = (P.y*A.x*B.x - P.x*(A.x*B.x)*(B.y/B.x) / (A.y*B.x - A.x*B.y)
			* x = (P.y*A.x*B.x - P.x*A.x*B.y) / (A.y*B.x - A.x*B.y)
			* 
			* And x must be whole.
			* */

			const Vec2i64 A = buttonA;
			const Vec2i64 B = buttonB;
			const Vec2i64 P = prize;
			
			const int64_t numerator1 = P.y*A.x*B.x;
			const int64_t numerator2 = P.x*A.x*B.y;
			const int64_t numerator = (numerator1 - numerator2);
			const int64_t denom1 = (A.y*B.x);
			const int64_t denom2 = (A.x*B.y);
			const int64_t denominator = (denom1 - denom2);

			const bool isWhole = (numerator % denominator) == 0;
			if (!isWhole)
				return {}; // cannot be solved, intersection does not land on an integer

			const int64_t xa = numerator / denominator;
			const int64_t xb = P.x - xa;

			if (xa % A.x != 0)
				return {}; // cannot be solved (requires partial amounts of A)

			if (xb % B.x != 0)
				return {}; // cannot be solved (requires partial amounts of B)

			const int64_t a = xa / A.x;
			const int64_t b = xb / B.x;

			return a*3 + b;
		}
	};

	struct Data13
	{
		std::vector<Machine> machines;
	};

	static auto loadData(const char* filename)
	{
		static auto readPos = [] (const std::string& line)
		{
			const size_t xPos = line.find('X');
			const size_t yPos = line.find('Y');
			const int x = std::stol(&line[xPos + 2]);
			const int y = std::stol(&line[yPos + 2]);
			return Vec2i64{x, y};
		};

		std::fstream s{ filename, s.in };
		assert(s.is_open());

		Data13 data;
		Machine machine;

		while (s.peek() != EOF)
		{
			std::string line;
			std::getline(s, line);
			if (line.size() == 0)
				continue;

			if (machine.buttonA == Vec2i64())
			{
				machine.buttonA = readPos(line);
			}
			else if (machine.buttonB == Vec2i64())
			{
				machine.buttonB = readPos(line);
			}
			else if (machine.prize == Vec2i64())
			{
				machine.prize = readPos(line);
				data.machines.push_back(machine);
				machine = Machine();
			}
			else
			{
				assert(false);
			}
		}

		return data;
	}

	static uint64_t partOne(const Data13& data)
	{
		uint64_t sum = 0;

		for (const Machine& machine : data.machines)
		{
			if (std::optional<uint64_t> cost = machine.calculateWinCost())
				sum += cost.value();
		}

		return sum;
	}

	static uint64_t partTwo(const Data13& data)
	{
		const int64_t offset = 10'000'000'000'000;

		uint64_t sum = 0;

		for (Machine machine : data.machines)
		{
			machine.prize += Vec2i64{offset, offset};

			if (std::optional<uint64_t> cost = machine.calculateWinCost())
				sum += cost.value();
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

void day13()
{
	d13::processPrintAndAssert("../data/13/test.txt", std::make_pair(480ull, 875318608908ull));
	d13::processPrintAndAssert("../data/13/real.txt", std::make_pair(39290ull, 73458657399094ull));
}
