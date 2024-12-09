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

namespace d09
{
	struct Block
	{
		int64_t id = -1;
		size_t fileSize = 0;
	};

	struct Data09
	{
		std::vector<size_t> rawDiskMap;
		std::vector<Block> blocks;

		uint64_t checksum() const
		{
			uint64_t sum = 0;

			for (size_t i = 0, e = blocks.size(); i < e; ++i)
				if (blocks[i].id != -1)
					sum += i * blocks[i].id;

			return sum;
		}
	};

	static auto loadData(const char* filename)
	{
		std::fstream s{ filename, s.in };
		assert(s.is_open());

		Data09 data;

		while (s.peek() != EOF)
		{
			std::string line;
			std::getline(s, line);
			assert(line.size() > 0);

			for (char c : line)
				data.rawDiskMap.push_back(c - '0');
		}

		{
			bool isFile = true;
			int64_t fileId = 0;
			for (size_t n : data.rawDiskMap)
			{
				data.blocks.insert(data.blocks.end(), n, Block{isFile ? fileId : -1, n});
				fileId += isFile ? 1 : 0;
				isFile = !isFile;
			}
		}

		return data;
	}

	static uint64_t partOne(const Data09& input)
	{
		Data09 data = input;

		size_t endIndex = data.blocks.size() - 1;
		while (data.blocks[endIndex].id == -1)
			--endIndex;

		for (size_t i = 0, e = data.blocks.size(); i < e && i < endIndex; ++i)
		{
			if (data.blocks[i].id == -1)
			{
				std::swap(data.blocks[i], data.blocks[endIndex]);
				while (data.blocks[endIndex].id == -1)
					--endIndex;
			}
		}

		return data.checksum();
	}

	static uint64_t partTwo(const Data09& input)
	{
		Data09 data = input;

		for (int i = (int) data.blocks.size() - 1; i >= 0; --i)
		{
			const int64_t id = data.blocks[i].id;
			const size_t fileSize = data.blocks[i].fileSize;

			i -= ((int) fileSize - 1);

			if (id == -1)
				continue;

			int pos = -1;
			for (int j = 0; j < i && pos == -1; ++j)
			{
				if ((data.blocks[j].id == -1)
					&& (data.blocks[j].fileSize >= fileSize))
				{
					pos = j;
				}
			}

			if (pos == -1)
				continue;

			const size_t excess = data.blocks[pos].fileSize - fileSize;

			for (size_t k = 0; k < fileSize; ++k)
			{
				assert(data.blocks[i + k].id == id);
				data.blocks[pos + k] = data.blocks[i + k];
				data.blocks[i + k].id = -1;
			}

			for (size_t k = 0; k < excess; ++k)
			{
				Block& block = data.blocks[pos + fileSize + k];
				assert(block.id == -1);
				block.fileSize = excess;
			}

			// ought to join contiguous blocks that were moved from,
			// but it doesn't really matter for this exercise
		}

		return data.checksum();
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

void day09()
{
	d09::processPrintAndAssert("../data/09/test.txt", std::make_pair(1928ull, 2858ull));
	d09::processPrintAndAssert("../data/09/real.txt", std::make_pair(6299243228569ull, 0ull));
}
