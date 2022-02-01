//
// Created by 何振邦 on 2021/9/20.
//

#ifndef HJGRAPHICS_LOG_H
#define HJGRAPHICS_LOG_H
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#define INIT_HJGRAPHICS_LOG \
	std::vector<spdlog::sink_ptr> sinks;\
	sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_st>());\
	sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_st>("log.txt",true));\
	auto HJGLogger = std::make_shared<spdlog::logger>("HJG", begin(sinks), end(sinks));\
	HJGLogger->sinks()[0]->set_pattern("[%s:%#@%!]%^[%l]%$ %v");\
	HJGLogger->sinks()[1]->set_pattern("[%H:%M:%S][%s:%#@%!][%l] %v");\
	spdlog::set_default_logger(HJGLogger);\
	spdlog::set_level(spdlog::level::debug);
#endif //HJGRAPHICS_LOG_H
