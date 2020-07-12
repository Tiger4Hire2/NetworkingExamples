#pragma once
#include "Common/Reflection.h"

struct CommonObj
{
	std::string text;
	char fixed_len_field[10];
	int number{ 0 };

	static constexpr auto get_members()
	{
		return std::make_tuple(
			Member("text", &CommonObj::text),
			Member("fixed_len_field", &CommonObj::fixed_len_field),
			Member("number", &CommonObj::number));
	}
};