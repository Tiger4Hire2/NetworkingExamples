#pragma once
#include <span>
#include <set>
#include <vector>
#include "Reflection.h"
#include <assert.h>
#include <string_view>
using MemBlock = std::span<const std::byte>;
using HandlerUniqueName = const char[];

template<typename T> concept Message = requires(T t) { T::Name; };

// Set-up Default system templates
class DefaultSystem;
template <class System = DefaultSystem> struct SelfRegister;
template <Message Msg, class System = DefaultSystem, HandlerUniqueName DefName = Msg::Name> class Handle;



// Define our Dispatcher, which is a singleton. By using template, we can (at run time) make more singletons though.
template <class System = DefaultSystem>
class Dispatcher
{
public: //types
    using ParseFn = int (*)(MemBlock, void *);
    using MessageID = int;

private:
    struct RegisteredHandler
    {
        ParseFn parse_fn;
        void *handler;
        std::string_view name;
        std::set<void *> listeners;
    };

public:
    static Dispatcher &Get()
    {
        assert(p_inst);
        return *p_inst;
    }
    // not thread-safe, does not have to be
    Dispatcher();
    static void RegisterMessage(SelfRegister<System> &);
    template <Message Msg, HandlerUniqueName Name>
    void RegisterHandler(Handle<Msg, System, Name> &);
    template <Message Msg, HandlerUniqueName Name>
    void UnregisterHandler(Handle<Msg, System, Name> &);
    template <Message Msg, HandlerUniqueName Name>
    void MoveHandler(Handle<Msg, System, Name> &, Handle<Msg, System, Name> &);

    void Dispatch();
    template <class Handler>
    void Dispatch(const Handler *, const Handler::Message &);
    template <class Handler>
    void Send(const Handler *, const typename Handler::Message &);
    std::vector<std::byte> GetMessages()
    {
        std::vector<std::byte> retval;
        std::swap(retval, messages);
        return retval;
    }
    void SetMessages(std::vector<std::byte> &&new_messages)
    {
        messages = std::move(new_messages);
    }
    std::vector<std::string_view> GetRegisteredTypes() const;
private:
    static inline SelfRegister<System> *message_list{nullptr};
    static inline Dispatcher *p_inst{nullptr};
    std::vector<RegisteredHandler> handlers;
    std::vector<std::byte> messages;
};

// must be static
template <class System>
struct SelfRegister
{
    Dispatcher<System>::ParseFn fn;
    SelfRegister *next{nullptr};
    Dispatcher<System>::MessageID id{-1};
    std::string_view name;
    SelfRegister(Dispatcher<System>::ParseFn f, HandlerUniqueName literal_name)
        : fn(f), name(literal_name)
    {
        Dispatcher<System>::RegisterMessage(*this);
    }
};

template <Message Msg, class System, HandlerUniqueName Name>
class Handle
{
    static int Parse(MemBlock, void *);
    static inline SelfRegister<System> self_register{Parse, Name};

public:
    using Message = Msg;
    virtual void HandleMsg(const Msg &) = 0;
    static Dispatcher<System>::MessageID GetID() noexcept
    {
        assert(self_register.id >= 0);
        return self_register.id;
    }

    Handle() { Dispatcher<System>::Get().RegisterHandler(*this); }

    // non-copyable
    Handle(const Handle &) = delete;
    ~Handle() {Dispatcher<System>::Get().UnregisterHandler(*this);}
    Handle &operator==(const Handle &) = delete;
    // but is movable
    Handle(Handle &&old) { Dispatcher<System>::Get().MoveHandler(old, *this); }
    Handle &operator==(Handle &&old)
    {
        Dispatcher<System>::Get().MoveHandler(self_register.id, *this, old);
        return *this;
    }

    void Send(const Msg &msg);
    void Dispatch(const Msg &msg);
};

// we need to copy to respect the alligment rules
template <class Msg, class System, HandlerUniqueName Name>
int Handle<Msg, System, Name>::Parse(MemBlock mem, void *inst)
{
    assert(mem.size() >= sizeof(Msg));
    Msg copy;
    const auto tgt = std::as_writable_bytes(std::span(&copy, 1));
    std::copy_n(mem.begin(), tgt.size(), tgt.begin());
    if (inst)
    {
        Handle *concrete_inst = reinterpret_cast<Handle<Msg, System, Name> *>(inst);
        concrete_inst->HandleMsg(copy);
    }
    return tgt.size();
}

template <class Msg, class System, HandlerUniqueName Name>
void Handle<Msg, System, Name>::Send(const Msg &msg)
{
    auto &dispatcher = Dispatcher<System>::Get();
    dispatcher.Send(this, msg);
}

template <class Msg, class System, HandlerUniqueName Name>
void Handle<Msg, System, Name>::Dispatch(const Msg &msg)
{
    auto &dispatcher = Dispatcher<System>::Get();
    dispatcher.Dispatch(this, msg);
}

template <class System>
template <class Handler>
void Dispatcher<System>::Send(const Handler *, const typename Handler::Message &msg)
{
    assert(p_inst);
    MessageID id{Handler::GetID()};
    const auto bytes = std::as_bytes(std::span(&msg, 1));
    const auto size_of_new = sizeof(id) + bytes.size();
    messages.resize(messages.size() + size_of_new);
    const auto tgt = std::as_writable_bytes(std::span(messages)).last(size_of_new);
    const auto tgt1 = tgt.first(sizeof(id));
    const auto tgt2 = tgt.last(bytes.size());
    const auto src1 = std::as_bytes(std::span(&id, 1));
    std::copy(src1.begin(), src1.end(), tgt1.begin());
    std::copy(bytes.begin(), bytes.end(), tgt2.begin());
}
