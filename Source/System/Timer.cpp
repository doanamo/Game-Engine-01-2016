#include "Precompiled.hpp"
#include "Timer.hpp"
using namespace System;

Timer::Timer() :
    m_previousTime(glfwGetTime()),
    m_deltaTime(0.0f),
    m_deltaTimeMax(std::numeric_limits<float>::max())
{
}

Timer::~Timer()
{
}

void Timer::Cleanup()
{
    *this = Timer();
}

void Timer::Reset()
{
    m_previousTime = glfwGetTime();
    m_deltaTime = 0.0f;
}

void Timer::Tick()
{
    // Calculate time delta.
    double currentTime = glfwGetTime();
    double deltaTime = currentTime - m_previousTime;

    // Set and sanitize the delta value.
    m_deltaTime = std::max(0.0f, (float)deltaTime);
    m_deltaTime = std::min(m_deltaTime, m_deltaTimeMax);

    // Set current time.
    m_previousTime = currentTime;
}

void Timer::SetMaxDelta(float value)
{
    m_deltaTimeMax = value;
}

float Timer::GetDelta() const
{
    return m_deltaTime;
}
