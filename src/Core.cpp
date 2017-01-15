#include "Core.h"
#include "utils/Logger.h"
#include "utils/Profiler.h"

bamboo::Core::Core()
{
	m_defaultLogger = std::make_unique<bamboo::Logger>();
	m_defaultLogger->attachSink(bamboo::Logger::StandardOutputSink::create(), Logger::LogLevel::DebugInfo, Logger::LogLevel::Warning);
	m_defaultLogger->attachSink(bamboo::Logger::StandardErrorSink::create(), Logger::LogLevel::Error, Logger::LogLevel::Fatal);
	m_defaultLogger->attachSink(bamboo::Logger::FileSink::create("default.log"));

	m_profiler = std::make_unique<bamboo::Profiler>();
}
