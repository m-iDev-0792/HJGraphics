cmake_minimum_required(VERSION 3.15)
add_definitions(-DUNICODE -D_UNICODE)
project(HJGraphics)
set(CMAKE_CXX_STANDARD 14)
#-----------include directories---------
include_directories(include)
include_directories(3rd-party)
include_directories(3rd-party/freetype2)#set include path for freetype
set(HJGRAPHICS_INCLUDE include 3rd-party 3rd-party/freetype2)

aux_source_directory(${PROJECT_SOURCE_DIR}/src HJGRAPHICS_SRC)
#aux_source_directory(${PROJECT_SOURCE_DIR}/src/ECS HJGRAPHICS_SRC) #todo. not ready yet

#-----------set up 3rd-party lib src-----------
set(GLAD_SRC 3rd-party/glad/glad.c)
#-----------set up 3rd-party lib link-----------
IF(WIN32)
    set(GLFW3_LIB glfw3dll.lib)
    set(CMAKE_MSVCIDE_RUN_PATH "${PROJECT_SOURCE_DIR}/lib/win64")#set dll path for win platform,but not working for now
    #from https://stackoverflow.com/questions/28533012/how-to-set-runtime-path-for-cmake-custom-command-on-windows
    set(ASSIMP_LIB assimp-vc141-mt.lib)
    set(FREETYPE_LIB freetype.lib)
    set(SPDLOG_LIB)
ELSEIF(APPLE)
    set(GLFW3_LIB libglfw.dylib)
    set(ASSIMP_LIB libassimp.dylib)
    set(FREETYPE_LIB libfreetype.dylib)
    set(SPDLOG_LIB spdlog.a)
    add_definitions(-DSPDLOG_COMPILED_LIB)#use pre-built static spdlog lib to speed up compilation on MacOS
ENDIF()
#----------lib directories---------
link_directories(bin)
IF(WIN32)
    link_directories(lib/win64)
ELSEIF(APPLE)
    link_directories(lib/mac)
ENDIF()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#-----------set up rpath-----------
IF(APPLE)
    SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    SET(CMAKE_INSTALL_RPATH "@loader_path/../lib/mac"  "@loader_path/")
    SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
ENDIF()

#Set up HJGraphics lib
add_library(HJGraphics SHARED ${HJGRAPHICS_SRC} ${GLAD_SRC})
target_link_libraries(HJGraphics ${GLFW3_LIB}  ${ASSIMP_LIB} ${FREETYPE_LIB} ${SPDLOG_LIB})
target_include_directories(HJGraphics PUBLIC ${HJGRAPHICS_INCLUDE})

#Set up example targets
add_subdirectory(example)
target_link_libraries(PBRDemo HJGraphics)

IF(WIN32)
    add_custom_command(TARGET HJGraphics
            POST_BUILD
            COMMENT  "copy dll to bin folder"
            COMMAND ${CMAKE_COMMAND} -E
            copy_if_different
            #src
            ${PROJECT_SOURCE_DIR}/lib/win64/freetype.dll
            ${PROJECT_SOURCE_DIR}/lib/win64/glfw3.dll
            ${PROJECT_SOURCE_DIR}/lib/win64/assimp-vc141-mt.dll
            #dst
            ${PROJECT_SOURCE_DIR}/bin
            )
ENDIF()