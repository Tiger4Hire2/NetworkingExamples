#include "GlobalSystem.h"

// some object that is an actor in a global system
class Thing
    : Handle<GlobalSystem::MsgA, GlobalSystem>
    , Handle<GlobalSystem::MsgB, GlobalSystem>
{
    using Handle<GlobalSystem::MsgA, GlobalSystem>::Send;
    using Handle<GlobalSystem::MsgB, GlobalSystem>::Send;
    void HandleMsg(const GlobalSystem::MsgA& msg) override;
    void HandleMsg(const GlobalSystem::MsgB& msg) override;
public:
    void Action();
};
