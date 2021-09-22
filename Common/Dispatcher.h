#pragma once
#include <span>
#include <set>
#include <vector>
#include "Reflection.h"
#include <assert.h>
using MemBlock = std::span<const std::byte>;

// HandlerUniqueName/StringLiteral This is all to generate a unique name, so that the order in whuch handler types are registered
// is based on something other than the module linking order  
template<size_t N>
struct StringLiteral {
    template<size_t N2>
    constexpr StringLiteral(const char (&str)[N2]) {
        std::copy_n(str, std::min(N,N2), value);
    }  
    constexpr StringLiteral(std::string_view v) {
        std::copy_n(v.begin(), N, value);
    }  
    char value[N];
};
using HandlerUniqueName = StringLiteral<32>;

// Set-up Default system templates
class DefaultSystem;
template <class System = DefaultSystem> struct SelfRegister;
template<class Msg, HandlerUniqueName Name, class System = DefaultSystem> class Handle;

// Define our Dispatcher, which is a singleton. By using template, we can (at run time) make more singletons though. 
template<class System = DefaultSystem>
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
    static void RegisterMessage(SelfRegister<System>&);
    template <class Msg, HandlerUniqueName Name> void RegisterHandler(Handle<Msg, Name, System>&);
    template <class Msg, HandlerUniqueName Name> void MoveHandler(Handle<Msg, Name, System>&, Handle<Msg, Name, System>&);

    void Dispatch();
    template <class Handler> void Dispatch(const Handler*, const Handler::Message&);
    template <class Handler> void Send(const Handler*, const typename Handler::Message&);
    std::vector<std::byte> GetMessages() {
        std::vector<std::byte> retval;
        std::swap(retval, messages);
        return retval;
    }
    void SetMessages(std::vector<std::byte>&& new_messages) {
        messages = std::move(new_messages);
    }
private:
    static inline SelfRegister<System>* message_list{nullptr};
    static inline Dispatcher* p_inst{nullptr};
    std::vector<RegisteredHandler> handlers;
    std::vector<std::byte> messages;
};

// must be static
template<class System>
struct SelfRegister
{
    Dispatcher<System>::ParseFn     fn;
    SelfRegister*           next{nullptr};
    Dispatcher<System>::MessageID   id{-1};
    std::string_view        name;
    SelfRegister(Dispatcher<System>::ParseFn f, HandlerUniqueName literal_name)
        : fn(f)
        , name(literal_name.value)
    {
        Dispatcher<System>::RegisterMessage(*this);
    }
};

template<class Msg, HandlerUniqueName Name, class System>
class Handle
{
    static int Parse(MemBlock, void*);
    static inline SelfRegister<System> self_register{Parse, Name};
public:
    using Message = Msg;
    virtual void HandleMsg(const Msg&) = 0;
    static Dispatcher<System>::MessageID GetID() noexcept { assert(self_register.id>=0); return self_register.id; }

    Handle() {Dispatcher<System>::Get().RegisterHandler(*this);}

    // non-copyable
    Handle(const Handle&) = delete;
    Handle& operator==(const Handle&) = delete;
    // but is movable
    Handle(Handle&& old) {Dispatcher<System>::Get().MoveHandler(old, *this);}
    Handle& operator==(Handle&& old) { Dispatcher<System>::Get().MoveHandler(self_register.id, *this, old); return *this; }

    void Send(const Msg& msg);
    void Dispatch(const Msg& msg);
};
// annoying that we must use macros for this, but it's just convenient
#define HANDLE(X)   
#define SCOPE_HANDLE(X, M) Handle<X::M,#X"::"#M, GlobalSystem>


// we need to copy to respect the alligment rules
template<class Msg, HandlerUniqueName Name, class System>
int Handle<Msg, Name, System>::Parse(MemBlock mem, void* inst)
{
    assert(mem.size()>=sizeof(Msg));
    Msg copy;
    const auto tgt = std::as_writable_bytes(std::span(&copy,1));
    std::copy_n(mem.begin(), tgt.size(), tgt.begin() );
    if (inst)
    {
        Handle* concrete_inst = reinterpret_cast<Handle<Msg, Name, System>*>(inst);   
        concrete_inst->HandleMsg(copy);
    }
    return tgt.size();
}

template<class Msg, HandlerUniqueName Name, class System>
void Handle<Msg, Name, System>::Send(const Msg& msg) 
{
    auto& dispatcher = Dispatcher<System>::Get();
    dispatcher.Send(this, msg);
}

template<class Msg, HandlerUniqueName Name, class System>
void Handle<Msg, Name, System>::Dispatch(const Msg& msg)
{
    auto& dispatcher = Dispatcher<System>::Get();
    dispatcher.Dispatch(this, msg);
}

template<class System>
template <class Handler>
void Dispatcher<System>::Send(const Handler*, const typename Handler::Message& msg)
{
    assert(p_inst);
    MessageID id{Handler::GetID()};
    const auto bytes = std::as_bytes(std::span(&msg,1));
    const auto size_of_new = sizeof(id)+bytes.size();
    messages.resize(messages.size()+size_of_new);
    const auto tgt = std::as_writable_bytes(std::span(messages)).last(size_of_new);
    const auto tgt1 = tgt.first(sizeof(id));
    const auto tgt2 = tgt.last(bytes.size());
    const auto src1 = std::as_bytes(std::span(&id, 1));
    std::copy(src1.begin(), src1.end(), tgt1.begin());
    std::copy(bytes.begin(), bytes.end(), tgt2.begin());
}
