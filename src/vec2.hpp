#pragma once

template<typename T>
struct vec2
{
	T x, y;
	vec2(T x = 0, T y = 0);

	static vec2<T> one();
};

template<typename T>
vec2<T>::vec2(T x, T y) : x(x), y(y)
{

}

template<typename T>
vec2<T> operator+(const vec2<T> &a, const vec2<T> &b)
{
	return vec2<T>(a.x + b.x, a.y + b.y);
}

template<typename T>
vec2<T> operator-(const vec2<T> &a, const vec2<T> &b)
{
	return vec2<T>(a.x - b.x, a.y - b.y);
}

template<typename T>
vec2<T> operator*(const vec2<T> &a, const T &b)
{
	return vec2<T>(a.x * b, a.y * b);
}

template<typename T>
vec2<T> operator/(const vec2<T> &a, const T &b)
{
	return vec2<T>(a.x / b, a.y / b);
}

template<typename T>
vec2<T> vec2<T>::one() { return vec2<T>(1, 1); }
