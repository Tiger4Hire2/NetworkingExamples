#pragma once
#include "Common/Dispatcher.h"

struct GlobalSystem
{
    struct MsgA
    {
        constexpr const char Name[] = "MsgA";
        int a;
    };

    struct MsgB
    {
        constexpr const char Name[] = "MsgB";
        int a,b;
    };

    Dispatcher<GlobalSystem> dispatcher;
};
