//
// Created by 何振邦 on 2022/6/18.
//

#include "Config.h"
#include <fstream>
std::shared_ptr<HJGraphics::Config> HJGraphics::Config::globalConfig=std::make_shared<Config>("../config/config.json");
HJGraphics::Config::Config(const std::string &_path) {
	if(!_path.empty()){
		std::ifstream in(_path);
		if(in.is_open()) in >> j;
	}
}

bool HJGraphics::Config::isOptionEnabled(const std::string &_optionName){
	if(globalConfig){
		auto iter=globalConfig->j.find(_optionName);
		if(iter!=globalConfig->j.end()){
			if(iter->is_boolean())return iter->get<bool>();
		}
	}
	return false;
}
