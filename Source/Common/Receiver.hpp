#pragma once

#include "Precompiled.hpp"
#include "Dispatcher.hpp"
#include "Delegate.hpp"

// Forward declarations.
template<typename Type>
class DispatcherBase;

template<typename Type>
class ReceiverInvoker;

//
// Receiver
//
//  Invokes a function/method after receiving a signal from a dispatcher.
//  A single receiver instance can be connected to only one dispatcher.
//  See dispatcher header for more information.
//

template<typename Type>
class Receiver;

template<typename ReturnType, typename... Arguments>
class Receiver<ReturnType(Arguments...)> : private Delegate<ReturnType(Arguments...)>
{
public:
    // Friend declarations.
    friend DispatcherBase<ReturnType(Arguments...)>;
    friend ReceiverInvoker<ReturnType(Arguments...)>;

public:
    Receiver() :
        m_dispatcher(nullptr),
        m_next(nullptr)
    {
    }

    virtual ~Receiver()
    {
        Cleanup();
    }

    // Restores instance to it's original state.
    void Cleanup()
    {
        // Unsubscribe from the dispatcher.
        this->Unsubscribe();

        // Cleanup base class.
        Delegate<ReturnType(Arguments...)>::Cleanup();
    }

    // Unsubscribes from the current dispatcher.
    void Unsubscribe()
    {
        // Unsubscribes from the dispatcher.
        if(m_dispatcher != nullptr)
        {
            m_dispatcher->Unsubscribe(*this);
            Assert(m_dispatcher == nullptr);
            Assert(m_next == nullptr);
        }
    }

    // Binds a static function.
    template<ReturnType (*Function)(Arguments...)>
    void Bind()
    {
        Delegate<ReturnType(Arguments...)>::Bind<Function>();
    }

    // Binds a functor object.
    template<class InstanceType>
    void Bind(InstanceType* instance)
    {
        Delegate<ReturnType(Arguments...)>::Bind(instance);
    }

    // Binds an instance method.
    template<class InstanceType, ReturnType (InstanceType::*Function)(Arguments...)>
    void Bind(InstanceType* instance)
    {
        Delegate<ReturnType(Arguments...)>::Bind<InstanceType, Function>(instance);
    }

private:
    // Receives an event and invokes a bound function.
    ReturnType Receive(Arguments... arguments)
    {
        // Call the bound delegate function.
        return this->Invoke(std::forward<Arguments>(arguments)...);
    }

private:
    // Intrusive singly linked list.
    DispatcherBase<ReturnType(Arguments...)>* m_dispatcher;
    Receiver<ReturnType(Arguments...)>*       m_next;
};
