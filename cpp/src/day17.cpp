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

namespace d17
{
	struct Program
	{
		uint64_t registerA = 0;
		uint64_t registerB = 0;
		uint64_t registerC = 0;
		std::string program;
		std::vector<std::pair<uint8_t, uint8_t>> instructions;

		void load()
		{
			std::vector<uint8_t> numbers;

			size_t pos = 0;
			while (pos != std::string::npos)
			{
				const char* sub = &program[pos];
				const uint32_t num = std::stoul(sub);
				numbers.push_back(static_cast<uint8_t>(num));

				pos = program.find(',', pos + 1);
				if (pos != std::string::npos)
					++pos;
			}

			assert(numbers.size() % 2 == 0);

			for (size_t i = 0; i < numbers.size(); i += 2)
				pushInst(numbers[i], numbers[i+1]);
		}

		void pushInst(uint8_t a, uint8_t b)
		{
			instructions.push_back(std::make_pair(a, b));
		}
	};

	struct Processor
	{
		uint64_t registerA = 0;
		uint64_t registerB = 0;
		uint64_t registerC = 0;
		int64_t programCounter = 0;
		std::string output;

		uint64_t resolveComboOperand(uint8_t v)
		{
			switch (v)
			{
				case 0: return 0;
				case 1: return 1;
				case 2: return 2;
				case 3: return 3;
				case 4: return registerA;
				case 5: return registerB;
				case 6: return registerC;
				case 7: assert(false); return 0;
				default: assert(false); return 0;
			}
		}

		void adv(uint8_t v)
		{
			const uint64_t c = resolveComboOperand(v);
			registerA = registerA / (1ull << c);
		}

		void bxl(uint8_t v)
		{
			registerB = registerB ^ v;
		}

		void bst(uint8_t v)
		{
			const uint64_t c = resolveComboOperand(v);
			registerB = c % 8;
		}

		void jnz(uint8_t v)
		{
			if (registerA != 0)
			{
				programCounter = v;
				--programCounter;
			}
		}

		void bxc(uint8_t)
		{
			registerB = registerB ^ registerC;
		}

		void out(uint8_t v)
		{
			const uint64_t c = resolveComboOperand(v);
			
			if (!output.empty())
				output += ',';

			output += std::to_string(c % 8);
		}

		void bdv(uint8_t v)
		{
			const uint64_t c = resolveComboOperand(v);
			registerB = registerA / (1ull << c);
		}

		void cdv(uint8_t v)
		{
			const uint64_t c = resolveComboOperand(v);
			registerC = registerA / (1ull << c);
		}

		void process(uint8_t opCode, uint8_t operand)
		{
			switch (opCode)
			{
				case 0: adv(operand); break;
				case 1: bxl(operand); break;
				case 2: bst(operand); break;
				case 3: jnz(operand); break;
				case 4: bxc(operand); break;
				case 5: out(operand); break;
				case 6: bdv(operand); break;
				case 7: cdv(operand); break;
				default: assert(false);
			}
		}

		void execute(const Program& program)
		{
			registerA = program.registerA;
			registerB = program.registerB;
			registerC = program.registerC;
			programCounter = 0;
			output = "";

			while (programCounter < (int) program.instructions.size())
			{
				assert(programCounter >= 0);
				auto& inst = program.instructions[programCounter];
				process(inst.first, inst.second);
				++programCounter;
			}
		}

		static void runUnitTests()
		{
			Processor proc;

			{
				Program p;
				p.registerC = 9;
				p.program = "2,6";
				p.load();
				proc.execute(p);
				assert(proc.registerB == 1);
			}
			{
				Program p;
				p.registerA = 10;
				p.program = "5,0,5,1,5,4";
				p.load();
				proc.execute(p);
				assert(proc.output == "0,1,2");
			}
			{
				Program p;
				p.registerA = 2024;
				p.program = "0,1,5,4,3,0";
				p.load();
				proc.execute(p);
				assert(proc.output == "4,2,5,6,7,7,7,7,3,1,0");
				assert(proc.registerA == 0);
			}
			{
				Program p;
				p.registerB = 29;
				p.program = "1,7";
				p.load();
				proc.execute(p);
				assert(proc.registerB == 26);
			}
			{
				Program p;
				p.registerB = 2024;
				p.registerC = 43690;
				p.program = "4,0";
				p.load();
				proc.execute(p);
				assert(proc.registerB == 44354);
			}
			{
				Program p;
				p.registerA = 117440;
				p.program = "0,3,5,4,3,0";
				p.load();
				proc.execute(p);
				assert(proc.output == "0,3,5,4,3,0");
			}
		}
	};

	struct Data17
	{
		Program program;
	};

	static auto loadData(const char* filename)
	{
		std::fstream s{ filename, s.in };
		assert(s.is_open());

		std::vector<std::string> lines;

		while (s.peek() != EOF)
		{
			std::string line;
			std::getline(s, line);
	
			if (!line.empty())
				lines.push_back(std::move(line));
		}

		assert(lines.size() == 4);

		auto readRegister = [] (const std::string& line)
		{
			const size_t pos = line.find(':');
			return std::stoull(&line[pos + 2]);
		};

		Data17 data;
		data.program.registerA = readRegister(lines[0]);
		data.program.registerB = readRegister(lines[1]);
		data.program.registerC = readRegister(lines[2]);
		data.program.program = lines[3].substr(lines[3].find(' ') + 1);
		data.program.load();

		return data;
	}

	static std::string partOne(const Data17& data)
	{
		Processor proc;
		proc.execute(data.program);
		return proc.output;
	}

	static uint64_t pt2Recurse(Program prog, const size_t searchPos)
	{
		const bool isFirstIteration = (searchPos == (prog.program.size() - 1));
		const char searchChar = prog.program[searchPos];
		const std::string foundSoFar = !isFirstIteration ? prog.program.substr(searchPos + 2) : std::string();

		while (true)
		{
			Processor proc;
			proc.execute(prog);

			if (proc.output.front() == searchChar)
			{
				if (searchPos == 0)
				{
					// We're done! return result
					assert(proc.output == prog.program);
					return prog.registerA;
				}
				else
				{
					std::cout
						<< "A: " << prog.registerA
						<< " Output: " << proc.output 
						<< std::endl;

					// Recurse
					Program progCopy = prog;
					progCopy.registerA *= 8;
					if (const uint64_t result = pt2Recurse(progCopy, searchPos - 2))
						return result;
				}
			}

			prog.registerA++;

			if (!isFirstIteration)
				if (proc.output.size() < 2 || proc.output.substr(2) != foundSoFar)
					return 0; // rolled over, give up on this branch
		}

		return 0;
	}

	static uint64_t partTwo(const Data17& data)
	{
		Program prog = data.program;
		prog.registerA = 0;
		return pt2Recurse(prog, prog.program.size() - 1);
	}

	static void processPrintAndAssert(const char* filename, const std::string& expected1, uint64_t expected2)
	{
		const auto data = loadData(filename);
		const auto result1 = partOne(data);
		const auto result2 = expected2 != UINT64_MAX ? partTwo(data) : 0ull;

		std::cout << "Part 1: " << result1 << " Part 2: " << result2 << std::endl;

		assert(expected1 == "" || result1 == expected1);
		assert(expected2 == 0 || expected2 == UINT64_MAX || result2 == expected2);
	}
}

void day17()
{
	d17::Processor::runUnitTests();
	d17::processPrintAndAssert("../data/17/test.txt",  "4,6,3,5,6,3,5,2,1,0", UINT64_MAX);
	d17::processPrintAndAssert("../data/17/test2.txt", "5,7,3,0",             117440ull);
	d17::processPrintAndAssert("../data/17/real.txt",  "7,4,2,0,5,0,5,3,7",   202991746427434ull);
}
