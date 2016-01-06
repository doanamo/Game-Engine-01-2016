#pragma once

//
// Standard
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

//
// External
//

// GLM
#include <glm/glm.hpp>

// GLEW
#define GLEW_STATIC
#include <gl/glew.h>

// GLFW
#include <glfw/glfw3.h>

// LuaJIT
#include <lua.hpp>

//
// Project
//

#include "Common/Debug.hpp"
#include "Common/Build.hpp"
#include "Common/Utility.hpp"
#include "Common/ScopeGuard.hpp"
