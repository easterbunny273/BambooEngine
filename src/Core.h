#ifndef __BAMBOO_CORE_H_
#define __BAMBOO_CORE_H_

#include <memory>

namespace bamboo
{
	class Logger;
	class Profiler;

	class Core
	{
	public:
		Core();

		Logger& getLogger() const { return *m_defaultLogger; }
		Profiler& getProfiler() const { return *m_profiler; }

	private:
		std::unique_ptr<Logger>		m_defaultLogger;
		std::unique_ptr<Profiler>	m_profiler;
	};
};

#endif