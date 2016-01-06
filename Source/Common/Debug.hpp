#pragma once

// Windows specific defines.
#if defined(WIN32) && !defined(NDEBUG)
    #define _CRTDBG_MAP_ALLOC
    #define _CRTDBG_MAP_ALLOC_NEW
    #include <stdlib.h>
    #include <crtdbg.h>

    #define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
    #define new DEBUG_NEW
#endif

//
// Debug
//

namespace Debug
{
    // Initializes debug helpers.
    inline void Initialize()
    {
        // Don't do anything if not in debug mode.
        #ifdef NDEBUG
            return;
        #endif

        // Enable memory leak detection.
        #if defined(WIN32)
            _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
        #endif
    }
}
