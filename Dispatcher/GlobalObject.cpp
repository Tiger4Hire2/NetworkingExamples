#include "GlobalObject.h"
#include <iostream>


void Thing::HandleMsg(const GlobalSystem::MsgA& msg) 
{std::cout << "A:" << msg.a << "\n";}

void Thing::HandleMsg(const GlobalSystem::MsgB& msg) 
{std::cout << "B:" << msg.a << "," << msg.b <<"\n";}

void Thing::Action()
{
    Send(GlobalSystem::MsgA{1});
    Send(GlobalSystem::MsgA{1001});
    Send(GlobalSystem::MsgB{1,2});
    Send(GlobalSystem::MsgB{3,4});
}
