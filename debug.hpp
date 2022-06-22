#pragma once

#define ERROR "\033[31;1m" // Bold Red
#define WARN "\033[0;33m" // Yellow
#define INFO "\033[0m" // White
#define DEBUG "\033[38;5;239m" // Gray
#define TRACE "\033[0;32m" // Green

#define ERROR_N 0
#define WARN_N 1
#define INFO_N 2
#define DEBUG_N 3
#define TRACE_N 4

#define CURRENT_LOG_LEVEL TRACE_N
#define LOGGING_FILE stderr

#define LOG(logLevel, logLevelColor , message) \
do { \
	if (logLevel <= CURRENT_LOG_LEVEL) { \
		fprintf(LOGGING_FILE, "%s => %s: [%d]	%s%s%s\n", __FILE__, __func__, __LINE__, logLevelColor, message, INFO); \
	} \
} while(0)

#define ELOG(message) \
LOG(ERROR_N, ERROR, message);

#define WLOG(message) \
LOG(WARN_N, WARN ,message);

#define ILOG(message) \
LOG(INFO_N, INFO, message);

#define DLOG(message) \
LOG(DEBUG_N, DEBUG, message);

#define TLOG(message) \
LOG(TRACE_N, TRACE, message);

	// Debbugging messages example
	// ELOG("Test error message");
	// WLOG("Test warn message");
	// ILOG("Test info message");
	// DLOG("Test debug message");
	// TLOG("Test trace message");

