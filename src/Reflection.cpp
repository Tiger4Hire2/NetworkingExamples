#include "Common/Reflection.h"
#include "Common/Json.h"
#include "Common/Overloaded.h"
#include <utility>
#include <span>
#include <iostream>
#include <sstream>

struct Point
{
	int x, y;

	static constexpr auto get_members()
	{
		return std::make_tuple(
			Member("x", &Point::x),
			Member("y", &Point::y)
		);
	}
};

struct Line
{
	Point from, to;

	static constexpr auto get_members()
	{
		return std::make_tuple(
			Member("from", &Line::from),
			Member("to", &Line::to)
		);
	}
};

template<class T>
void ReverseEndian(T& v)
{
	const auto as_bytes = std::as_writable_bytes(std::span(&v, 1));
	std::reverse(as_bytes.begin(), as_bytes.end());
}

int main()
{
	Line l{ {0x12345678, 0x12345678}, {0x12345678, 0x12345678} };
	visit_recursive(l, [](auto& v) {ReverseEndian(v); });

	std::cout << std::hex;
	JSON(std::cout, l);
	std::cout << std::endl;

	std::istringstream is("{\"from\"={\"x\"=1}, \"to\"={\"y\"=2}}");
	JSON(is, l);

	std::cout << "Reread\n";
	JSON(std::cout, l);
	std::cout << std::endl;

	enumerate_recursive(l, overloaded{
		[](const ObjectStart&, std::string_view name){std::cout << name << " {\n";},
		[](const ObjectEnd&, std::string_view name){std::cout << "}\n";},
		[](const auto& ptr, std::string_view name){std::cout << name << "\n";}
	});
};