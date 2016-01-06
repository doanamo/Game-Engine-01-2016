#pragma once

#include "Precompiled.hpp"

//
// Collectors
//
//  Different rules for interpretating results from multiple receiver invocations.
//

// Default collector.
template<typename ReturnType>
class CollectLast;

template<typename Type>
class CollectDefault : public CollectLast<Type>
{
};

// Collector that returns the result of the last receiver invocation.
template<typename ReturnType>
class CollectLast
{
public:
    CollectLast() :
        m_result()
    {
    }

    bool operator()(ReturnType result)
    {
        m_result = result;
        return true;
    }

    ReturnType GetResult() const
    {
        return m_result;
    }

private:
    ReturnType m_result;
};

// Default collector specialized for dealing with void return type.
template<>
class CollectDefault<void>
{
public:
    bool operator()(void)
    {
        return true;
    }

    void GetResult() const
    {
    }
};

// Collector that keeps dispatcher propagating while receivers invocations return true.
template<typename ReturnType = bool>
class CollectWhileTrue
{
public:
    CollectWhileTrue() :
        m_result()
    {
    }

    bool operator()(ReturnType result)
    {
        m_result = result;
        return m_result ? true : false;
    }

    ReturnType GetResult() const
    {
        return m_result;
    }

private:
    ReturnType m_result;
};

// Collector that keeps dispatcher propagating while receivers invocations return false.
template<typename ReturnType = bool>
class CollectWhileFalse
{
public:
    public:
    CollectWhileFalse() :
        m_result()
    {
    }

    bool operator()(ReturnType result)
    {
        m_result = result;
        return m_result ? false : true;
    }

    ReturnType GetResult() const
    {
        return m_result;
    }

private:
    ReturnType m_result;
};
