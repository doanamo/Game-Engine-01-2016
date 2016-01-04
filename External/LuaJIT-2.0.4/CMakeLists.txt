# Minimum version required.
CMake_Minimum_Required(VERSION 3.1)

# Create the project.
Project(LuaJIT)

# Define include directories.
Include_Directories("${CMAKE_CURRENT_BINARY_DIR}")
Include_Directories("${CMAKE_CURRENT_SOURCE_DIR}/src/")

# Disable Visual C++ warnings.
If("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  Add_Definitions(-D_CRT_SECURE_NO_WARNINGS)
EndIf()

# Add MiniLua target.
Add_Executable(LuaJIT-MiniLua "src/host/minilua.c")

# Generate the BuildVM header.
Set(DynASM_File "${CMAKE_CURRENT_SOURCE_DIR}/dynasm/dynasm.lua")
Set(DynASM_Output "${CMAKE_CURRENT_BINARY_DIR}/buildvm_arch.h")
Set(DynASM_Flags -D JIT -D FFI)

If(CMAKE_SIZEOF_VOID_P EQUAL 8)
    List(APPEND DynASM_Flags -D P64)
EndIf()

If(WIN32)
    List(APPEND DynASM_Flags -D WIN)
EndIf()

Add_Custom_Command(
    OUTPUT ${DynASM_Output}
    COMMAND LuaJIT-MiniLua ${DynASM_File} -LN ${DynASM_Flags} -o ${DynASM_Output} "${CMAKE_CURRENT_SOURCE_DIR}/src/vm_x86.dasc"
    DEPENDS ${DynASM_File} LuaJIT-MiniLua
)

# Add BuildVM target.
File(GLOB BuildVM_Files "src/host/buildvm*.c")

Add_Executable(LuaJIT-BuildVM ${BuildVM_Files} ${DynASM_Output})

# List Lua library files.
Set(LuaLib_Files
    "src/lib_base.c"
    "src/lib_math.c"
    "src/lib_bit.c"
    "src/lib_string.c"
    "src/lib_table.c"
    "src/lib_io.c"
    "src/lib_os.c"
    "src/lib_package.c"
    "src/lib_debug.c"
    "src/lib_jit.c"
    "src/lib_ffi.c"
)

# Generate additional dependencies using BuildVM.
Macro(Add_BuildVM_Target _Mode _Target)
    Add_Custom_Command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${_Target}"
        COMMAND LuaJIT-BuildVM -m ${_Mode} -o "${CMAKE_CURRENT_BINARY_DIR}/${_Target}" ${ARGN}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS LuaJIT-BuildVM ${ARGN}
    )
EndMacro()

If(WIN32 AND NOT CYGIWN)
    Add_BuildVM_Target("peobj" "lj_vm.obj")
    Set(BuildVM_ObjFile "lj_vm.obj")
ElseIf(APPLE)
    Add_BuildVM_Target("machasm" "lj_vm.s")
    Set(BuildVM_ObjFile "lj_vm.s")
ElseIf(UNIX)
    Add_BuildVM_Target("elfasm" "lj_vm.s")
    Set(BuildVM_ObjFile "lj_vm.s")
Else()
    Message(FATAL_ERROR "Unknown operating system.")
EndIf()

Add_BuildVM_Target("bcdef" "lj_bcdef.h" ${LuaLib_Files})
Add_BuildVM_Target("ffdef" "lj_ffdef.h" ${LuaLib_Files})
Add_BuildVM_Target("libdef" "lj_libdef.h" ${LuaLib_Files})
Add_BuildVM_Target("recdef" "lj_recdef.h" ${LuaLib_Files})
Add_BuildVM_Target("vmdef" "jit/vmdef.lua" ${LuaLib_Files})
Add_BuildVM_Target("folddef" "lj_folddef.h" "src/lj_opt_fold.c")

Set(LuaJIT_Dependencies
    "${CMAKE_CURRENT_BINARY_DIR}/${BuildVM_ObjFile}"
    "${CMAKE_CURRENT_BINARY_DIR}/lj_bcdef.h"
    "${CMAKE_CURRENT_BINARY_DIR}/lj_ffdef.h"
    "${CMAKE_CURRENT_BINARY_DIR}/lj_libdef.h"
    "${CMAKE_CURRENT_BINARY_DIR}/lj_recdef.h"
    "${CMAKE_CURRENT_BINARY_DIR}/lj_folddef.h"
)

# Add LuaJIT target.
File(GLOB LuaJIT_Files "src/lj_*.c" "src/lib_*.c")

Add_Library(LuaJIT STATIC ${LuaJIT_Files} ${LuaJIT_Dependencies})
