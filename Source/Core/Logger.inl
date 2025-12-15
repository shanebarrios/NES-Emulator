inline Logger g_Logger;

inline constexpr std::string_view LogLevelToPrefix(LogLevel level)
{
	switch (level)
	{
	case LogLevel::Verbose:
		return "[\033[37mverbose\033[0m]: ";
	case LogLevel::Info:
		return "[\033[32minfo\033[0m]: ";
	case LogLevel::Warn:
		return "[\033[33mwarn\033[0m]: ";
	case LogLevel::Error:
		return "[\033[31merror\033[0m]: ";
	}
}

template <LogLevel level, typename... Args>
inline void Logger::Log(std::string_view format, Args&&... args)
{
	if (static_cast<int>(level) < static_cast<int>(m_Level)) return;

	constexpr std::string_view logLevelPrefix = LogLevelToPrefix(level);

	constexpr usize prefixLen = logLevelPrefix.size();

	std::memcpy(m_Buf.data(), logLevelPrefix.data(), prefixLen);

	const int written = std::snprintf(
		m_Buf.data() + prefixLen,
		m_Buf.size() - prefixLen,
		format.data(),
		args...);

	fwrite(m_Buf.data(), 1, prefixLen + written, m_Out);
	fputc('\n', m_Out);
}