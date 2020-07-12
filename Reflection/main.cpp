#include "Reflection.h"
#include <utility>

struct MyType
{
	int a, b;
	char c;
	short d;
	float e;

	static constexpr auto get_members()
	{
		return std::make_tuple(
			Member("a", &MyType::a),
			Member("b", &MyType::b),
			Member("c", &MyType::c),
			Member("d", &MyType::d),
			Member("e", &MyType::e)
		);
	}
};

int main()
{
	MyType val;
	const auto mbrs = MyType::get_members();
	const auto set_zero = [&](const auto...mbr)
	{
		(((val.*mbr.pointer = 0)), ...);
	};
	std::apply(set_zero, mbrs);
}