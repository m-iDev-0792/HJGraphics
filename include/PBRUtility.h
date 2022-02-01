//
// Created by 何振邦 on 2022/1/25.
//

#ifndef HJGRAPHICS_PBRUTILITY_H
#define HJGRAPHICS_PBRUTILITY_H

#include "Texture.h"
#include "Shader.h"
#include "Quad.h"
namespace HJGraphics{
	void texture2DToCubeMap(Texture2D *tex2D, CubeMapTexture *cubemap);
	void generateDiffuseIrradianceMap(CubeMapTexture *inCubeMap, CubeMapTexture *outDiffuseIrradianceMap,
	                                  float sampleDelta);
	void generateSpecularPrefilteredMap(CubeMapTexture *inCubeMap, CubeMapTexture *outSpecularPrefiltered,
	                                    unsigned int sampleNum);
	struct IBLManager{
		std::shared_ptr<Texture2D> environmentTex;
		std::shared_ptr<CubeMapTexture> environmentCubeMap;
		std::shared_ptr<CubeMapTexture> diffuseIrradiance;
		std::shared_ptr<CubeMapTexture> specularPrefiltered;
		std::shared_ptr<Texture2D> brdfLUTMap;
		static std::shared_ptr<HJGraphics::IBLManager>
		bakeIBLMap(std::shared_ptr<Texture2D> _environmentTex, Sizei enviCubeMapSize,
		           Sizei irradianceSize, Sizei prefilteredSize, float irradianceSampleDelta,
		           int prefilteredSampleNum);
	};
}


#endif //HJGRAPHICS_PBRUTILITY_H
