#include "Renderer.h"

#include "Windows.h"
#include <iostream>
#include <chrono>
#include <thread>

Renderer::Renderer(const Bounds& bounds)
	: Renderer(bounds.botRght.x, bounds.botRght.y)
{
}

Renderer::Renderer(size_t width, size_t height)
	: width(width)
	, height(height)
	, emptyGrid(height, std::string(width, ' '))
{
	HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleScreenBufferSize(output, {200, 200});

	clear();
}

void Renderer::clear()
{
	buffer = emptyGrid;
}

void Renderer::clearScreen()
{
	system("cls");
}

void Renderer::plot(Vec2 pos, char c)
{
	buffer[pos.y][pos.x] = c;
}

void Renderer::render(bool resetPos)
{
	if (resetPos)
	{
		COORD pos = {0, 0};
		static HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleCursorPosition(output, pos);
	}

	++iteration;
	std::cout << "Iteration: " << iteration << '\n';

	for (auto& line : buffer)
		std::cout << line << '\n';
}

std::string Renderer::concatenated() const
{
	std::string concat;

	for (auto& line : buffer)
		concat += line;

	return concat;
}

int Renderer::waitForInput()
{
	return std::cin.get();
}

void Renderer::sleep(int milliseconds)
{
	const std::chrono::duration<int, std::milli> duration{milliseconds};
	std::this_thread::sleep_for(duration);
}
