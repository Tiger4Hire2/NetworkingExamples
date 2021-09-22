#include "Common/Dispatcher.h"
#include "Common/Dispatcher_impl.h"
#include "Common/Reflection.h"
#include "Common/Json.h"
#include <iostream>
#include "GlobalSystem.h"
#include "GlobalObject.h"

namespace Geometry
{
    struct Point;
    struct Line;
}

class MyContainer
    : HANDLE(Geometry::Point)
    , HANDLE(Geometry::Line)
{
    using HANDLE(Geometry::Point)::Send;
    using HANDLE(Geometry::Line)::Send;
    using HANDLE(Geometry::Point)::Dispatch;
    using HANDLE(Geometry::Line)::Dispatch;
    void HandleMsg(const Geometry::Point& msg) override;
    void HandleMsg(const Geometry::Line& msg) override;
public:
    void Action();
};


namespace Geometry
{
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
}


void MyContainer::HandleMsg(const Geometry::Point& msg)
{
    JSON(std::cout, msg);
}
void MyContainer::HandleMsg(const Geometry::Line& msg) 
{ 
    JSON(std::cout, msg); 
}

void MyContainer::Action()
{
    Dispatch(Geometry::Line{32,16});
    Send(Geometry::Point{1,2});
    Send(Geometry::Line{Geometry::Point{3,4}, Geometry::Point{5,6}});
}


int main()
{
    Dispatcher standalone;
    GlobalSystem system;
    Thing thing;

    MyContainer thing2;
    thing.Action();
    std::cout << "Dispatch global system\n";
    system.dispatcher.Dispatch();

    std::cout << "Default system\n";
    thing2.Action();
    standalone.Dispatch();
}