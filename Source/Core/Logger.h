#pragma once

#include "Common.h"

#include <cstdio>
#include <string_view>

enum class LogLevel
{
	Verbose,
	Info,
	Warn,
	Error
};

#define LOG_VERBOSE(fmt, ...) (g_Logger.Log<LogLevel::Verbose>(fmt, __VA_ARGS__))
#define LOG_INFO(fmt, ...) (g_Logger.Log<LogLevel::Info>(fmt, __VA_ARGS__))
#define LOG_WARN(fmt, ...) (g_Logger.Log<LogLevel::Warn>(fmt, __VA_ARGS__))
#define LOG_ERROR(fmt, ...) (g_Logger.Log<LogLevel::Error>(fmt, __VA_ARGS__))

class Logger
{
public:
	Logger() = default;

	void Init();

	void SetLogLevel(LogLevel level) { m_Level = level; }

	void SetOut(FILE* out) { m_Out = out; };

	template <LogLevel level, typename... Args>
	void Log(std::string_view fmt, Args&&... args);

private:
	LogLevel m_Level = LogLevel::Info;
	FILE* m_Out = stdout;

	Array<char, 512> m_Buf{};
};

#include "Logger.inl"