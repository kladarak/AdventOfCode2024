#pragma once

struct Vec2
{
	int x = 0;
	int y = 0;

	friend Vec2 operator+(Vec2 l, Vec2 r) { return {l.x + r.x, l.y + r.y}; }
	friend Vec2 operator-(Vec2 l, Vec2 r){ return {l.x - r.x, l.y - r.y}; }
	Vec2& operator+=(Vec2 r) { return *this = (*this + r); }
	Vec2& operator-=(Vec2 r) { return *this = (*this - r); }

	bool operator==(Vec2 r) const { return x == r.x && y == r.y; }
	bool operator!=(Vec2 r) const { return !(*this == r); }

	bool operator<(Vec2 r) const
	{
		if (x != r.x)
			return x < r.x;
		else
			return y < r.y;
	}
};
