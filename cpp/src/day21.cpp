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

namespace d21
{
	struct Data21
	{
		std::vector<std::string> codes;
		std::vector<int> numeric;
	};

	static auto loadData(const char* filename)
	{
		Data21 data;

		data.codes = String::readLines(filename);

		for (const std::string& code : data.codes)
		{
			const char* codeChars = code.data();
			while (*codeChars && !std::isdigit(*codeChars))
				++codeChars;

			data.numeric.push_back(std::stoi(codeChars));
		}

		return data;
	}

	static const std::vector<std::string> numKeypad = 
	{
		"789",
		"456",
		"123",
		" 0A"
	};

	static const std::vector<std::string> dirKeypad = 
	{
		" ^A",
		"<v>",
	};

	struct Robot
	{
		const std::vector<std::string>& keypad;
		Robot* controlledBy = nullptr;
		char currentButton = 'A';

		using InstCountKey = std::pair<char, char>;
		std::map<InstCountKey, uint64_t> memoizedInstCount;

		explicit Robot(const std::vector<std::string>& keypad)
			: keypad(keypad)
		{
		}

		Vec2 getKeyPos(char k) const
		{
			for (int y = 0; y < (int) keypad.size(); ++y)
			{
				for (int x = 0; x < (int) keypad.front().size(); ++x)
				{
					if (keypad[y][x] == k)
						return {x,y};
				}
			}
			assert(false);
			return {-1,-1};
		}

		const std::vector<std::string>& getPermutations(const char from, const char to) const
		{
			using Key = std::pair<char, char>;
			static std::map<Key, std::vector<std::string>> memoized;

			const Key key{from, to};
			if (auto iter = memoized.find(key); iter != memoized.end())
				return iter->second;

			const Vec2 fromPos = getKeyPos(from);
			const Vec2 toPos = getKeyPos(to);
			const Vec2 deadPos = getKeyPos(' ');

			assert(fromPos != deadPos);
			assert(toPos != deadPos);

			const Vec2 delta = toPos - fromPos;

			const char buttonX = (delta.x < 0) ? '<' : (delta.x > 0) ? '>' : ' ';
			const char buttonY = (delta.y < 0) ? '^' : (delta.y > 0) ? 'v' : ' ';

			// It's always better to group button presses.
			// It's always more expensive to do <v< than <<v for instance.
			std::string permXFirst;
			permXFirst.append(abs(delta.x), buttonX);
			permXFirst.append(abs(delta.y), buttonY);
			permXFirst.push_back('A');

			std::string permYFirst;
			permYFirst.append(abs(delta.y), buttonY);
			permYFirst.append(abs(delta.x), buttonX);
			permYFirst.push_back('A');

			std::vector<std::string>& perms = memoized[key];

			if (fromPos + Vec2{delta.x, 0} == deadPos)
			{
				perms.push_back(std::move(permYFirst));
			}
			else if (fromPos + Vec2{0, delta.y} == deadPos)
			{
				perms.push_back(std::move(permXFirst));
			}
			else if (permXFirst != permYFirst)
			{
				perms.push_back(std::move(permXFirst));
				perms.push_back(std::move(permYFirst));
			}
			else
			{
				perms.push_back(std::move(permXFirst));
			}

			return perms;
		}

		uint64_t request(char targetButton)
		{
			const InstCountKey key{currentButton, targetButton};
			if (auto iter = memoizedInstCount.find(key); iter != memoizedInstCount.end())
			{
				currentButton = targetButton;
				return iter->second;
			}

			uint64_t count = UINT64_MAX;

			for (const std::string& instructions : getPermutations(currentButton, targetButton))
			{
				uint64_t trialCount = 0;
				for (const char instruction : instructions)
					trialCount += controlledBy ? controlledBy->request(instruction) : 1;

				count = std::min(count, trialCount);
			}
			
			assert(count != UINT64_MAX);

			currentButton = targetButton;
			memoizedInstCount[key] = count;

			return count;
		}

		uint64_t calculateInstructionCount(const std::string& code)
		{
			uint64_t count = 0;

			for (size_t i = 0, e = code.size(); i < e; ++i)
				count += request(code[i]);

			return count;
		}
	};

	static uint64_t calculateComplexity(const std::string& code, int robotCount)
	{
		Robot numPadRobot{numKeypad};
		std::vector<Robot> dirRobots(robotCount, Robot(dirKeypad));

		for (size_t i = 0; i < dirRobots.size(); ++i)
		{
			if (i == 0)
				numPadRobot.controlledBy = &dirRobots[i];
			else
				dirRobots[i-1].controlledBy = &dirRobots[i];
		}
		
		const uint64_t complexity = std::stoi(code);
		const size_t instructionCount = numPadRobot.calculateInstructionCount(code);

		return instructionCount * complexity;
	}

	static uint64_t partOne(const Data21& data)
	{
		uint64_t complexitySum = 0;

		for (const std::string& code : data.codes)
			complexitySum += calculateComplexity(code, 2);

		return complexitySum;
	}

	static uint64_t partTwo(const Data21& data)
	{
		uint64_t complexitySum = 0;

		for (const std::string& code : data.codes)
			complexitySum += calculateComplexity(code, 25);

		return complexitySum;
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

	void assertComplexity(
		const std::string& code,
		const std::string& instruction,
		size_t expectedLen,
		uint64_t expectedCodeNum)
	{
		uint64_t expectedComplexity = expectedLen * expectedCodeNum;
		assert(std::stoi(code) == expectedCodeNum);
		assert(instruction.size() == expectedLen);
		uint64_t result = calculateComplexity(code, 2);
		assert(result == expectedComplexity);
	}
}

void day21()
{
	d21::assertComplexity("029A", "<vA<AA>>^AvAA<^A>A<v<A>>^AvA^A<vA>^A<v<A>^A>AAvA^A<v<A>A>^AAAvA<^A>A", 68, 29);
	d21::assertComplexity("980A", "<v<A>>^AAAvA^A<vA<AA>>^AvAA<^A>A<v<A>A>^AAAvA<^A>A<vA>^A<A>A", 60, 980);
	d21::assertComplexity("179A", "<v<A>>^A<vA<A>>^AAvAA<^A>A<v<A>>^AAvA^A<vA>^AA<A>A<v<A>A>^AAAvA<^A>A", 68, 179);
	d21::assertComplexity("456A", "<v<A>>^AA<vA<A>>^AAvAA<^A>A<vA>^A<A>A<vA>^A<A>A<v<A>A>^AAvA<^A>A", 64 , 456);
	d21::assertComplexity("379A", "<v<A>>^AvA^A<vA<AA>>^AAvA<^A>AAvA^A<vA>^AA<A>A<v<A>A>^AAAvA<^A>A", 64 , 379);
	d21::processPrintAndAssert("../data/21/test.txt", 126384ull, 154115708116294ull);
	d21::processPrintAndAssert("../data/21/real.txt", 162740ull, 203640915832208ull);
}
