#pragma once

#include <numeric>

enum class Direction
{
	First,
	Up = First,
	Right,
	Down,
	Left,

	Count
};

inline Direction& operator++(Direction& d)
{
	++d;
	return d;
}

inline Direction operator++(Direction& d, int)
{
	Direction copy = d;
	++d;
	return copy;
}

template <typename T>
struct Vec2T
{
	T x = 0;
	T y = 0;

	friend Vec2T operator+(Vec2T l, Vec2T r) { return {l.x + r.x, l.y + r.y}; }
	friend Vec2T operator-(Vec2T l, Vec2T r){ return {l.x - r.x, l.y - r.y}; }
	friend Vec2T operator*(Vec2T l, T n) { return {l.x * n, l.y * n}; }

	Vec2T& operator+=(Vec2T r) { return *this = (*this + r); }
	Vec2T& operator-=(Vec2T r) { return *this = (*this - r); }
	Vec2T& operator*=(T n) { return *this = (*this * n); }

	friend Vec2T operator-(Vec2T r) { return Vec2T() - r; }


	bool operator==(Vec2T r) const { return x == r.x && y == r.y; }
	bool operator!=(Vec2T r) const { return !(*this == r); }

	bool operator<(Vec2T r) const
	{
		if (x != r.x)
			return x < r.x;
		else
			return y < r.y;
	}

	static const Vec2T directions[(int) Direction::Count];
	static const Vec2T getDirection(Direction d) { return directions[(int) d]; }
};

template <typename T>
const Vec2T<T> Vec2T<T>::directions[(int) Direction::Count] =
{
	{0, -1},
	{1, 0},
	{0, 1},
	{-1, 0},
};

using Vec2 = Vec2T<int>;
using Vec2i64 = Vec2T<int64_t>;

struct Bounds
{
	Vec2 topLeft;
	Vec2 botRght;

	bool isInBounds(Vec2 pos) const
	{
		return topLeft.x <= pos.x
			&& topLeft.y <= pos.y
			&& pos.x < botRght.x
			&& pos.y < botRght.y;
	}
};
