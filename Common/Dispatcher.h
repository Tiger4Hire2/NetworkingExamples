#pragma once
#include <span>
#include <set>
#include <vector>
#include "Reflection.h"
#include <assert.h>
using MemBlock = std::span<const std::byte>;

struct SelfRegister;
template<class Msg> class Handle;

class Dispatcher
{
public: //types
    using ParseFn = int (*)(MemBlock, void*);
    using MessageID = int;
private:
    struct RegisteredHandler
    {
        ParseFn parse_fn;
        void*   handler;
        std::set<void*> listeners;
    };
public:
    static Dispatcher& Get() {assert(p_inst); return *p_inst;} 
    // not thread-safe, does not have to be
    Dispatcher();
    static void RegisterMessage(SelfRegister&);
    template <class Msg> void RegisterHandler(Handle<Msg>&);
    template <class Msg> void MoveHandler(Handle<Msg>&, Handle<Msg>&);

    void Dispatch();
    template <class Msg> void Dispatch(std::span<const std::byte>);
    template <class Msg> void Send(std::span<const std::byte>);
    std::vector<std::byte> GetMessages() {
        std::vector<std::byte> retval;
        std::swap(retval, messages);
        return retval;
    }
    void SetMessages(std::vector<std::byte>&& new_messages) {
        messages = std::move(new_messages);
    }
private:
    static inline SelfRegister* message_list{nullptr};
    static inline Dispatcher* p_inst{nullptr};
    std::vector<RegisteredHandler> handlers;
    std::vector<std::byte> messages;
};

// must be static
struct SelfRegister
{
    Dispatcher::ParseFn     fn;
    SelfRegister*           next{nullptr};
    Dispatcher::MessageID   id{-1};
    SelfRegister(Dispatcher::ParseFn f)
        : fn(f)
    {
        Dispatcher::RegisterMessage(*this);
    }
};

template<class Msg>
class Handle
{
    static int Parse(MemBlock, void*);
    static inline SelfRegister self_register{Parse};
public:
    virtual void HandleMsg(const Msg&) = 0;
    static Dispatcher::MessageID GetID() noexcept { assert(self_register.id>=0); return self_register.id; }

    Handle() {
        (void)self_register;
        Dispatcher::Get().RegisterHandler(*this);
    }

    // non-copyable
    Handle(const Handle&) = delete;
    Handle& operator==(const Handle&) = delete;
    // but is movable
    Handle(Handle&& old) {Dispatcher::Get().MoveHandler(self_register.id, *this, old);}
    Handle& operator==(Handle&& old) { Dispatcher::Get().MoveHandler(self_register.id, *this, old); return *this; }

    void Send(const Msg& msg) {Dispatcher::Get().Send<Msg>(std::as_bytes(std::span(&msg,1))); }
    void Dispatch(const Msg& msg) {Dispatcher::Get().Dispatch<Msg>(std::as_bytes(std::span(&msg,1))); }
};

Dispatcher::Dispatcher() 
{
    assert(!p_inst); 
    p_inst=this;
    auto msg = message_list;
    while(msg)
    {
        p_inst->handlers.push_back(RegisteredHandler{msg->fn, nullptr});
        msg->id = static_cast<int>(p_inst->handlers.size()-1);
        const auto old_msg = msg;
        msg = msg->next;
        old_msg->next = nullptr;
    }
    message_list = nullptr;
}

void Dispatcher::RegisterMessage(SelfRegister& details)
{
    assert(!p_inst);
    details.next = message_list;
    message_list = &details;
}

template <class Msg>
void Dispatcher::Send(std::span<const std::byte> bytes)
{
    assert(p_inst);
    MessageID id{Handle<Msg>::GetID()};
    const auto size_of_new = sizeof(id)+bytes.size();
    messages.resize(messages.size()+size_of_new);
    const auto tgt = std::as_writable_bytes(std::span(messages)).last(size_of_new);
    const auto tgt1 = tgt.first(sizeof(id));
    const auto tgt2 = tgt.last(bytes.size());
    const auto src1 = std::as_bytes(std::span(&id, 1));
    std::copy(src1.begin(), src1.end(), tgt1.begin());
    std::copy(bytes.begin(), bytes.end(), tgt2.begin());
}

template <class Msg>
void Dispatcher::Dispatch(std::span<const std::byte> bytes)
{
    assert(p_inst);
    MessageID id{Handle<Msg>::GetID()};
    assert((int)handlers.size()>id);
    if (handlers[id].handler)
        handlers[id].parse_fn(bytes, handlers[id].handler);
//  else???? Maybe throw, maybe ignore, it really depends on the use case
}

void Dispatcher::Dispatch()
{
    assert(p_inst);

    auto bytes = std::span(messages);
    while (!bytes.empty())
    {
        MessageID id;
        const auto id_span = std::as_writable_bytes(std::span(&id,1));
        const auto id_src = bytes.first(id_span.size());
        bytes = bytes.subspan(id_span.size());
        std::copy(id_src.begin(), id_src.end(), id_span.begin());
        assert((int)handlers.size()>id);        
        const auto consumed = handlers[id].parse_fn(bytes, handlers[id].handler);
        bytes = bytes.subspan(consumed);
    }
    messages.clear();
}

template<class Msg>
void Dispatcher::RegisterHandler(Handle<Msg>& new_handler)
{
    assert(p_inst);
    const MessageID id{Handle<Msg>::GetID()};
    assert((int)handlers.size()>id);
    assert(!handlers[id].handler);
    handlers[id].handler = &new_handler;
}

template<class Msg>
void Dispatcher::MoveHandler(Handle<Msg>& old_handler, Handle<Msg>& new_handler)
{
    assert(p_inst);
    const MessageID id{Handle<Msg>::GetID()};
    assert((int)handlers.size()>id);
    assert(handlers[id].handler == &old_handler);
    handlers[id].handler = &new_handler;
}

// we need to copy to respect the alligment rules
template<class Msg>
int Handle<Msg>::Parse(MemBlock mem, void* inst)
{
    assert(mem.size()>=sizeof(Msg));
    Msg copy;
    const auto tgt = std::as_writable_bytes(std::span(&copy,1));
    std::copy_n(mem.begin(), tgt.size(), tgt.begin() );
    if (inst)
    {
        Handle* concrete_inst = reinterpret_cast<Handle<Msg>*>(inst);   
        concrete_inst->HandleMsg(copy);
    }
    return tgt.size();
}
