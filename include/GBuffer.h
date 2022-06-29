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

    class GBuffer : public FrameBuffer{
    public:
        GBuffer(int _width, int _height);

        virtual void bindTexturesForShading() const;

        virtual void writeUniform(const std::shared_ptr<Shader>& _shader)const;

        GLint getId(const std::string& _name)const;
    public:
	    std::shared_ptr<Shader> shader;
    };
}
#endif
