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

namespace d04
{
	struct Data04
	{
		std::vector<std::string> grid;
		int rows = 0;
		int cols = 0;
	};

	static Data04 loadData(const char* filename)
	{
		std::fstream s{ filename, s.in };
		assert(s.is_open());

		Data04 data;

		while (s.peek() != EOF)
		{
			std::string line;
			std::getline(s, line);
			
			if (data.cols == 0)
				data.cols = (int) line.size();

			assert(line.size() == data.cols);

			data.grid.push_back(std::move(line));
		}

		data.rows = (int) data.grid.size();

		return data;
	}

	static uint64_t search_xmas(const Data04& data, int row, int col)
	{
		uint64_t count = 0;

		// forwards
		if (col + 3 < data.cols)
		{
			if (data.grid[row][col] == 'X'
				&& data.grid[row][col+1] == 'M'
				&& data.grid[row][col+2] == 'A'
				&& data.grid[row][col+3] == 'S')
				++count;
		}

		// backwards
		if (col - 3 >= 0)
		{
			if (data.grid[row][col] == 'X'
				&& data.grid[row][col-1] == 'M'
				&& data.grid[row][col-2] == 'A'
				&& data.grid[row][col-3] == 'S')
				++count;
		}

		// down
		if (row + 3 < data.rows)
		{
			if (data.grid[row][col] == 'X'
				&& data.grid[row+1][col] == 'M'
				&& data.grid[row+2][col] == 'A'
				&& data.grid[row+3][col] == 'S')
				++count;
		}

		// up
		if (row - 3 >= 0)
		{
			if (data.grid[row][col] == 'X'
				&& data.grid[row-1][col] == 'M'
				&& data.grid[row-2][col] == 'A'
				&& data.grid[row-3][col] == 'S')
				++count;
		}

		// down and right
		if ((row + 3 < data.rows)
			&& (col + 3 < data.cols))
		{
			if (data.grid[row][col] == 'X'
				&& data.grid[row+1][col+1] == 'M'
				&& data.grid[row+2][col+2] == 'A'
				&& data.grid[row+3][col+3] == 'S')
				++count;
		}

		// down and left
		if ((row + 3 < data.rows)
			&& (col - 3 >= 0))
		{
			if (data.grid[row][col] == 'X'
				&& data.grid[row+1][col-1] == 'M'
				&& data.grid[row+2][col-2] == 'A'
				&& data.grid[row+3][col-3] == 'S')
				++count;
		}

		// up and right
		if ((row - 3 >= 0)
			&& (col + 3 < data.cols))
		{
			if (data.grid[row][col] == 'X'
				&& data.grid[row-1][col+1] == 'M'
				&& data.grid[row-2][col+2] == 'A'
				&& data.grid[row-3][col+3] == 'S')
				++count;
		}

		// up and left
		if ((row - 3 >= 0)
			&& (col - 3 >= 0))
		{
			if (data.grid[row][col] == 'X'
				&& data.grid[row-1][col-1] == 'M'
				&& data.grid[row-2][col-2] == 'A'
				&& data.grid[row-3][col-3] == 'S')
				++count;
		}

		return count;
	}

	static uint64_t partOne(const Data04& data)
	{
		uint64_t count = 0;

		for (int r = 0; r < data.rows; ++r)
		{
			for (int c = 0; c < data.cols; ++c)
			{
				if (data.grid[r][c] == 'X')
					count += search_xmas(data, r, c);
			}
		}

		return count;
	}

	static uint64_t search_x_mas(const Data04& data, int row, int col)
	{
		if (row == 0 || col == 0 || row == data.rows - 1 || col == data.cols - 1)
			return 0;

		assert(data.grid[row][col] == 'A');

		bool found_NW_SE = false;
		bool found_SW_NE = false;

		found_NW_SE
			|= (data.grid[row-1][col-1] == 'M')
			&& (data.grid[row+1][col+1] == 'S');

		found_NW_SE
			|= (data.grid[row-1][col-1] == 'S')
			&& (data.grid[row+1][col+1] == 'M');

		found_SW_NE
			|= (data.grid[row+1][col-1] == 'M')
			&& (data.grid[row-1][col+1] == 'S');

		found_SW_NE
			|= (data.grid[row+1][col-1] == 'S')
			&& (data.grid[row-1][col+1] == 'M');

		if (found_NW_SE && found_SW_NE)
			return 1;

		return 0;
	}

	static uint64_t partTwo(const Data04& data)
	{
		uint64_t count = 0;

		for (int r = 0; r < data.rows; ++r)
		{
			for (int c = 0; c < data.cols; ++c)
			{
				if (data.grid[r][c] == 'A')
					count += search_x_mas(data, r, c);
			}
		}

		return count;
	}

	static std::pair<uint64_t, uint64_t> process(const char* filename)
	{
		const Data04 data = loadData(filename);
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

void day04()
{
	d04::processPrintAndAssert("../data/04/test.txt", std::make_pair(18ull, 9ull));
	d04::processPrintAndAssert("../data/04/real.txt", std::make_pair(2593ull, 1950ull));
}
