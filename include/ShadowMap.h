﻿//
// Created by 何振邦(m_iDev_0792) on 2020/2/22.
//

#ifndef HJGRAPHICS_SHADOWMAP_H
#define HJGRAPHICS_SHADOWMAP_H
#include "FrameBuffer.h"
namespace HJGraphics {

    class ShadowMap : public FrameBuffer{
    public:
        ShadowMap();
        ShadowMap(int _width, int _height);
    };
    class ShadowCubeMap: public FrameBuffer {
    public:
        ShadowCubeMap();
        ShadowCubeMap(int _width, int _height=0);
        void bindAttachments() override;
    };
}

#endif
