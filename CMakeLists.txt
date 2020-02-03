cmake_minimum_required(VERSION 3.10)
project(HJGraphics)
set(CMAKE_CXX_STANDARD 14)
include_directories(3rd-party)
include_directories(include)
aux_source_directory(${PROJECT_SOURCE_DIR}/src HJGRAPHICS_SRC)
#-----------set up 3rd-party lib src-----------
set(GLAD_SRC 3rd-party/glad/glad.c)
#-----------set up 3rd-party lib link-----------
#GLFW3.3
IF(WIN32)
    set(GLFW3_LIB glfw3dll.lib)
ELSEIF(APPLE)
    set(GLFW3_LIB libglfw.3.3.dylib)
ENDIF()
#assimp5.0.1
IF(WIN32)
    set(ASSIMP_LIB)
ELSEIF(APPLE)
    set(ASSIMP_LIB libassimp.dylib)
ENDIF()
link_directories(lib)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#-----------set up rpath-----------
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
SET(CMAKE_INSTALL_RPATH "@loader_path/../lib")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
add_executable(HJGraphics main.cpp ${HJGRAPHICS_SRC} ${GLAD_SRC})
target_link_libraries(HJGraphics ${GLFW3_LIB}  ${ASSIMP_LIB})