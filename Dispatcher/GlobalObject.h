#include "GlobalSystem.h"

// some object that is an actor in a global system
class Thing
    : SCOPE_HANDLE(GlobalSystem,MsgA)
    , SCOPE_HANDLE(GlobalSystem,MsgB)
{
    using SCOPE_HANDLE(GlobalSystem,MsgA)::Send;
    using SCOPE_HANDLE(GlobalSystem,MsgB)::Send;
    void HandleMsg(const GlobalSystem::MsgA& msg) override;
    void HandleMsg(const GlobalSystem::MsgB& msg) override;
public:
    void Action();
};
