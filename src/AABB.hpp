#pragma once
#include "vec2.hpp"

template<typename T>
struct AABB
{
	vec2<T> p1, p2; 	

	static AABB<T> fromCenterAndDimensions(vec2<T> center, vec2<T> dimensions);

	bool contains(vec2<T> pt)
	{
		return p1.x < pt.x && pt.x < p2.x && p1.y < pt.y && pt.y < p2.y;
	}
}; 

template<typename T>
AABB<T> AABB<T>::fromCenterAndDimensions(vec2<T> center, vec2<T> dimensions)
{
	AABB<T> ret;
	ret.p1 = ret.p2 = center;
	ret.p1.x -= dimensions.x / 2;
	ret.p1.y -= dimensions.y / 2;
	ret.p2.x += dimensions.x / 2;
	ret.p2.y += dimensions.y / 2;
	return ret;
}