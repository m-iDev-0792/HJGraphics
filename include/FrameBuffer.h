//
// Created by 何振邦 on 2020/2/3.
//

#ifndef HJGRAPHICS_FRAMEBUFFER_H
#define HJGRAPHICS_FRAMEBUFFER_H
#include "Shader.h"
#include <string>
#include <iostream>
namespace HJGraphics{
    class RenderBuffer: public GLResource{
    public:
        int width;
        int height;
        GLenum internalFormat;
        RenderBuffer(int _width,int _height,GLenum _internalFormat):width(_width),height(_height),internalFormat(_internalFormat){
            glGenRenderbuffers(1, &id);
            glBindRenderbuffer(GL_RENDERBUFFER, id);
            glRenderbufferStorage(GL_RENDERBUFFER, _internalFormat, width, height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }
    };

	enum class FrameBufferAttachmentType{
	    Texture2D,
	    RenderBuffer,
	    OtherTexture
	};
    struct FrameBufferAttachment{
        std::string name;
        GLuint slot;//which slot to bind?
        std::shared_ptr<GLResource> attachment;
        FrameBufferAttachmentType type;
        FrameBufferAttachment()=default;
        FrameBufferAttachment(std::shared_ptr<GLResource> _attachment, GLuint _slot, const std::string& _name, FrameBufferAttachmentType _type=FrameBufferAttachmentType::Texture2D):
        attachment(_attachment),slot(_slot),name(_name),type(_type){
        }
        GLuint getId()const{
            return attachment?attachment->id:0;
        }
    };
    class FrameBufferNew : public GLResource {
    public:
        std::vector<std::shared_ptr<FrameBufferAttachment>> colorAttachments;
        std::shared_ptr<FrameBufferAttachment> depthAttachment;
        std::shared_ptr<FrameBufferAttachment> stencilAttachment;

        int width;
        int height;

        static std::shared_ptr<Shader> defaultShader;

        FrameBufferNew();

        //create a framebuffer constaions one color attachment
        FrameBufferNew(int _width,int _height,int _internalFormat=GL_RGB,int _format=GL_RGB,int _dataType=GL_UNSIGNED_BYTE,int _filter=GL_LINEAR,bool _createDepthRBO=true);

        //more general constructor for creating a framebuffer, resources are pre-allocated
        FrameBufferNew(int _width,int _height,std::vector<std::shared_ptr<FrameBufferAttachment>>& _colors,std::shared_ptr<FrameBufferAttachment> _depth,std::shared_ptr<FrameBufferAttachment> _stencil);

        void clearBind() const;

        void bind() const;

        static void unbind();

        void debugDrawBuffer(int index);

        virtual void bindAttachments();

        virtual void clearAttachments(glm::vec4 clearColor=glm::vec4(0),float depthValue=1.0,int stencilValue=0);

        static void setDrawBuffers(int n){
            std::vector<GLenum> attach(n,0);
            for(int i=0;i<n;++i)attach[i]=GL_COLOR_ATTACHMENT0+i;
            glDrawBuffers(n,&attach[0]);
        }

        void copyDepthBitTo(GLuint target = 0);

        inline bool hasDepthAttachment() const{
            return depthAttachment!= nullptr;
        }
        inline bool hasStencilAttachment() const{
            return stencilAttachment!= nullptr;
        }
        inline bool isDepthStencilShareAttachment() const{
            return depthAttachment==stencilAttachment;
        }
    };

	class DeferredTarget: public FrameBufferNew{
	public:
        std::shared_ptr<FrameBufferAttachment> sharedVelocity;

		DeferredTarget(int _width,int _height):FrameBufferNew(_width,_height,GL_RGBA16F,GL_RGBA,GL_FLOAT){
		}
		DeferredTarget(int _width,int _height, std::shared_ptr<FrameBufferAttachment> _sharedVelocity);
	};
}

#endif //HJGRAPHICS_FRAMEBUFFER_H
