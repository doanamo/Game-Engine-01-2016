# Minimum version required.
CMake_Minimum_Required(VERSION 3.1)

# Create project.
Project(GLEW)

# Define include directory.
Include_Directories("${CMAKE_CURRENT_SOURCE_DIR}/include/")

# Add preprocessor definitions.
Add_Definitions(-DGLEW_BUILD)
Add_Definitions(-DGLEW_STATIC)
Add_Definitions(-DGLEW_NO_GLU)

# Create library target.
Add_Library(GLEW STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/glew.c"
)

# Find OpenGL library.
Find_Package(OpenGL REQUIRED)

# Link OpenGL library.
Target_Link_Libraries(GLEW ${OPENGL_gl_LIBRARY})
