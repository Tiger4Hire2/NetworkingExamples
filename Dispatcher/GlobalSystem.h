#pragma once
#include "Common/Dispatcher.h"

struct GlobalSystem
{
    struct MsgA
    {
        static constexpr const char Name[] = "MsgA";
        int a;
    };

    struct MsgB
    {
        static constexpr const char Name[] = "MsgB";
        int a,b;
    };

    Dispatcher<GlobalSystem> dispatcher;
};
