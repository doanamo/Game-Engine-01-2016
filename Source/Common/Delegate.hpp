#pragma once

#include "Precompiled.hpp"

//
// Delegate
//
//  Binds an instance to a function which can be invoked at a later time.
//  Be careful not to invoke a delagate to a method of an instance that no longer exists.
//
//  Binding and invoking a function:
//      bool Function(const char* c, int i) { /*...*/ }
//      Delegate<bool(const char*, int)> delegate;
//      delegate.Bind<&Function>();
//      delegate.Invoke("hello", 5);
//
//  Binding and invoking a functor:
//      auto Object = [](const char* c, int i) { /*...*/ };
//      Delegate<bool(const char*, int)> delegate;
//      delegate.Bind(&Object);
//      delegate.Invoke("hello", 5);
//
//  Binding and invoking a method:
//      bool Class::Function(const char* c, int i) { /*...*/ }
//      Class instance;
//      Delegate<bool(const char*, int)> delegate;
//      delegate.Bind<Class, &Class::Function>(&instance);
//      delegate.Invoke("hello", 5);
//

template<typename Type>
class Delegate;

template<typename ReturnType, typename... Arguments>
class Delegate<ReturnType(Arguments...)>
{
private:
    // Type declarations.
    typedef void* InstancePtr;
    typedef ReturnType (*FunctionPtr)(InstancePtr, Arguments...);

    // Compile time invocation stubs.
    template<ReturnType (*Function)(Arguments...)>
    static ReturnType FunctionStub(InstancePtr instance, Arguments... arguments)
    {
        return (Function)(std::forward<Arguments>(arguments)...);
    }

    template<class InstanceType>
    static ReturnType FunctorStub(InstancePtr instance, Arguments... arguments)
    {
        return (*static_cast<InstanceType*>(instance))(std::forward<Arguments>(arguments)...);
    }

    template<class InstanceType, ReturnType (InstanceType::*Function)(Arguments...)>
    static ReturnType MethodStub(InstancePtr instance, Arguments... arguments)
    {
        return (static_cast<InstanceType*>(instance)->*Function)(std::forward<Arguments>(arguments)...);
    }

public:
    Delegate() :
        m_instance(nullptr),
        m_function(nullptr)
    {
    }

    void Cleanup()
    {
        m_instance = nullptr;
        m_function = nullptr;
    }

    // Binds a static function.
    template<ReturnType (*Function)(Arguments...)>
    void Bind()
    {
        m_instance = nullptr;
        m_function = &FunctionStub<Function>;
    }

    // Binds a functor object.
    template<class InstanceType>
    void Bind(InstanceType* instance)
    {
        m_instance = instance;
        m_function = &FunctorStub<InstanceType>;
    }

    // Binds an instance method.
    template<class InstanceType, ReturnType (InstanceType::*Function)(Arguments...)>
    void Bind(InstanceType* instance)
    {
        m_instance = instance;
        m_function = &MethodStub<InstanceType, Function>;
    }

    // Invokes the delegate.
    ReturnType Invoke(Arguments... arguments)
    {
        if(m_function == nullptr)
            return ReturnType();

        return m_function(m_instance, std::forward<Arguments>(arguments)...);
    }

private:
    InstancePtr m_instance;
    FunctionPtr m_function;
};

// Implementation based on: http://molecularmusings.wordpress.com/2011/09/19/generic-type-safe-delegates-and-events-in-c/
