#pragma once

#include <vector>
#include <string>

namespace String
{
	std::vector<std::string> readLines(const char* filename);

	std::vector<std::string> delimit(const std::string& line, char delimiter);
	std::vector<std::string> delimit(const std::string& line, const std::string& delimiters);

	std::string trim(const std::string& str);
}