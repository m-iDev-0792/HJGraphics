//
// Created by 何振邦 on 2020/7/7.
//

#ifndef HJGRAPHICS_TEXTRENDERER_H
#define HJGRAPHICS_TEXTRENDERER_H
#include "Shader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ft2build.h"
#include FT_FREETYPE_H
#include <string>
#include <iostream>
#include <map>
struct Character{
	GLuint     id;
	glm::ivec2 size;
	glm::ivec2 bearing;
	GLuint     advance;
};
namespace HJGraphics{
	class TextRenderer{
	public:
		glm::mat4 projection;
		std::string fontPath;
		int textHeight;
		std::map<char,Character> characters;
		GLuint VAO,VBO;
		static std::shared_ptr<Shader> textShader;
		glm::vec2 size;
		TextRenderer(std::string _font,glm::vec2 _size,int _textHeight);
		void renderTextDynamic(std::string text,glm::vec2 position,glm::vec3 color=glm::vec3(1.0f),float scale=1.0f);
	};
}
#endif //HJGRAPHICS_TEXTRENDERER_H
