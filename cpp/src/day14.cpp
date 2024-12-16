#include <windows.h>
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
#include <chrono>
#include <thread>

#include "Vec2.h"
#include "Renderer.h"

namespace d14
{
	struct Robot
	{
		Vec2 pos;
		Vec2 vel;
		char glyph = 0;
	};

	struct Data14
	{
		std::vector<Robot> robots;
		Bounds bounds;
	};

	static auto loadData(const char* filename)
	{
		std::fstream s{ filename, s.in };
		assert(s.is_open());

		Data14 data;

		while (s.peek() != EOF)
		{
			std::string line;
			std::getline(s, line);
			assert(line.size() > 0);

			if (data.bounds.botRght == Vec2())
			{
				data.bounds.botRght.x = std::stol(line);
				data.bounds.botRght.y = std::stol(&line[line.find(',') + 1]);
			}
			else
			{
				assert(line[0] == 'p');
				assert(line[1] == '=');

				Robot r;
				r.pos.x = std::stol(&line[2]);
				
				auto pos = line.find(',') + 1;
				r.pos.y = std::stol(&line[pos]);

				pos = line.find('v') + 2;
				r.vel.x = std::stol(&line[pos]);
				pos = line.find(',', pos) + 1;
				r.vel.y = std::stol(&line[pos]);

				const size_t index = data.robots.size();
				const size_t mod52 = index % 52;
				if (mod52 < 26)
					r.glyph = 'A' + (char) mod52;
				else
					r.glyph = 'a' + (char) (mod52 - 26);

				data.robots.push_back(r);
			}
		}

		return data;
	}

	Vec2 wrap(Vec2 pos, const Bounds& bounds)
	{
		const int width = bounds.botRght.x;
		const int height = bounds.botRght.y;

		if (pos.x < 0)
			pos.x += width;

		if (pos.y < 0)
			pos.y += height;

		if (pos.x >= width)
			pos.x -= width;

		if (pos.y >= height)
			pos.y -= height;

		return pos;
	}

	void tick(std::vector<Robot>& robots, [[maybe_unused]] const Bounds& bounds)
	{
		[[maybe_unused]] const int width = bounds.botRght.x;
		[[maybe_unused]] const int height = bounds.botRght.y;

		for (Robot& r : robots)
		{
			Vec2 pos = r.pos + r.vel;
			r.pos = wrap(pos, bounds);
		}
	}

	bool hasContinuousChunk(const Renderer& renderer)
	{
		for (auto& line : renderer.getBuffer())
			if (&line != &renderer.getBuffer().back() && line.find("#######") != std::string::npos)
				return true;

		return false;
	}

	static uint64_t partOne(const Data14& data)
	{
		const int width = data.bounds.botRght.x;
		const int height = data.bounds.botRght.y;
		std::vector<Robot> robots = data.robots;

		for (int i = 0; i < 100; ++i)
			tick(robots, data.bounds);

		const int hWidth = width / 2;
		const int hHeight = height / 2;
		assert(hWidth*2 + 1 == width);
		assert(hHeight*2 + 1 == height);

		Bounds quadrants[4] =
		{
			{{0, 0}, {hWidth, hHeight}},
			{{width - hWidth, 0}, {width, hHeight}},
			{{0, height - hHeight}, {hWidth, height}},
			{{width - hWidth, height - hHeight}, {width, height}},
		};

		uint64_t counts[4] = {0};

		for (const Robot& r : robots)
		{
			for (int i = 0; i < 4; ++i)
				if (quadrants[i].isInBounds(r.pos))
					counts[i]++;
		}

		uint64_t product = 1;
		for (int i = 0; i < 4; ++i)
			product *= counts[i];

		return product;
	}

	static uint64_t partTwo(const Data14& data)
	{
		using namespace std::chrono_literals;

		system("cls");

		Renderer renderer(data.bounds);
		std::set<std::string> seen;
		std::vector<Robot> robots = data.robots;

		for (int i = 1; true; ++i)
		{
			if (!seen.insert(renderer.concatenated()).second)
				break; // escape infinite loop

			tick(robots, data.bounds);

			constexpr bool animated = false;

			if (animated)
			{
				for (int frame = 0; frame < 60; ++frame)
				{
					renderer.clear();
					const float t = frame / 60.0f;

					for (const Robot& robot : robots)
					{
						const Vec2 pos = robot.pos;
						const Vec2 vel = robot.vel;
						const Vec2 prev = pos - vel;
						const float deltaX = vel.x * t;
						const float deltaY = vel.y * t;

						const int x = (int)(prev.x + deltaX);
						const int y = (int)(prev.y + deltaY);
						const Vec2 animPos = wrap({x, y}, data.bounds);
						renderer.plot(animPos, robot.glyph);
					}

					renderer.render();
					std::this_thread::sleep_for(16ms);
				}
			}
			else
			{
				renderer.clear();
				for (const Robot& robot : robots)
					renderer.plot(robot.pos, robot.glyph);
		
				if (hasContinuousChunk(renderer))
				{
					renderer.render();

					//if (std::cin.get() == 'y')
						return i;
				}
			}
		}

		return 0;
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

void day14()
{
	d14::processPrintAndAssert("../data/14/test.txt", std::make_pair(12ull, 0ull));
	d14::processPrintAndAssert("../data/14/real.txt", std::make_pair(211692000ull, 6586ull));
}
