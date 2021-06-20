//
// Created by 何振邦(m_iDev_0792) on 2020/2/19.
//

#ifndef HJGRAPHICS_GBUFFER_H
#define HJGRAPHICS_GBUFFER_H

#include <memory>
#include "OpenGLHeader.h"
#include "Shader.h"
#include "FrameBuffer.h"
namespace HJGraphics {
	class DeferredRenderer;

    class GBufferNew : public FrameBufferNew{
        friend DeferredRenderer;
    protected:
        std::shared_ptr<Shader> shader;
    public:
        GBufferNew(int _width, int _height);

        virtual void bindTextures() const;

        virtual void writeUniform(std::shared_ptr<Shader> shader)const;
    };
}
#endif
