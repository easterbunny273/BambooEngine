#include "Core.h"

#include "utils/Logger.h"
#include "utils/Profiler.h"
#include "utils/Config.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/dist_sink.h"

bamboo::Core::Core()
{
	m_config = std::make_unique<bamboo::CoreConfig>("config.ini");
	m_profiler = std::make_unique<bamboo::Profiler>();

	std::vector<spdlog::sink_ptr> sinks;
	sinks.push_back(std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>());
	sinks.push_back(std::make_shared<spdlog::sinks::simple_file_sink_mt>("engine.log"));
	m_defaultLogger = std::make_shared<spdlog::logger>("main", std::begin(sinks), std::end(sinks));
}
