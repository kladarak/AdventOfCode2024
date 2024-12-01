#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <optional>
#include <unordered_map>
#include <cstdint>

struct Data01
{
	std::vector<int64_t> leftList;
	std::vector<int64_t> rightList;
};

static Data01 readData(const char* filename)
{
	std::fstream s{filename, s.in};
	assert(s.is_open());

	Data01 data;

	while (s.peek() != EOF)
	{
		std::string line;
		std::getline(s, line);
		assert(line.size() > 0);

		const int64_t left = std::stoi(line);
		const size_t pos = line.find(' ');
		const int64_t right = std::stoi(line.substr(pos));

		data.leftList.push_back(left);
		data.rightList.push_back(right);
	}

	std::sort(begin(data.leftList), end(data.leftList));
	std::sort(begin(data.rightList), end(data.rightList));

	return data;
}

static int64_t part1(const Data01& data)
{
	int64_t sum = 0;

	for (size_t i = 0; i < data.leftList.size(); ++i)
	{
		const int64_t diff = data.leftList[i] - data.rightList[i];
		sum += std::abs(diff);
	}

	return sum;
}

static int64_t part2(const Data01& data)
{
	std::unordered_map<int64_t, int64_t> occurrences;
	for (const int64_t right : data.rightList)
		occurrences[right]++;

	int64_t simScore = 0;
	for (const int64_t left : data.leftList)
		simScore += left * occurrences[left];

	return simScore;
}

static void process(const char* filename, std::optional<int64_t> expected1 = {}, std::optional<int64_t> expected2 = {})
{
	const Data01 data = readData(filename);
	const int64_t result1 = part1(data);
	const int64_t result2 = part2(data);

	std::cout << "--- " << filename << " ---" << std::endl;
	
	std::cout << "pt1: " << result1;
	if (expected1)
		std::cout << " (expected: " << expected1.value() << ")";
	std::cout << std::endl;

	std::cout << "pt2: " << result2;
	if (expected2)
		std::cout << " (expected: " << expected2.value() << ")";
	std::cout << std::endl;

	assert(!expected1 || result1 == expected1.value());
	assert(!expected2 || result2 == expected2.value());
}

void day01()
{
	process("../data/01/test.txt", 11, 31);
	process("../data/01/real.txt", 2057374, 23177084);
}
