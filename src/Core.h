#ifndef __BAMBOO_CORE_H_
#define __BAMBOO_CORE_H_

#include "utils/Profiler.h"
#include "utils/Config.h"

#include <spdlog/spdlog.h>

#include <memory>

namespace bamboo
{
	class CoreConfig;

	class Core
	{
	public:
		Core();

		auto getLogger() const			{ return m_defaultLogger; }
		Profiler& getProfiler() const	{ return *m_profiler; }
		CoreConfig& getConfig() const		{ return *m_config; }

	private:
		std::shared_ptr<spdlog::logger>		m_defaultLogger;
		std::unique_ptr<Profiler>			m_profiler;
		std::unique_ptr<CoreConfig>			m_config;
	};
};

#endif