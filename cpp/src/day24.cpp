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

namespace d24
{
	enum class Operator
	{
		AND,
		OR,
		XOR
	};

	struct GateData
	{
		std::string input1;
		std::string input2;
		std::string output;
		Operator op = Operator::AND;
	};

	struct Data24
	{
		std::set<std::string> wireIds;
		std::map<std::string, bool> wireValues; 
		std::vector<GateData> gates;
	};

	static auto loadData(const char* filename)
	{
		Data24 data;

		bool parseWires = true;

		for (const std::string& line : String::readLines(filename))
		{
			if (line.empty())
			{
				parseWires = false;
			}
			else if (parseWires)
			{
				auto parts = String::delimit(line, ": ");
				assert(parts.size() == 2);
				assert(parts[1].size() == 1);
				data.wireIds.insert(parts[0]);
				data.wireValues[parts[0]] = (parts[1][0] == '1');
			}
			else
			{
				auto parts = String::delimit(line, ' ');
				assert(parts.size() == 5);

				GateData g;
				g.input1 = parts[0];
				g.input2 = parts[2];
				g.output = parts[4];

				data.wireIds.insert(g.input1);
				data.wireIds.insert(g.input2);
				data.wireIds.insert(g.output);

				if (parts[1] == "AND")
					g.op = Operator::AND;
				else if (parts[1] == "OR")
					g.op = Operator::OR;
				else if (parts[1] == "XOR")
					g.op = Operator::XOR;
				else
					assert(false);

				data.gates.push_back(g);
			}
		}

		return data;
	}

	struct Wire;
	struct Gate;

	struct Wire
	{
		std::string id;
		std::optional<bool> value;
		
		Gate* from = nullptr;
		std::set<Gate*> to;
	};

	struct Gate
	{
		Wire* input1 = nullptr;
		Wire* input2 = nullptr;
		Wire* output = nullptr;
		Operator op = Operator::AND;

		Wire* getOtherInput(Wire* input)
		{
			assert(input == input1 || input == input2);
			return input == input1 ? input2 : input1;
		}

		void operate()
		{
			assert(input1);
			assert(input2);
			assert(output);
			assert(input1->value);
			assert(input2->value);
			assert(!output->value);

			bool in1 = input1->value.value();
			bool in2 = input2->value.value();

			switch(op)
			{
				case Operator::AND:
					output->value = in1 && in2;
					break;
				case Operator::OR:
					output->value = in1 || in2;
					break;
				case Operator::XOR:
					output->value = in1 ^ in2;
					break;
			}

			assert(output->value);
		}
	};

	struct Network
	{
		std::vector<Wire> wires;
		std::vector<Gate> gates;
		std::map<std::string, Wire*> wireMap;
		std::map<std::string, Gate*> gateMap;
		uint64_t xValue = 0;
		uint64_t yValue = 0;
		uint64_t zValue = 0;

		explicit Network(const Data24& data)
		{
			wires.resize(data.wireIds.size());
			gates.resize(data.gates.size());

			size_t i = 0;
			for (const std::string& id : data.wireIds)
			{
				wires[i].id = id;
				wireMap[id] = &wires[i];
				++i;
			}

			for (auto& iter : data.wireValues)
				wireMap.find(iter.first)->second->value = iter.second;

			for (i = 0; i < data.gates.size(); ++i)
			{
				const GateData& gData = data.gates[i];
				Wire* input1 = wireMap.find(gData.input1)->second;
				Wire* input2 = wireMap.find(gData.input2)->second;
				Wire* output = wireMap.find(gData.output)->second;

				Gate& gate = gates[i];
				gate.input1 = input1;
				gate.input2 = input2;
				gate.output = output;
				gate.op = gData.op;
				gateMap[output->id] = &gate;

				input1->to.insert(&gate);
				input2->to.insert(&gate);
				output->from = &gate;
			}

			xValue = parseWires('x');
			yValue = parseWires('y');
		}

		Wire* getWire(char c, size_t bitIndex)
		{
			std::string id;
			id += c;
			id += std::to_string(bitIndex);

			if (id.size() == 2)
				id.insert(id.begin() + 1, '0');

			assert(id.size() == 3);

			return getWire(id);
		}
		Wire* getWire(const std::string& id)
		{
			return wireMap.find(id)->second;
		}

		uint64_t parseWires(char c)
		{
			uint64_t value = 0;
			for (const Wire& wire : wires)
			{
				if (wire.id.front() == c)
				{
					uint32_t num = std::stoul(&wire.id[1]);
					uint64_t val = wire.value.value() ? 1 : 0;
					value |= val << num;
				}
			}
			return value;
		}

		void execute()
		{
			for (Wire& wire : wires)
			{
				if (!wire.value)
					execGate(*wire.from);
			}

			zValue = parseWires('z');
		}
		void execGate(Gate& gate)
		{
			assert(!gate.output->value);

			if (!gate.input1->value)
			{
				execGate(*gate.input1->from);
				assert(gate.input1->value);
			}

			if (!gate.input2->value)
			{
				execGate(*gate.input2->from);
				assert(gate.input2->value);
			}

			assert(!gate.output->value);

			gate.operate();

			assert(gate.output->value);
		}

		void swapGateOutputs(const std::string& id1, const std::string& id2)
		{
			Gate* g1 = gateMap.find(id1)->second;
			Gate* g2 = gateMap.find(id2)->second;
			std::swap(g1->output, g2->output);
			gateMap[id1] = g2;
			gateMap[id2] = g1;
		}
	};

	static uint64_t partOne(const Data24& data)
	{
		Network network(data);
		network.execute();
		return network.zValue;
	}

	struct BitGate
	{
		size_t bitIndex = 0;

		Wire* inputX = nullptr;
		Wire* inputY = nullptr;
		Wire* inputC = nullptr; // carry
		Wire* bufferZ = nullptr; 
		Wire* bufferC1 = nullptr; 
		Wire* bufferC2 = nullptr; 
		Wire* outputZ = nullptr;
		Wire* outputC = nullptr;

		Gate* inXORGate = nullptr;
		Gate* inANDGate = nullptr;
		Gate* carryXORGate = nullptr;
		Gate* carryANDGate = nullptr;
		Gate* carryORGate = nullptr;

		// inX XOR inY -> bufferZ (or outputZ if 0 bit)
		// inX AND inY -> bufferC1 (or outputC if 0 bit)
		// inC XOR bufferZ -> outputZ
		// inC AND bufferZ -> bufferC2
		// bufferC1 || bufferC2 -> outputC
	};

	static std::string partTwo(const Data24& data)
	{
		// It's an ALU. And looking at the data, a "perfect" one (no erroneous connections).
		// So, build it up, identify the buffer gates, and detect where it seems incorrect.

		Network network(data);

		network.swapGateOutputs("nqk", "z07");
		network.swapGateOutputs("fpq", "z24");
		network.swapGateOutputs("srn", "z32");
		network.swapGateOutputs("pcp", "fgt");
		
		std::vector<BitGate> bitGates(data.wireValues.size() / 2, BitGate());
		for (size_t i = 0; i < bitGates.size(); ++i)
		{
			BitGate& bitGate = bitGates[i];

			bitGate.bitIndex = i;

			bitGate.inputX = network.getWire('x', i);
			bitGate.inputY = network.getWire('y', i);
			bitGate.outputZ = network.getWire('z', i);

			for (Gate* gate : bitGate.inputX->to)
			{
				assert(gate->getOtherInput(bitGate.inputX) == bitGate.inputY);
				assert(gate->op == Operator::XOR || gate->op == Operator::AND);

				if (gate->op == Operator::XOR)
				{
					if (i == 0)
					{
						if (gate->output != bitGate.outputZ)
						{
							std::cout << "Error at " << i << " (inXOR gate has wrong output)"
								<< ": gate->output: " << gate->output->id
								<< ", bitGate.outputZ: " << bitGate.outputZ->id
								<< std::endl;
						}
					}
					else
					{
						bitGate.bufferZ = gate->output;
					}

					bitGate.inXORGate = gate;
				}
				else if (gate->op == Operator::AND)
				{
					((i == 0) ? bitGate.outputC : bitGate.bufferC1) = gate->output;
					bitGate.inANDGate = gate;
				}
				else
				{
					assert(false);
				}
			}

			assert(i == 0 || bitGate.bufferZ);
			if (i == 0)
				continue;

			Wire* prevCarry = bitGates[i-1].outputC;

			for (Gate* gate : bitGate.bufferZ->to)
			{
				Wire* otherInput = gate->getOtherInput(bitGate.bufferZ);
				
				if (gate->op == Operator::XOR)
				{
					if (otherInput != prevCarry)
					{
						std::cout << "Error at " << i << " (carryXOR wrong prevCarry (wrong output on prev carryORGate))"
							<< ": prevCarry: " << (prevCarry ? prevCarry->id : "null")
							<< ", bufferZ: " << bitGate.bufferZ->id
							<< ", otherInput: " << otherInput->id
							<< std::endl;
					}

					if (gate->output != bitGate.outputZ)
					{
						std::cout << "Error at " << i << " (carryXOR gate has wrong output)"
							<< ": gate->output: " << gate->output->id
							<< ", bitGate.outputZ: " << bitGate.outputZ->id
							<< std::endl;
					}

					bitGate.carryXORGate = gate;
				}
				else if (gate->op == Operator::AND)
				{
					if (otherInput != prevCarry)
					{
						std::cout << "Error at " << i << " (carryAND wrong prevCarry (wrong output on prev carryORGate))"
							<< ": prevCarry: " << (prevCarry ? prevCarry->id : "null")
							<< ", bufferZ: " << bitGate.bufferZ->id
							<< ", otherInput: " << otherInput->id
							<< std::endl;
					}

					bitGate.bufferC2 = gate->output;
					bitGate.carryANDGate = gate;
				}
				else
				{
					std::cout << "Error at " << i << " (OR wrong prevCarry (wrong output on prev inORGate))"
						<< ": prevCarry: " << (prevCarry ? prevCarry->id : "null")
						<< std::endl;
				}
			}

			if (bitGate.bufferC1 && bitGate.bufferC1->to.size() == 0)
			{
				std::cout << "Error at " << i << " (no outputs on bufferC1 - wrong output on inAND gate)"
					<< ": bufferC1: " << bitGate.bufferC1->id
					<< std::endl;
			}

			if (bitGate.bufferC1 && bitGate.bufferC1->to.size() > 1)
			{
				std::cout << "Error at " << i << " (too many outputs on bufferC1 - wrong output on inAND gate)"
					<< ": bufferC1: " << bitGate.bufferC1->id
					<< std::endl;
			}

			if (bitGate.bufferC2 && bitGate.bufferC2->to.size() == 0)
			{
				std::cout << "Error at " << i << " (no outputs on bufferC2 - wrong output on carryAND gate)"
					<< ": bufferC2: " << bitGate.bufferC2->id
					<< std::endl;
			}

			if (bitGate.bufferC2 && bitGate.bufferC2->to.size() > 1)
			{
				std::cout << "Error at " << i << " (too many outputs on bufferC2 - wrong output on carryAND gate)"
					<< ": bufferC2: " << bitGate.bufferC2->id
					<< std::endl;
			}
			
			if (bitGate.bufferC1 && bitGate.bufferC2 && bitGate.bufferC1->to.size() > 0)
			{
				Gate* outC = *bitGate.bufferC1->to.begin();
				Wire* otherWire = outC->getOtherInput(bitGate.bufferC1);
				if (otherWire != bitGate.bufferC2)
				{
					std::cout << "Error at " << i << " (carryOR has wrong inputs - wrong output bufferC1 or bufferC2 (inAND or carryAND)"
						<< ": bufferC2: " << bitGate.bufferC2->id
						<< std::endl;
				}

				bitGate.outputC = outC->output;
				bitGate.carryORGate = outC;
			}
		}

		// summary?
		std::cout << "Summary Report of Incorrect Outputs" << std::endl;

		for (BitGate& gate : bitGates)
		{
			if (gate.bitIndex == 0)
			{
				if (gate.inXORGate->output != gate.outputZ)
					std::cout << "Gate " << gate.bitIndex << " inXOR, wire: " << gate.inXORGate->output->id << std::endl;

				Wire* carry = gate.inANDGate->output;
				if (carry->to.size() != 2)
					std::cout << "Gate " << gate.bitIndex << " inAND, wire: " << gate.inANDGate->output->id << std::endl;
				else
				{
					for (Gate* carryGate : carry->to)
					{
						if (carryGate->op == Operator::XOR)
						{
							if (carryGate->output != network.getWire('z', gate.bitIndex + 1))
								std::cout << "Gate " << gate.bitIndex << " inAND, wire: " << gate.inANDGate->output->id
								<< "or gate " << gate.bitIndex + 1 << " carryXOR, wire: " << carryGate->output->id << std::endl;
						}
					}
				}
			}
			else
			{
				auto isZOutput = [] (Wire* w) { return w->id.front() == 'z'; }; 
				if (gate.inXORGate && isZOutput(gate.inXORGate->output))
					std::cout << "Gate " << gate.bitIndex << " inXOR, wire: " << gate.inXORGate->output->id << std::endl;

				if (gate.inANDGate && isZOutput(gate.inANDGate->output))
					std::cout << "Gate " << gate.bitIndex << " inAND, wire: " << gate.inANDGate->output->id << std::endl;

				if (gate.carryXORGate && !isZOutput(gate.carryXORGate->output))
					std::cout << "Gate " << gate.bitIndex << " carryXOR, wire: " << gate.carryXORGate->output->id << std::endl;

				if (gate.carryANDGate && isZOutput(gate.carryANDGate->output))
					std::cout << "Gate " << gate.bitIndex << " carryAND, wire: " << gate.carryANDGate->output->id << std::endl;

				if (gate.carryORGate && isZOutput(gate.carryORGate->output))
					std::cout << "Gate " << gate.bitIndex << " carryOR, wire: " << gate.carryORGate->output->id << std::endl;

			}
		}

		// Manually figured out from above error reporting and some manual inspection
		return "fgt,fpq,nqk,pcp,srn,z07,z24,z32";
	}

	static void processPrintAndAssert(const char* filename, auto expected1, std::optional<std::string> expected2)
	{
		const auto data = loadData(filename);
		const auto result1 = partOne(data);
		const auto result2 = expected2 ? partTwo(data) : std::string();

		std::cout << "Part 1: " << result1 << " Part 2: " << result2 << std::endl;

		assert(expected1 == decltype(expected1){} || result1 == expected1);
		assert(!expected2 || expected2.value() == "" || result2 == expected2.value());
	}
}

void day24()
{
	d24::processPrintAndAssert("../data/24/test.txt", 4ull, {});
	d24::processPrintAndAssert("../data/24/test2.txt", 2024ull, {});
	d24::processPrintAndAssert("../data/24/real.txt", 61886126253040ull, "fgt,fpq,nqk,pcp,srn,z07,z24,z32");
}
