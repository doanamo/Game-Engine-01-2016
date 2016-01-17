#pragma once

//
// Timer
//
//  Tracks time and calculates the delta between frames.
//
//  Example usage:
//      System::Timer timer;
//      timer.SetMaxDelta(1.0f / 10.0f);
//      
//      while(true)
//      {
//          float dt = timer.GetDelta();
//          
//          /* ... */
//          
//          timer.tick();
//      }
//
//  Todo:
//  - Implement frame rate calculation.
//

namespace System
{
    // Timer class.
    class Timer
    {
    public:
        Timer();
        ~Timer();

        // Restores instance to it's original state.
        void Cleanup();

        // Resets the timer.
        void Reset();

        // Advances the timer forward.
        // Calculates the frame's delta time.
        void Tick();

        // Sets the maximum delta value. 
        void SetMaxDelta(float value);

        // Gets the frame's delta time.
        float GetDelta() const;

    private:
        // Time book keeping.
        double m_previousTime;

        // Frame delta value.
        float m_deltaTime;
        float m_deltaTimeMax;
    };
}
