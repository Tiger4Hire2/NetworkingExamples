#pragma once
#include "Common/Reflection.h"

struct CommonObj
{
	std::string text;
	time_t fixed_len_field;

	static constexpr auto get_members()
	{
		return std::make_tuple(
			Member("text", &CommonObj::text),
			Member("fixed_len_field", &CommonObj::fixed_len_field)
		);
	}
};