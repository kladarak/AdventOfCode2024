#pragma once

#include <vector>
#include <string>

#include "Vec2.h"

class Renderer
{
public:
	explicit Renderer(const Bounds& bounds);
	Renderer(size_t width, size_t height);

	void clear();
	void clearScreen();

	void plot(Vec2 pos, char c);

	void render(bool resetPos = true);

	int waitForInput();
	void sleep(int milliseconds);

	std::string concatenated() const;
	const std::vector<std::string>& getBuffer() const { return buffer; }

private:
	const size_t width;
	const size_t height;
	std::vector<std::string> emptyGrid;
	std::vector<std::string> buffer;
	int iteration = 0;
};