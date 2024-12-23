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

namespace d23
{
	using Connection = std::pair<std::string, std::string>;
	using Network = std::map<std::string, std::set<std::string>>;

	struct Data23
	{
		std::vector<Connection> connections;
	};

	static auto loadData(const char* filename)
	{
		Data23 data;

		for (std::string& line : String::readLines(filename))
		{
			auto pcs = String::delimit(line, '-');
			data.connections.emplace_back(pcs[0], pcs[1]);
		}

		return data;
	}

	static Network createNetwork(const std::vector<Connection>& connections)
	{
		Network network;
		for (const auto& conn : connections)
		{
			network[conn.first].insert(conn.second);
			network[conn.second].insert(conn.first);
		}
		return network;
	}

	static uint64_t partOne(const Data23& data)
	{
		Network network = createNetwork(data.connections);

		std::set<std::vector<std::string>> triplets;
		for (auto iter = network.begin(); iter != network.end(); ++iter)
		{
			const std::string& first = iter->first;
			for (const std::string& second : iter->second)
			{
				for (const std::string& third : network.find(second)->second)
				{
					if (third != first && iter->second.find(third) != iter->second.end())
					{
						std::vector<std::string> triple;
						triple.push_back(first);
						triple.push_back(second);
						triple.push_back(third);
						
						std::sort(triple.begin(), triple.end());
						
						triplets.insert(triple);
					}
				}
			}
		}

		uint64_t tripleCountWithPCStartingWithT = 0;

		for (auto& triplet : triplets)
		{
			auto found = std::find_if(triplet.begin(), triplet.end(), [] (const std::string& pc)
			{
				return pc.front() == 't';
			});

			if (found != triplet.end())
				++tripleCountWithPCStartingWithT;
		}

		return tripleCountWithPCStartingWithT;
	}

	static std::string partTwo(const Data23& data)
	{
		const Network network = createNetwork(data.connections);

		std::vector<std::vector<std::string>> subNetworks;
		size_t largestSize = 0;
		size_t largestIndex = 0;

		for (auto iter = network.begin(); iter != network.end(); ++iter)
		{
			const std::string& pc = iter->first;
			std::vector<std::vector<std::string>> toAdd;
			toAdd.push_back({pc});

			for (const std::vector<std::string>& sub : subNetworks)
			{
				const bool connected = std::all_of(sub.begin(), sub.end(),
					[&] (const std::string& other)
					{
						return iter->second.find(other) != iter->second.end();
					}
				);

				if (connected)
				{
					toAdd.push_back(sub);
					toAdd.back().push_back(pc);
				}
			}

			subNetworks.reserve(subNetworks.size() + toAdd.size());
			for (auto& sub : toAdd)
			{
				subNetworks.push_back(std::move(sub));

				if (subNetworks.back().size() > largestSize)
				{
					largestSize = subNetworks.back().size();
					largestIndex = subNetworks.size() - 1;
				}
			}
		}

		assert(largestSize > 0);
		auto& selected = subNetworks[largestIndex];
		std::sort(selected.begin(), selected.end());
		std::string password;
		for (const std::string& pc : selected)
		{
			if (!password.empty())
				password += ',';

			password += pc;
		}
		return password;
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

void day23()
{
	d23::processPrintAndAssert("../data/23/test.txt", 7ull, "co,de,ka,ta");
	d23::processPrintAndAssert("../data/23/real.txt", 1308ull, "bu,fq,fz,pn,rr,st,sv,tr,un,uy,zf,zi,zy");
}
