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

namespace d05
{
	struct Data05
	{
		std::vector<std::pair<uint64_t, uint64_t>> pageOrdering;
		std::vector<std::vector<uint64_t>> pageUpdates;

		std::map<uint64_t, std::set<uint64_t>> pageDeps;

		bool canFirstGoBeforeSecond(uint64_t first, uint64_t second) const
		{
			auto deps = pageDeps.find(first);
			return (deps == pageDeps.end() || !deps->second.contains(second));
		}

		bool isPageUpdateValid(const std::vector<uint64_t>& pages) const
		{
			for (size_t i = 0; i < pages.size() - 1; ++i)
			{
				for (size_t j = i + 1; j < pages.size(); ++j)
				{
					const uint64_t page = pages[i];
					const uint64_t next = pages[j];

					if (!canFirstGoBeforeSecond(page, next))
						return false;
				}
			}

			return true;
		}

		std::vector<uint64_t> fixPageUpdate(const std::vector<uint64_t>& badPages) const
		{
			std::vector<uint64_t> fixed = badPages;

			std::sort(fixed.begin(), fixed.end(), [&] (uint64_t lhs, uint64_t rhs)
			{
				return canFirstGoBeforeSecond(lhs, rhs);
			});

			return fixed;
		}

	};

	static uint64_t getMiddlePage(const std::vector<uint64_t>& pages)
	{
		assert(pages.size() % 2 == 1);
		return pages[(pages.size() - 1) / 2];
	}

	static auto loadData(const char* filename)
	{
		std::fstream s{ filename, s.in };
		assert(s.is_open());

		Data05 data;

		bool parseUpdates = false;

		while (s.peek() != EOF)
		{
			std::string line;
			std::getline(s, line);

			if (line.size() == 0)
			{
				parseUpdates = true;
			}
			else if (parseUpdates)
			{
				std::vector<uint64_t> pages;
				for (const auto page : std::views::split(line, ','))
				{
					const std::string pageStr(&*page.begin(), std::ranges::distance(page));
					pages.push_back(std::stoull(pageStr));
				}

				assert(pages.size() % 2 == 1); // odd count
				data.pageUpdates.push_back(std::move(pages));
			}
			else
			{
				std::vector<uint64_t> pages;
				for (const auto page : std::views::split(line, '|'))
				{
					const std::string pageStr(&*page.begin(), std::ranges::distance(page));
					pages.push_back(std::stoull(pageStr));
				}

				assert(pages.size() == 2);
				data.pageOrdering.emplace_back(pages[0], pages[1]);
			}
		}

		for (const std::pair<uint64_t, uint64_t>& order : data.pageOrdering)
			data.pageDeps[order.second].insert(order.first);

		return data;
	}

	static uint64_t partOne(const Data05& data)
	{
		uint64_t middlePageSum = 0;

		for (const std::vector<uint64_t>& pages : data.pageUpdates)
			if (data.isPageUpdateValid(pages))
				middlePageSum += getMiddlePage(pages);

		return middlePageSum;
	}

	static uint64_t partTwo(const Data05& data)
	{
		uint64_t middlePageSum = 0;

		for (const std::vector<uint64_t>& pages : data.pageUpdates)
		{
			if (data.isPageUpdateValid(pages))
				continue;

			std::vector<uint64_t> fixed = data.fixPageUpdate(pages);
			middlePageSum += getMiddlePage(fixed);
		}

		return middlePageSum;
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

void day05()
{
	d05::processPrintAndAssert("../data/05/test.txt", std::make_pair(143ull, 123ull));
	d05::processPrintAndAssert("../data/05/real.txt", std::make_pair(5391ull, 0ull));
}
