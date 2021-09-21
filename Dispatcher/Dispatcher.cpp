#include "Common/Dispatcher.h"
#include <iostream>

struct GlobalSystem
{
    Dispatcher<GlobalSystem> dispatcher;
    struct MsgA
    {
        int a;
    };

    struct MsgB
    {
        int a,b;
    };
};

class Thing
    : Handle<GlobalSystem::MsgA, GlobalSystem>
    , Handle<GlobalSystem::MsgB, GlobalSystem>
{
    using Handle<GlobalSystem::MsgA, GlobalSystem>::Send;
    using Handle<GlobalSystem::MsgB, GlobalSystem>::Send;
    void HandleMsg(const GlobalSystem::MsgA& msg) override {std::cout << "A:" << msg.a << "\n";}
    void HandleMsg(const GlobalSystem::MsgB& msg) override {std::cout << "B:" << msg.a << "," << msg.b <<"\n";}
public:
    void Action()
    {
        Send(GlobalSystem::MsgA{1});
        Send(GlobalSystem::MsgA{1001});
        Send(GlobalSystem::MsgB{1,2});
        Send(GlobalSystem::MsgB{3,4});
    }
};

int main()
{
    GlobalSystem system;
    Thing thing;
    thing.Action();
    system.dispatcher.Dispatch();
}