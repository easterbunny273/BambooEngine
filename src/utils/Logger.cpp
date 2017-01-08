#include "Logger.h"

#include <chrono>
#include <iomanip>
#include <sstream>
#include <time.h>
#include <cassert>

using namespace bamboo;

std::unique_ptr<Logger::FileSink> Logger::FileSink::create(std::string filename, OpenMode openMode /* = OpenMode::Clear*/)
{
	std::ofstream outfile(filename, openMode == Logger::FileSink::OpenMode::Clear ? std::ofstream::trunc : std::ofstream::app);

	if (outfile.is_open() == false || outfile.fail())
		return nullptr;

	return std::unique_ptr<Logger::FileSink>(new FileSink(std::move(outfile)));
}

void bamboo::Logger::attachSink(std::unique_ptr<ISink> &&sink, LogLevel lowerThreshold /*= LogLevel::DebugInfo*/, LogLevel upperThreshold /*= LogLevel::Fatal*/)
{
	std::lock_guard<std::mutex> guard(m_modifySinksMutex);

	assert(lowerThreshold <= upperThreshold);

	m_sinks.emplace_back(std::move(sink), lowerThreshold, upperThreshold);
}

void bamboo::Logger::logMessage(LogLevel level, const std::string &message)
{
	std::lock_guard<std::mutex> guard(m_modifySinksMutex);

	for (auto &sinkInfo : m_sinks)
	{
		if (level < sinkInfo.lowerThreshold ||
			level > sinkInfo.upperThreshold)
			continue;

		std::lock_guard<std::mutex> guardLogging(m_logMutex);
		sinkInfo.sink->getStream() << "[" << getCurrentTimeAsString() << ", " << getLogLevelAsString(level) << "] " <<  message << "\n";
	}
}

void bamboo::Logger::flush()
{
	std::lock_guard<std::mutex> guard(m_modifySinksMutex);

	for (auto &sinkInfo : m_sinks)
		sinkInfo.sink->getStream() << std::flush;
}

std::string bamboo::Logger::getCurrentTimeAsString()
{
	const std::time_t now = std::time(nullptr);

	struct tm buf;
	localtime_s(&buf, &now);

	std::stringstream buffer;
	buffer << std::put_time(&buf, "%T");

	return buffer.str();
}

const char * bamboo::Logger::getLogLevelAsString(LogLevel level)
{
	switch (level)
	{
	case LogLevel::DebugInfo:	return "Debug  ";
	case LogLevel::Info:		return "Info   ";
	case LogLevel::Warning:		return "Warning";
	case LogLevel::Error:		return "Error  ";
	case LogLevel::Fatal:		return "Fatal  ";
	default:					break;
	}

	assert(!"should not reach this point");
	return "undefined";
}

std::ostream& bamboo::Logger::FileSink::getStream()
{
	return m_file;
}

bamboo::Logger::FileSink::~FileSink()
{
	m_file.close();
	assert(m_file.fail() == false);
}

bamboo::Logger::FileSink::FileSink(std::ofstream &&ofstream)
	: m_file(std::move(ofstream))
{
	assert(m_file.is_open());
}

std::unique_ptr<bamboo::Logger::StandardOutputSink> bamboo::Logger::StandardOutputSink::create()
{
	return std::make_unique<Logger::StandardOutputSink>();
}

std::ostream& bamboo::Logger::StandardOutputSink::getStream()
{
	return std::cout;
}

std::unique_ptr<bamboo::Logger::StandardErrorSink> bamboo::Logger::StandardErrorSink::create()
{
	return std::make_unique<Logger::StandardErrorSink>();
}

std::ostream& bamboo::Logger::StandardErrorSink::getStream()
{
	return std::cerr;
}
