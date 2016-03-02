#pragma once

#include "Precompiled.hpp"
#include "Collector.hpp"
#include "Receiver.hpp"

// Forward declarations.
template<typename Type>
class Receiver;

//
// Dispatcher
//
//  Holds a list of subscribed receivers that can be invoked all at once. 
//  More safe than using raw delegates, as unsubscribing is automated at 
//  receiver's destruction. No dangerous dangling pointers are left.
//
//  A single dispatcher instance can have multiple receivers subscribed,
//  but a single receiver can be subscribed to only one dispatcher.
//
//  Example usage:
//      struct EventData { /* ... */ };
//      
//      void Class::FunctionA(const EventData& event) { /*...*/ }
//      void Class::FunctionB(const EventData& event) { /*...*/ }
//      Class instance;
//      
//      Receiver<void(const EventData&)> receiverA;
//      receiverA.Bind<Class, &Class::FunctionA>(&instance);
//      
//      Receiver<void(const EventData&)> receiverB;
//      receiverB.Bind<Class, &Class::FunctionB>(&instance);
//      
//      Dispatcher<void(const EventData&)> dispatcher;
//      dispatcher.Subscribe(receiverA);
//      dispatcher.Subscribe(receiverB);
//      dispatcher.Dispatch(EventData(/* ... */));
//

// Dispatcher base class.
template<typename Type>
class DispatcherBase;

template<typename ReturnType, typename... Arguments>
class DispatcherBase<ReturnType(Arguments...)>
{
protected:
    DispatcherBase();
    virtual ~DispatcherBase();

    // Restores instance to it's original state.
    void Cleanup();

public:
    // Subscribes a receiver.
    void Subscribe(Receiver<ReturnType(Arguments...)>& receiver);

    // Unsubscribes a receiver.
    void Unsubscribe(Receiver<ReturnType(Arguments...)>& receiver);

    // Checks if has any subscribers.
    bool HasSubscribers() const;

protected:
    // Invokes receivers with following arguments.
    template<typename Collector>
    ReturnType Dispatch(Arguments... arguments);

    // Gets the previous receiver.
    Receiver<ReturnType(Arguments...)>* GetPrevious(Receiver<ReturnType(Arguments...)>& receiver);

private:
    // List of receivers.
    Receiver<ReturnType(Arguments...)>* m_begin;
    Receiver<ReturnType(Arguments...)>* m_end;
};

// Dispatcher class.
template<typename Type, class Collector = CollectDefault<typename std::function<Type>::result_type>>
class Dispatcher;

template<typename Collector, typename ReturnType, typename... Arguments>
class Dispatcher<ReturnType(Arguments...), Collector> : public DispatcherBase<ReturnType(Arguments...)>
{
public:
    // Restores instance to it's original state.
    void Cleanup();

    // Invokes receivers with following arguments.
    ReturnType Dispatch(Arguments... arguments);

    // Overloaded call operator that is used as dispatch.
    ReturnType operator()(Arguments... arguments);
};

// Receiver invoker class.
template<typename Type>
class ReceiverInvoker;

template<typename ReturnType, typename... Arguments>
class ReceiverInvoker<ReturnType(Arguments...)>
{
protected:
    ReturnType Dispatch(Receiver<ReturnType(Arguments...)>* receiver, Arguments... arguments)
    {
        return receiver->Receive(std::forward<Arguments>(arguments)...);
    }
};

// Collector invocation functor for non void return types.
template<typename Collector, typename Type>
class CollectorInvocation;

template<class Collector, typename ReturnType, typename... Arguments>
class CollectorInvocation<Collector, ReturnType(Arguments...)> : public ReceiverInvoker<ReturnType(Arguments...)>
{
public:
    bool operator()(Collector& collector, Receiver<ReturnType(Arguments...)>* receiver, Arguments... arguments)
    {
        return collector(this->Dispatch(receiver, std::forward<Arguments>(arguments)...));
    }
};

// Collector invocation functor for void return types.
template<class Collector, typename... Arguments>
class CollectorInvocation<Collector, void(Arguments...)> : public ReceiverInvoker<void(Arguments...)>
{
public:
    bool operator()(Collector& collector, Receiver<void(Arguments...)>* receiver, Arguments... arguments)
    {
        this->Dispatch(receiver, std::forward<Arguments>(arguments)...);
        return collector();
    }
};

// Template definitions.
template<typename ReturnType, typename... Arguments>
DispatcherBase<ReturnType(Arguments...)>::DispatcherBase() :
    m_begin(nullptr),
    m_end(nullptr)
{
}

template<typename ReturnType, typename... Arguments>
DispatcherBase<ReturnType(Arguments...)>::~DispatcherBase()
{
    Cleanup();
}

template<typename ReturnType, typename... Arguments>
void DispatcherBase<ReturnType(Arguments...)>::Cleanup()
{
    // Unsubscribe all receivers.
    Receiver<ReturnType(Arguments...)>* iterator = m_begin;
    
    while(iterator != nullptr)
    {
        Receiver<ReturnType(Arguments...)>* receiver = iterator;

        // Advance to the next receiver.
        iterator = iterator->m_next;

        // Unsubscribe a receiver.
        receiver->m_dispatcher = nullptr;
        receiver->m_next = nullptr;
    }
    
    m_begin = nullptr;
    m_end = nullptr;
}

template<typename ReturnType, typename... Arguments>
void DispatcherBase<ReturnType(Arguments...)>::Subscribe(Receiver<ReturnType(Arguments...)>& receiver)
{
    // Check if receiver is already subscribed somewhere else.
    if(receiver.m_dispatcher != nullptr)
        return;

    Assert(receiver.m_next == nullptr);
    
    // Add receiver to the linked list.
    if(m_begin == nullptr)
    {
        Assert(m_end == nullptr);
        m_begin = &receiver;
        m_end = &receiver;
    }
    else
    {
        Assert(m_end != nullptr);
        m_end->m_next = &receiver;
        m_end = &receiver;
    }

    // Set receiver's members.
    receiver.m_dispatcher = this;
}

template<typename ReturnType, typename... Arguments>
void DispatcherBase<ReturnType(Arguments...)>::Unsubscribe(Receiver<ReturnType(Arguments...)>& receiver)
{
    // Check if receiver is subscribed to this dispatcher.
    if(receiver.m_dispatcher != this)
        return;

    // Remove receiver from the linked list.
    if(m_begin == &receiver)
    {
        // Removing from the beginning of the list.
        if(m_end == &receiver)
        {
            m_begin = nullptr;
            m_end = nullptr;
        }
        else
        {
            m_begin = receiver.m_next; 
        }
    }
    else
    {
        // Removing from anywhere else in the list.
        Receiver<ReturnType(Arguments...)>* previous = this->GetPrevious(receiver);
        Assert(previous != nullptr);

        previous->m_next = receiver.m_next;

        if(m_end == &receiver)
        {
            m_end = previous;
        }
    }

    // Clear receiver's members.
    receiver.m_dispatcher = nullptr;
    receiver.m_next = nullptr;
}

template<typename ReturnType, typename... Arguments>
template<typename Collector>
ReturnType DispatcherBase<ReturnType(Arguments...)>::Dispatch(Arguments... arguments)
{
    // Create a result collector.
    Collector collector;

    // Send an event to all receivers.
    Receiver<ReturnType(Arguments...)>* receiver = m_begin;
        
    while(receiver != nullptr)
    {
        // Send an event to a receiver and collect the result.
        CollectorInvocation<Collector, ReturnType(Arguments...)> invocation;
        if(!invocation(collector, receiver, std::forward<Arguments>(arguments)...))
            break;

        // Advance to the next receiver.
        receiver = receiver->m_next;
    }

    // Return collected result.
    return collector.GetResult();
}

template<typename ReturnType, typename... Arguments>
Receiver<ReturnType(Arguments...)>* DispatcherBase<ReturnType(Arguments...)>::GetPrevious(Receiver<ReturnType(Arguments...)>& receiver)
{
    // Iterate through all receivers.
    Receiver<ReturnType(Arguments...)>* iterator = m_begin;
    Receiver<ReturnType(Arguments...)>* previous = nullptr;

    while(iterator != nullptr)
    {
        // Check if it's the one we are looking for.
        if(iterator == &receiver)
            break;

        // Save the previous iterator.
        previous = iterator;

        // Iterate to the next receiver.
        iterator = iterator->m_next;
    }

    // Return the previous receiver.
    return previous;
}

template<typename ReturnType, typename... Arguments>
bool DispatcherBase<ReturnType(Arguments...)>::HasSubscribers() const
{
    return m_begin != nullptr;
}

template<typename Collector, typename ReturnType, typename... Arguments>
void Dispatcher<ReturnType(Arguments...), Collector>::Cleanup()
{
    DispatcherBase<ReturnType(Arguments...)>::Cleanup();
}

template<typename Collector, typename ReturnType, typename... Arguments>
ReturnType Dispatcher<ReturnType(Arguments...), Collector>::Dispatch(Arguments... arguments)
{
    return DispatcherBase<ReturnType(Arguments...)>::Dispatch<Collector>(std::forward<Arguments>(arguments)...);
}

template<typename Collector, typename ReturnType, typename... Arguments>
ReturnType Dispatcher<ReturnType(Arguments...), Collector>::operator()(Arguments... arguments)
{
    return DispatcherBase<ReturnType(Arguments...)>::Dispatch<Collector>(std::forward<Arguments>(arguments)...);
}
