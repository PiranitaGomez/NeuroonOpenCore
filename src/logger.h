#ifndef LOGGER_H
#define LOGGER_H


#define ELPP_DISABLE_DEBUG_LOGS

#ifdef __linux__
	#define ELPP_STACKTRACE_ON_CRASH
#endif

#define ELPP_DEFAULT_LOG_FILE "neuroon-alg-core.log"

#include "easylogging++.h"


/*
 * You can use such a structure to capture the logs
 * this may be useful e.g. when using mobile devices
 * where you can't write to a file directly using
 * the C++ standard library
 */
#ifdef CAPTURE_LOGS_FROM_EASYLOGGING
struct callback : public el::LogDispatchCallback {

	std::ofstream* log_file;

	callback() {
	    const std::string filename("./some_log_file.log");
		log_file = new std::ofstream();
		log_file->open(filename, std::ofstream::app);
	}

	~callback() {
		log_file->close();
		delete log_file;
	}

	void handle(const el::LogDispatchData* handlePtr) {
		const el::LogMessage* logMessage = handlePtr->logMessage();
		std::string message = logMessage->message();
		message = logMessage->logger()->logBuilder()->build(logMessage, false);

		logMessage->logger()->logBuilder()->convertToColoredOutput(&message, logMessage->level());
		(*log_file) << message << std::endl;
	}
};
#endif

#define ONCE_PER_APP_INITIALIZE_LOGGER INITIALIZE_EASYLOGGINGPP

void configure_logger() {

#ifdef CAPTURE_LOGS_FROM_EASYLOGGING
	el::Helpers::installLogDispatchCallback<callback>("dummy");
#endif
	el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
	el::Logger* dummyLogger = el::Loggers::getLogger("dummy");
	el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format, "%datetime %level %logger %fbase:%line  %msg");
	CLOG(INFO, "default") << "Logger initialized";
}

#endif
