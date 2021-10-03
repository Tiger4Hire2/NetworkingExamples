#include "Common/Dispatcher.h"
#include "Common/Dispatcher_impl.h"
#include "Common/Reflection.h"
#include "Common/Json.h"
#include <iostream>
#include "GlobalSystem.h"
#include "GlobalObject.h"

namespace Geometry
{
struct Point
{
    static constexpr const char Name[] = "Point";
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

    static constexpr const char Name[] = "Line";
	static constexpr auto get_members()
	{
		return std::make_tuple(
			Member("from", &Line::from),
			Member("to", &Line::to)
		);
	}
};
}

class MyContainer
    : Handle<Geometry::Point>
    , Handle<Geometry::Line>
{
    using Handle<Geometry::Point>::Send;
    using Handle<Geometry::Line>::Send;
    using Handle<Geometry::Point>::Dispatch;
    using Handle<Geometry::Line>::Dispatch;
    void HandleMsg(const Geometry::Point& msg) override;
    void HandleMsg(const Geometry::Line& msg) override;
public:
    void Action();
};




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
    for (const auto& s : standalone.GetRegisteredTypes())
        std::cout << s << "\n";
    GlobalSystem system;
    for (const auto& s : system.dispatcher.GetRegisteredTypes())
        std::cout << s << "\n";
    Thing thing;

    MyContainer thing2;
    thing.Action();
    std::cout << "Dispatch global system\n";
    system.dispatcher.Dispatch();

    std::cout << "Default system\n";
    thing2.Action();
    standalone.Dispatch();
}