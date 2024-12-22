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
		std::vector<uint64_t> secrets;
	};

	static auto loadData(const char* filename)
	{
		Data22 data;

		for (std::string& line : String::readLines(filename))
		{
			data.secrets.push_back(std::stoull(line));
		}

		return data;
	}

	static uint64_t prng(uint64_t s)
	{
		const uint64_t a = s * 64;
		s = s ^ a;
		s = s % 16777216;

		const uint64_t b = s / 32;
		s = s ^ b;
		s = s % 16777216;

		const uint64_t c = s * 2048;
		s = s ^ c;
		s = s % 16777216;
		return s;
	}

	static void unittest_prng()
	{
		uint64_t s = 123;
		assert((s = prng(s)) == 15887950ull);
		assert((s = prng(s)) == 16495136ull);
		assert((s = prng(s)) == 527345ull);
		assert((s = prng(s)) == 704524ull);
		assert((s = prng(s)) == 1553684ull);
		assert((s = prng(s)) == 12683156ull);
		assert((s = prng(s)) == 11100544ull);
		assert((s = prng(s)) == 12249484ull);
		assert((s = prng(s)) == 7753432ull);
		assert((s = prng(s)) == 5908254ull);
	}

	static uint64_t partOne(const Data22& data)
	{
		uint64_t sum = 0;

		for (uint64_t s : data.secrets)
		{
			for (int i = 0; i < 2000; ++i)
				s = prng(s);

			sum += s;
		}

		return sum;
	}
	
	using Sequence = std::tuple<int, int, int, int>;

	struct Buyer
	{
		std::vector<int> prices;
		std::vector<int> diffs;

		std::map<Sequence, int> pricesPerSequence;

		explicit Buyer(uint64_t s, int count = 2000)
		{
			int last = (int) (s % 10);
			prices.push_back(last);

			for (int i = 0; i < count; ++i)
			{
				s = prng(s);

				const int price = (int) (s % 10);

				prices.push_back(price);
				diffs.push_back(price - last);
				last = price;
			}

			for (size_t i = 3; i < diffs.size(); ++i)
			{
				Sequence key =
				{
					diffs[i-3],
					diffs[i-2], 
					diffs[i-1], 
					diffs[i-0], 
				};

				pricesPerSequence.insert({key, prices[i+1]});
			}
		}
	};

	uint64_t calculateSalesForSequence(const Sequence& sequence, const std::vector<Buyer>& buyers)
	{
		uint64_t sales = 0;
		
		for (const Buyer& buyer : buyers)
		{
			const auto& b2Iter = buyer.pricesPerSequence.find(sequence);
			if (b2Iter != buyer.pricesPerSequence.end())
				sales += b2Iter->second;
		}

		return sales;
	}

	void unittest_sequence()
	{
		std::vector<Buyer> buyers =
		{
			Buyer(1),
			Buyer(2),
			Buyer(3),
			Buyer(2024)
		};

		assert(calculateSalesForSequence({-2,1,-1,3}, buyers) == 23ull);
	}

	static uint64_t partTwo(const Data22& data)
	{
		std::vector<Buyer> buyers;

		for (uint64_t s : data.secrets)
			buyers.emplace_back(s);

		uint64_t bestSale = 0;
		std::set<Sequence> seen;

		for (const Buyer& b1 : buyers)
		{
			for (const auto& iter : b1.pricesPerSequence)
			{
				const Sequence& seq = iter.first;
				if (!seen.insert(seq).second)
					continue;

				const uint64_t sales = calculateSalesForSequence(seq, buyers);
				bestSale = std::max(bestSale, sales);
			}
		}

		return bestSale;
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
	d22::unittest_prng();
	d22::unittest_sequence();
	d22::processPrintAndAssert("../data/22/test.txt", 37327623ull, 24ull);
	d22::processPrintAndAssert("../data/22/real.txt", 17965282217ull, 2152ull);
}
