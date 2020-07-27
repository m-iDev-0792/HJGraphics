cmake_minimum_required(VERSION 3.10)
add_definitions(-DUNICODE -D_UNICODE)
project(HJGraphics)
set(CMAKE_CXX_STANDARD 14)
include_directories(3rd-party)
include_directories(include)
aux_source_directory(${PROJECT_SOURCE_DIR}/src HJGRAPHICS_SRC)
#-----------set up 3rd-party lib include-----------
include_directories(3rd-party/freetype2)#set include path for freetype
#-----------set up 3rd-party lib src-----------
set(GLAD_SRC 3rd-party/glad/glad.c)
#-----------set up 3rd-party lib link-----------
#GLFW3.3
IF(WIN32)
    set(GLFW3_LIB glfw3dll.lib)
    set(CMAKE_MSVCIDE_RUN_PATH "${PROJECT_SOURCE_DIR}/lib")#set dll path for win platform
    #from https://stackoverflow.com/questions/28533012/how-to-set-runtime-path-for-cmake-custom-command-on-windows
ELSEIF(APPLE)
    set(GLFW3_LIB libglfw.3.3.dylib)
ENDIF()
#assimp5.0.1
IF(WIN32)
    set(ASSIMP_LIB assimp-vc141-mt.lib)
ELSEIF(APPLE)
    set(ASSIMP_LIB libassimp.dylib)
ENDIF()
#freetype2
IF(WIN32)
    set(FREETYPE_LIB freetype.lib)
ELSEIF(APPLE)
    set(FREETYPE_LIB libfreetype.dylib)
ENDIF()
link_directories(lib)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#-----------set up rpath-----------
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
SET(CMAKE_INSTALL_RPATH "@loader_path/../lib")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_executable(HJGraphics main.cpp ${HJGRAPHICS_SRC} ${GLAD_SRC})
add_executable(NanoSuitDemo nanoSuitDemo.cpp ${HJGRAPHICS_SRC} ${GLAD_SRC})
add_executable(PBRDemo PBRDemo.cpp ${HJGRAPHICS_SRC} ${GLAD_SRC})

target_link_libraries(HJGraphics ${GLFW3_LIB}  ${ASSIMP_LIB} ${FREETYPE_LIB})
target_link_libraries(NanoSuitDemo ${GLFW3_LIB}  ${ASSIMP_LIB} ${FREETYPE_LIB})
target_link_libraries(PBRDemo ${GLFW3_LIB}  ${ASSIMP_LIB} ${FREETYPE_LIB})