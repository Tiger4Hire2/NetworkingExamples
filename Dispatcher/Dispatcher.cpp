#include "Common/Dispatcher.h"
#include <iostream>

struct MsgA
{
    int a;
};

struct MsgB
{
    int a,b;
};

class Thing
    : Handle<MsgA>
    , Handle<MsgB>
{
    using Handle<MsgA>::Send;
    using Handle<MsgB>::Send;
    void HandleMsg(const MsgA& msg) override {std::cout << "A:" << msg.a << "\n";}
    void HandleMsg(const MsgB& msg) override {std::cout << "B:" << msg.a << "," << msg.b <<"\n";}
public:
    void Action()
    {
        Send(MsgA{1});
        Send(MsgA{1001});
        Send(MsgB{1,2});
        Send(MsgB{3,4});
    }
};

int main()
{
    Dispatcher dispatcher;    
    Thing thing;
    thing.Action();
    dispatcher.Dispatch();
}