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

#include "Vec2.h"
#include "Renderer.h"

namespace d15
{
	enum class EntityType
	{
		None,
		Wall,
		Box,
		Robot
	};

	struct Data15
	{
		std::vector<std::vector<EntityType>> grid;
		std::vector<Direction> instructions;
		Bounds bounds;
	};

	static auto loadData(const char* filename)
	{
		std::fstream s{ filename, s.in };
		assert(s.is_open());

		Data15 data;
		bool processGrid = true;

		while (s.peek() != EOF)
		{
			std::string line;
			std::getline(s, line);

			if (line.size() == 0)
			{
				processGrid = false;
			}
			else if (processGrid)
			{
				data.grid.push_back({});

				std::vector<EntityType>& row = data.grid.back();

				for (char c : line)
				{
					if (c == '#')
						row.push_back(EntityType::Wall);
					else if (c == '.')
						row.push_back(EntityType::None);
					else if (c == 'O')
						row.push_back(EntityType::Box);
					else if (c == '@')
						row.push_back(EntityType::Robot);
					else
						assert(false);
				}

				assert(data.grid.front().size() == data.grid.back().size());
			}
			else
			{
				for (char c : line)
				{
					if (c == '^')
						data.instructions.push_back(Direction::Up);
					else if (c == 'v')
						data.instructions.push_back(Direction::Down);
					else if (c == '<')
						data.instructions.push_back(Direction::Left);
					else if (c == '>')
						data.instructions.push_back(Direction::Right);
					else
						assert(false);
				}
			}
		}

		data.bounds.botRght = {(int) data.grid.front().size(), (int) data.grid.size() };

		return data;
	}

	struct Entity
	{
		EntityType type = EntityType::None;
		Vec2 pos;
	};

	struct Grid
	{
		std::vector<std::unique_ptr<Entity>> entities;
		std::vector<Entity*> walls;
		std::vector<Entity*> boxes;
		Entity* robot = nullptr;
		Bounds bounds;
		bool isDoubleWidth = false;
		std::unique_ptr<Renderer> renderer;

		explicit Grid(const Data15& data, bool doubleWidth)
			: bounds(data.bounds)
			, isDoubleWidth(doubleWidth)
		{
			if (doubleWidth)
				bounds.botRght.x *= 2;

			renderer = std::make_unique<Renderer>(bounds);

			for (int y = 0; y < (int) data.grid.size(); ++y)
			{
				for (int x = 0; x < (int) data.grid.front().size(); ++x)
				{
					const EntityType type = data.grid[y][x];
					const Vec2 pos = doubleWidth ? Vec2{x*2, y} : Vec2{x, y};

					switch (type)
					{
						case EntityType::None:
							break;

						case EntityType::Box:
							entities.emplace_back(new Entity{type, pos});
							boxes.push_back(entities.back().get());
							break;

						case EntityType::Wall:
							entities.emplace_back(new Entity{type, pos});
							walls.push_back(entities.back().get());

							if (doubleWidth)
							{
								entities.emplace_back(new Entity{type, pos + Vec2(1, 0)});
								walls.push_back(entities.back().get());
							}
							break;

						case EntityType::Robot:
							entities.emplace_back(new Entity{type, pos});
							robot = entities.back().get();
							break;
					}
				}
			}
		}

		const Entity* getEntityAtPos(Vec2 pos) const
		{
			for (const std::unique_ptr<Entity>& entity : entities)
			{
				if (entity->pos == pos)
					return entity.get();

				if (isDoubleWidth
					&& entity->type == EntityType::Box
					&& entity->pos + Vec2(1, 0) == pos)
					return entity.get();
			}

			return nullptr;
		}

		Entity* getEntityAtPos(Vec2 pos)
		{
			return const_cast<Entity*>(static_cast<const Grid*>(this)->getEntityAtPos(pos));
		}

		bool isEmpty(Vec2 pos) const
		{
			return getEntityAtPos(pos) == nullptr;
		}

		void moveRobot(Vec2 delta)
		{
			const bool isVertical = (delta.x == 0);

			std::set<Entity*> pushedBoxes;
			std::vector<Vec2> posQueue;
			posQueue.push_back(robot->pos + delta);

			for (size_t i = 0; i < posQueue.size(); ++i)
			{
				const Vec2 pos = posQueue[i];
				Entity* entity = getEntityAtPos(pos);

				if (entity && entity->type == EntityType::Wall)
					return;

				if (entity)
				{
					assert(entity->type == EntityType::Box);
					pushedBoxes.insert(entity);
					posQueue.push_back(pos + delta);

					if (isVertical && isDoubleWidth)
					{
						if (pos == entity->pos)
							posQueue.push_back(pos + Vec2(1, 0) + delta);
						else
							posQueue.push_back(pos - Vec2(1, 0) + delta);
					}
				}
			}

			for (Entity* entity : pushedBoxes)
			{
				assert(entity->type == EntityType::Box);
				entity->pos += delta;
			}

			robot->pos += delta;
		}

		void render()
		{
			renderer->clear();

			for (const std::unique_ptr<Entity>& entity : entities)
			{
				const Vec2 pos = entity->pos;

				switch (entity->type)
				{
					case EntityType::None: renderer->plot(pos, '.'); break;
					case EntityType::Wall: renderer->plot(pos, '#'); break;
					case EntityType::Robot: renderer->plot(pos, '@'); break;

					case EntityType::Box:
					{
						if (isDoubleWidth)
						{
							renderer->plot(pos, '['); 
							renderer->plot(pos + Vec2(1, 0), ']'); 
						}
						else
						{
							renderer->plot(pos, 'O'); 
						}
						break;
					}

					default:
						assert(false);
				}
			}

			renderer->render();
			renderer->sleep(16);
		}

		uint64_t sumBoxGPSCoords() const
		{
			uint64_t sum = 0;

			for (Entity* box : boxes)
			{
				const Vec2 pos = box->pos;
				const uint64_t gps = (pos.y * 100) + pos.x;
				sum += gps;
			}

			return sum;
		}

		uint64_t execute(const std::vector<Direction>& instructions)
		{
			//renderer->clearScreen();

			for (const Direction dir : instructions)
			{
				moveRobot(Vec2::getDirection(dir));
				//render();
			}

			return sumBoxGPSCoords();
		}
	};

	static uint64_t partOne(const Data15& data)
	{
		constexpr bool doubleWidth = false;
		return Grid(data, doubleWidth).execute(data.instructions);
	}

	static uint64_t partTwo(const Data15& data)
	{
		constexpr bool doubleWidth = true;
		return Grid(data, doubleWidth).execute(data.instructions);
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

void day15()
{
	d15::processPrintAndAssert("../data/15/test.txt", std::make_pair(10092ull, 9021ull));
	d15::processPrintAndAssert("../data/15/test2.txt", std::make_pair(2028ull, 1751ull));
	d15::processPrintAndAssert("../data/15/real.txt", std::make_pair(1406628ull, 1432781ull));
}
