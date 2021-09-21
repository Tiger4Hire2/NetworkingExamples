#include "Common/Dispatcher.h"
#include <iostream>

struct GlobalSystem
{
    Dispatcher<GlobalSystem> dispatcher;
    template <class Msg> using Handle = Handle<Msg, GlobalSystem>;
    struct MsgA
    {
        int a;
    };

    struct MsgB
    {
        int a,b;
    };
};

struct MsgC
{
    int a,b;
};

class Thing
    : GlobalSystem::Handle<GlobalSystem::MsgA>
    , GlobalSystem::Handle<GlobalSystem::MsgB>
{
    using GlobalSystem::Handle<GlobalSystem::MsgA>::Send;
    using GlobalSystem::Handle<GlobalSystem::MsgB>::Send;
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

class Thing2
    : Handle<MsgC>
{
    using Handle<MsgC>::Send;
    void HandleMsg(const MsgC& msg) override {std::cout << "C:" << msg.a << "," << msg.b <<"\n";}
public:
    void Action()
    {
        Send(MsgC{1,2});
        Send(MsgC{3,4});
    }
};

int main()
{
    Dispatcher standalone;
    GlobalSystem system;
    Thing thing;
    Thing2 thing2;
    thing.Action();
    thing2.Action();
    system.dispatcher.Dispatch();
    standalone.Dispatch();
}