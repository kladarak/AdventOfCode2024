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

namespace d02
{
	struct Data02
	{
		std::vector<std::vector<int64_t>> reports;
	};

	static auto loadData(const char* filename)
	{
		std::fstream s{ filename, s.in };
		assert(s.is_open());

		Data02 data;

		while (s.peek() != EOF)
		{
			std::string line;
			std::getline(s, line);
			assert(line.size() > 0);

			data.reports.push_back({});

			for (const auto word : std::views::split(line, ' '))
			{
				const std::string token(&*word.begin(), std::ranges::distance(word));

				const int64_t num = std::stoull(token);
				data.reports.back().push_back(num);
			}
		}

		return data;
	}

	static bool isSafe(const std::vector<int64_t>& report)
	{
		std::optional<int64_t> prev;
		std::optional<int64_t> diff;

		for (int64_t num : report)
		{
			if (prev)
			{
				int64_t delta = prev.value() - num;
				int64_t absDelta = std::abs(delta);

				if (absDelta < 1 || absDelta > 3)
					return false;

				if (diff && (diff.value() < 0) != (delta < 0))
					return false;

				diff = delta;
			}

			prev = num;
		}

		return true;
	}

	static int64_t partOne(const auto& data)
	{
		return std::accumulate(
			begin(data.reports),
			end(data.reports),
			0ll,
			[] (int64_t sum, const std::vector<int64_t>& report)
			{
				return sum + (isSafe(report) ? 1 : 0);
			}
		);
	}

	static int64_t partTwo(const auto& data)
	{
		return std::accumulate(
			begin(data.reports),
			end(data.reports),
			0ll,
			[] (int64_t sum, const std::vector<int64_t>& report)
			{
				if (isSafe(report))
					return sum + 1;

				for (size_t i = 0; i < report.size(); ++i)
				{
					auto copy = report;
					copy.erase(copy.begin() + i);
					if (isSafe(copy))
						return sum + 1;
				}

				return sum;
			}
		);
	}

	static std::pair<int64_t, int64_t> process(const char* filename)
	{
		const auto data = loadData(filename);
		return std::make_pair(partOne(data), partTwo(data));
	}

	static void processPrintAndAssert(const char* filename, std::pair<int64_t, int64_t> expected)
	{
		const auto result = process(filename);
		std::cout << "Part 1: " << result.first << " Part 2: " << result.second << std::endl;

		assert(expected.first == 0 || result.first == expected.first);
		assert(expected.second == 0 || result.second == expected.second);
	}
}

void day02()
{
	d02::processPrintAndAssert("../data/02/test.txt", std::make_pair(2ll, 4ll));
	d02::processPrintAndAssert("../data/02/real.txt", std::make_pair(598ll, 0ll));
}
