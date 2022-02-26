//
// Created by 何振邦 on 2020/7/4.
//

#ifndef HJGRAPHICS_SSAO_H
#define HJGRAPHICS_SSAO_H
#include "FrameBuffer.h"
#include "Quad.h"
#include "Texture.h"
#include <vector>
namespace HJGraphics{
	class SSAO {
	public:
		//---------------------------------------
		//     Tips: how to set ssao parameters
		//---------------------------------------
		// ssaoRadius: according to your model size, greater radius , darker ao and larger ao range
		// ssaoBlurRadius: not too large, 1~3 is enough
		// ssaoNoiseSize: greater size, smoother ao
		// ssaoBias: less bias, darker ao
		SSAO(glm::vec2 _ssaoSize, glm::vec2 _ssaoNoiseSize, float _sampleNum, int _ssaoBlurRadius=1, float _ssaoRadius=0.3,
		     float _ssaoBias=0.01);
		void render(GLuint gNormal,GLuint gDepth, glm::mat4 projectionMat, glm::mat4 inverseProjectionView, glm::vec2 zNearAndzFar, glm::vec3 cameraPosition);
		GLuint getAOTexID(bool enableSSAO) const;
	protected:
		void generateSamplesAndNoise();
		void blur();
	protected:
		std::shared_ptr<FrameBuffer> ssao;
		std::shared_ptr<FrameBuffer> ssaoBlurred;
		glm::vec2 ssaoSize;

		GLuint ssaoNoise;
		std::vector<glm::vec3> noises;
		glm::vec2 ssaoNoiseSize;
		std::vector<glm::vec3> samples;
		float sampleNum;
		float ssaoRadius;
		float ssaoBias;
		int ssaoBlurRadius;

		static std::shared_ptr<Shader> ssaoShader;
		static std::shared_ptr<Shader> ssaoBlurShader;
		static std::shared_ptr<SolidTexture> defaultAOTex;
	};
}



#endif //HJGRAPHICS_SSAO_H
