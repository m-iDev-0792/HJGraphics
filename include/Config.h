//
// Created by 何振邦 on 2022/6/18.
//

#ifndef HJGRAPHICS_CONFIG_H
#define HJGRAPHICS_CONFIG_H
#include "nlohmann/json.hpp"
#include <memory>
namespace HJGraphics{
	class Config{
	public:
		Config(const std::string& _path);
		static bool isOptionEnabled(const std::string& _optionName);
		static std::shared_ptr<Config> globalConfig;
	private:
		nlohmann::json j;
	};
}

#endif //HJGRAPHICS_CONFIG_H
