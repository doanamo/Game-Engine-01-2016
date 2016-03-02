#pragma once

//
// Standard
//

#include <cctype>
#include <typeindex>
#include <memory>
#include <numeric>
#include <algorithm>
#include <functional>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <unordered_map>

//
// External
//

// Windows
#ifdef WIN32
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <windows.h>
#endif

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>

// GLEW
#define GLEW_STATIC
#include <gl/glew.h>

// GLFW
#include <glfw/glfw3.h>

// LuaJIT
#include <lua.hpp>

// ZLib
#include <zlib.h>

// PNG
#include <png.h>

//
// Project
//

#include "Common/Debug.hpp"
#include "Common/Build.hpp"
#include "Common/Utility.hpp"
#include "Common/NonCopyable.hpp"
#include "Common/ScopeGuard.hpp"
#include "Common/Delegate.hpp"
#include "Common/Dispatcher.hpp"
#include "Common/Receiver.hpp"
#include "Common/Context.hpp"
#include "Logger/Logger.hpp"
