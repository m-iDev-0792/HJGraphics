//
// Created by 何振邦 on 2020/7/4.
//

#ifndef HJGRAPHICS_SSAO_H
#define HJGRAPHICS_SSAO_H
#include "FrameBuffer.h"
#include "Quad.h"
#include <vector>
namespace HJGraphics{
	class SSAO {
	public:
		std::shared_ptr<FrameBuffer> ssao;
		std::shared_ptr<FrameBuffer> ssaoBlured;
		glm::vec2 ssaoSize;

		GLuint ssaoNoise;
		std::vector<glm::vec3> noises;
		glm::vec2 ssaoNoiseSize;
		std::vector<glm::vec3> samples;
		float sampleNum;
		float ssaoRadius;
		float ssaoBias;
		int ssaoBlurRaidus;

		static std::shared_ptr<Shader> ssaoShader;
		static std::shared_ptr<Shader> ssaoBlurShader;

		//---------------------------------------
		//     Tips: how to set ssao parameters
		//---------------------------------------
		// ssaoRadius: according to your model size, greater radius , darker ao and larger ao range
		// ssaoBlurRaidus: not too large, 1~3 is enough
		// ssaoNoiseSize: greater size, smoother ao
		// ssaoBias: less biass, darker ao
		SSAO(glm::vec2 _ssaoSize, glm::vec2 _ssaoNoiseSize, float _sampleNum, int _ssaoBlurRadius=1, float _ssaoRadius=0.3,
		     float _ssaoBias=0.01);
		void render(GLuint gPositionDepth, GLuint gNormal, glm::mat4 projectionMat,glm::vec2 zNearAndzFar,glm::vec3 cameraPosition);
		void generateSamplesAndNoise();
		void blur();
	};
}



#endif //HJGRAPHICS_SSAO_H
