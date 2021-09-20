cmake_minimum_required(VERSION 3.10)
add_definitions(-DUNICODE -D_UNICODE)
project(HJGraphics)
set(CMAKE_CXX_STANDARD 14)
#-----------include directories---------
include_directories(include)
include_directories(3rd-party)
include_directories(3rd-party/freetype2)#set include path for freetype

aux_source_directory(${PROJECT_SOURCE_DIR}/src HJGRAPHICS_SRC)
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
IF(WIN32)
    link_directories(lib/win64)
ELSEIF(APPLE)
    link_directories(lib/mac)
ENDIF()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#-----------set up rpath-----------
IF(APPLE)
    SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    SET(CMAKE_INSTALL_RPATH "@loader_path/../lib/mac")
    SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
ENDIF()

add_executable(HJGraphics main.cpp ${HJGRAPHICS_SRC} ${GLAD_SRC})
add_executable(NanoSuitDemo nanoSuitDemo.cpp ${HJGRAPHICS_SRC} ${GLAD_SRC})
add_executable(PBRDemo PBRDemo.cpp ${HJGRAPHICS_SRC} ${GLAD_SRC})

target_link_libraries(HJGraphics ${GLFW3_LIB}  ${ASSIMP_LIB} ${FREETYPE_LIB})
target_link_libraries(NanoSuitDemo ${GLFW3_LIB}  ${ASSIMP_LIB} ${FREETYPE_LIB})
target_link_libraries(PBRDemo ${GLFW3_LIB}  ${ASSIMP_LIB} ${FREETYPE_LIB} ${SPDLOG_LIB})

IF(WIN32)
    add_custom_command(TARGET PBRDemo
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