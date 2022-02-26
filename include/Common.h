//
// Created by bytedance on 2021/6/19.
//

#ifndef HJGRAPHICS_COMMON_H
#define HJGRAPHICS_COMMON_H
#include <iostream>
#include <utility>

namespace HJGraphics {
	template<typename T> struct Size{
		T width;
		T height;
		Size()=default;
		Size(T _width,T _height):width(_width),height(_height){}
	};
	typedef Size<int> Sizei;
	typedef Size<unsigned int> Sizeu;
	typedef Size<float> Sizef;

    class GLResource { //todo. remove GLResource, useless
    public:
        GLuint id;
        std::string resourceType;

        GLResource() = default;

        GLResource(GLuint _id, std::string _type) : id(_id), resourceType(std::move(_type)) {}
    };

    class VertexArrayBuffer : public GLResource {
    public:
        VertexArrayBuffer() {
            glGenVertexArrays(1, &id);
        }

        void bind() {
            glBindVertexArray(id);
        }
    };

    class VertexBuffer : public GLResource {
    public:
        VertexBuffer() {
            glGenBuffers(1, &id);
        }

        void bind() const {
            glBindBuffer(GL_ARRAY_BUFFER, id);
        }

        static void writeData(void *data, size_t size, GLenum usage = GL_STATIC_DRAW) {
            glBufferData(GL_ARRAY_BUFFER, size, data, usage);
        }

        void bindWriteData(void *data, size_t size, GLenum usage = GL_STATIC_DRAW) {
            glBindBuffer(GL_ARRAY_BUFFER, id);
            glBufferData(GL_ARRAY_BUFFER, size, data, usage);
        }

        static void unbind() {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    };

    class VertexElementBuffer : public GLResource {
        VertexElementBuffer() {
            glGenBuffers(1, &id);
        }

        void bind() const {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        }

        static void unbind() {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        static void writeData(void *data, size_t size, GLenum usage = GL_STATIC_DRAW) {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
        }

        void bindWriteData(void *data, size_t size, GLenum usage = GL_STATIC_DRAW) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
        }
    };
}
#endif //HJGRAPHICS_COMMON_H
