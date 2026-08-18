cmake_minimum_required(VERSION 3.15)
project(MacOpenGL_Tutorial)

# Enforce modern C++17 standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tell CMake to look in our local include folder for header files
include_directories(include)

# Find the GLFW package installed via Homebrew
# We use the REQUIRED flag to stop the build if it is missing
find_package(glfw3 REQUIRED)

# Define our executable name and the source files required to build it
# We MUST include glad.c so the function pointers are compiled into the binary
add_executable(MacEngine src/main1.cpp src/glad.c src/shader.cpp)

# Apple Specific Linking
# We must find and link the core macOS frameworks required for graphics
find_library(COCOA_FRAMEWORK Cocoa)
find_library(OPENGL_FRAMEWORK OpenGL)
find_library(IOKIT_FRAMEWORK IOKit)
find_library(COREVIDEO_FRAMEWORK CoreVideo)

# Bind the executable to all the required libraries and frameworks
target_link_libraries(MacEngine 
    glfw 
    ${COCOA_FRAMEWORK} 
    ${OPENGL_FRAMEWORK} 
    ${IOKIT_FRAMEWORK} 
    ${COREVIDEO_FRAMEWORK}
)
