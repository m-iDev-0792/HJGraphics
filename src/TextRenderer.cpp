//
// Created by 何振邦 on 2020/7/7.
//

#include "TextRenderer.h"
std::shared_ptr<HJGraphics::Shader> HJGraphics::TextRenderer::textShader=nullptr;
HJGraphics::TextRenderer::TextRenderer(std::string _font,glm::vec2 _size,int _textHeight) {
	if(textShader==nullptr)textShader=std::make_shared<Shader>(ShaderCodeList{"../shader/text/text.vs.glsl"_vs, "../shader/text/text.fs.glsl"_fs});
	size=_size;
	fontPath=_font;
	textHeight=_textHeight;
	projection=glm::ortho(0.0f, size.x, 0.0f, size.y);//NOTE the origin is on the lower left corner

	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cerr << "ERROR @ TextRenderer: Could not init FreeType Library" << std::endl;

	FT_Face face;
	if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
		std::cerr << "ERROR @ TextRenderer: Failed to load font" << std::endl;

	//here width=0 means we want to fix height and get dynamic width
	FT_Set_Pixel_Sizes(face, 0, textHeight);

	//init characters
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //禁用字节对齐限制
	for (GLubyte c = 0; c < 128; c++){
		//load characters
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)){
			std::cout << "ERROR @ TextRenderer: Failed to load Glyph" << std::endl;
			continue;
		}

		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RED,face->glyph->bitmap.width,face->glyph->bitmap.rows,0,
				GL_RED,GL_UNSIGNED_BYTE,face->glyph->bitmap.buffer);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Character character{texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				static_cast<GLuint>(face->glyph->advance.x)
		};
		characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4); //restore default setting
	//set up VAO and VBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	//clean up
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}
void HJGraphics::TextRenderer::renderTextDynamic(std::string text, glm::vec2 position, glm::vec3 color, float scale) {
	//NOTE the origin is on the lower left corner
	textShader->use();
	textShader->set4fm("projection",projection);
	textShader->set3fv("textColor",color);
	textShader->setInt("text",0);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	float x=position.x;
	float y=position.y;
	//see https://learnopengl-cn.github.io/img/06/02/glyph_offset.png to figure out how to calculate position
	for (auto c = text.begin(); c != text.end(); c++){
		Character ch = characters[*c];

		GLfloat xpos = x + ch.bearing.x * scale;
		GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

		GLfloat w = ch.size.x * scale;
		GLfloat h = ch.size.y * scale;

		GLfloat vertices[6][4] = {
				{ xpos,     ypos + h,   0.0, 0.0 },
				{ xpos,     ypos,       0.0, 1.0 },
				{ xpos + w, ypos,       1.0, 1.0 },

				{ xpos,     ypos + h,   0.0, 0.0 },
				{ xpos + w, ypos,       1.0, 1.0 },
				{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		glBindTexture(GL_TEXTURE_2D, ch.id);
		//update VBO
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		//update x
		x += (ch.advance >> 6) * scale; // 位偏移6个单位来获取单位为像素的值 (2^6 = 64)
	}
//	glBindVertexArray(0);
//	glBindTexture(GL_TEXTURE_2D, 0);
}