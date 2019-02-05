cmake_minimum_required(VERSION 3.10)
project(HJGraphics)
include_directories("/usr/local/include")
find_package(OpenGL REQUIRED)


link_directories(${OpenGL_LIBRARY_DIRS})
include_directories(${OpenGL_INCLUDE_DIR})

link_directories(${GLFW_LIBRARY_DIRS})
include_directories(${GLFW_INCLUDE_DIR})
set(GLFW3_LIB "/usr/local/lib/libglfw.3.2.dylib")
set(ASSIMP_LIB "/usr/local/Cellar/assimp/4.0.1/lib/libassimp.4.0.1.dylib")
set(CMAKE_CXX_STANDARD 11)
add_executable(HJGraphics main.cpp GLFWWindowObject.cpp BasicGLObject.cpp Shader.cpp ElementObjects.cpp Scene.cpp Camera.cpp Light.cpp Material.cpp Model.cpp DebugUtility.cpp Model.cpp Model.h)
target_link_libraries(HJGraphics ${GLFW3_LIB} ${OPENGL_LIBRARY} ${ASSIMP_LIB})