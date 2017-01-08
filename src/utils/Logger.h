#pragma once

#ifndef __BAMBOOENGINE_LOGGER_H_
#define __BAMBOOENGINE_LOGGER_H_

#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <mutex>

namespace bamboo
{
	class Logger
	{
	public:
		class ISink
		{
		public:
			virtual std::ostream& getStream() = 0;
			virtual ~ISink() {};
		};

		class FileSink : public ISink
		{
		public:
			enum class OpenMode
			{
				Clear,
				Append
			};

			static std::unique_ptr<FileSink> create(std::string filename, OpenMode openMode = OpenMode::Clear);
			std::ostream& getStream() override;
			virtual ~FileSink();

		private:
			FileSink(std::ofstream &&ofstream);
			std::ofstream m_file;
		};

		class StandardOutputSink : public ISink
		{
		public:
			static std::unique_ptr<StandardOutputSink> create();
			std::ostream& getStream() override;
		};

		class StandardErrorSink : public ISink
		{
		public:
			static std::unique_ptr<StandardErrorSink> create();
			std::ostream& getStream() override;
		};

		enum class LogLevel
		{
			DebugInfo,
			Info,
			Warning,
			Error,
			Fatal,
		};

		void attachSink(std::unique_ptr<ISink> &&sink, LogLevel lowerThreshold = LogLevel::DebugInfo, LogLevel upperThreshold = LogLevel::Fatal);

		void logMessage(LogLevel level, const std::string &message);
		void flush();

	private:
		std::string getCurrentTimeAsString();
		const char* getLogLevelAsString(LogLevel level);

		struct TAttachedSink
		{
			std::unique_ptr<ISink> sink;
			LogLevel lowerThreshold, upperThreshold;

			TAttachedSink(std::unique_ptr<ISink> &&_sink, LogLevel _lower, LogLevel _upper) : sink(std::move(_sink)), lowerThreshold(_lower), upperThreshold(_upper) {};
		};

		std::vector<TAttachedSink>	m_sinks;
		std::mutex					m_modifySinksMutex;
		std::mutex					m_logMutex;
	};

}

#endif