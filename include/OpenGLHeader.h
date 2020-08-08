//
// Created by 何振邦 on 2019-03-11.
//
//NOTE: this header file handles how we access OpenGL API
//to access OpenGL API via GLAD, please #define HJGRAPHICS_GLAD
//to access OpenGL API via Qt SDK, please #define HJGRAPHICS_QT

#ifndef HJGRAPHICS_OPENGLHEADER_H
#define HJGRAPHICS_OPENGLHEADER_H
//in which way do you want to access OpenGL API?
#define HJGRAPHICS_GLAD

#ifdef HJGRAPHICS_GLAD
#include "OpenGLCache.h"
#endif

#ifdef HJGRAPHICS_QT
#include <QOpenGLFunctions_3_3_Core>
#include <QDebug>
using QtOpenGLBaseClass = QOpenGLFunctions_3_3_Core;
#endif

#endif //HJGRAPHICS_OPENGLHEADER_H
