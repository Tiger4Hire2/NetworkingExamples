#pragma once
#include "Dispatcher.h"
// Functions for the dispatcher
template<class System>
Dispatcher<System>::Dispatcher() 
{
    assert(!p_inst); 
    p_inst=this;
    std::vector<SelfRegister<System>*> sort_buffer; 
    for (auto msg = message_list; msg; msg = msg->next)
        sort_buffer.push_back(msg);

    std::sort(sort_buffer.begin(), sort_buffer.end(), [](SelfRegister<System>* a, SelfRegister<System>*b)
    {
        return a->name < b->name;
    });
        
    for (const auto msg: sort_buffer)
    {
        p_inst->handlers.push_back(RegisteredHandler{msg->fn, nullptr});
        msg->id = static_cast<int>(p_inst->handlers.size()-1);
        msg->next = nullptr;
    }
    message_list = nullptr;
}

template<class System>
void Dispatcher<System>::RegisterMessage(SelfRegister<System>& details)
{
    assert(!p_inst);
    details.next = message_list;
    message_list = &details;
}


template<class System>
template <class Handler>
void Dispatcher<System>::Dispatch(const Handler*, const typename Handler::Message& msg)
{
    assert(p_inst);
    MessageID id{Handler::GetID()};
    const auto bytes = std::as_bytes(std::span(&msg,1));
    assert((int)handlers.size()>id);
    if (handlers[id].handler)
        handlers[id].parse_fn(bytes, handlers[id].handler);
//  else???? Maybe throw, maybe ignore, it really depends on the use case
}

template<class System>
void Dispatcher<System>::Dispatch()
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

template<class System>
template<class Msg, HandlerUniqueName Name>
void Dispatcher<System>::RegisterHandler(Handle<Msg, Name, System>& new_handler)
{
    assert(p_inst);
    const MessageID id{Handle<Msg, Name, System>::GetID()};
    assert((int)handlers.size()>id);
    assert(!handlers[id].handler);
    handlers[id].handler = &new_handler;
}

template<class System>
template<class Msg, HandlerUniqueName Name>
void Dispatcher<System>::MoveHandler(Handle<Msg, Name, System>& old_handler, Handle<Msg, Name, System>& new_handler)
{
    assert(p_inst);
    const MessageID id{Handle<Msg, Name, System>::GetID()};
    assert((int)handlers.size()>id);
    assert(handlers[id].handler == &old_handler);
    handlers[id].handler = &new_handler;
}
