#include "String.h"

#include <ranges>
#include <fstream>
#include <cassert>

namespace String
{
	std::vector<std::string> readLines(const char* filename)
	{
		std::fstream s{ filename, s.in };
		assert(s.is_open());

		std::vector<std::string> lines;

		while (s.peek() != EOF)
		{
			std::string line;
			std::getline(s, line);
			lines.push_back(line);
		}

		return lines;
	}

	std::vector<std::string> delimit(const std::string& line, char delimiter)
	{
		std::vector<std::string> delimited;

		// TODO: Try to find a range util to do this without a for loop
		for (const auto word : std::views::split(line, delimiter))
			delimited.emplace_back(&*word.begin(), std::ranges::distance(word));

		return delimited;
	}

	std::vector<std::string> delimit(const std::string& line, const std::string& delimiters)
	{
		std::vector<std::string> delimited;
		delimited.push_back(line);

		std::vector<std::string> scratch;
		for (char c : delimiters)
		{
			for (const std::string& word : delimited)
			{
				for (std::string& w : delimit(word, c))
					if (!w.empty())
						scratch.push_back(std::move(w));
			}

			delimited = std::exchange(scratch, {});
		}

		return delimited;
	}

	std::string trim(const std::string& str)
	{
		if (str.empty())
			return str;

		size_t l = 0;
		while (l < str.size() && str[l] == ' ')
			++l;

		if (l == str.size())
			return {};

		size_t r = str.size() - 1;
		while (str[r] == ' ')
			--r;

		return str.substr(l, r - l + 1);
	}
}
