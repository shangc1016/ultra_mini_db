#include "../include/log.h"

namespace minikv {

// 静态变量的初始化，不受修饰符的约束
// class static variables
int Logger::currentLogLevel = 2;

int Logger::facility = Logger::GenFacility();

int Logger::option = Logger::GenOption();

std::string Logger::ident = "minikv";

// class methods
void Logger::Debug(std::string str) { Logger::Log(LogDebug, str); }

void Logger::Info(std::string str) { Logger::Log(LogInfo, str); }

void Logger::Warning(std::string str) { Logger::Log(LogWarn, str); }

void Logger::Error(std::string str) { Logger::Log(LogError, str); }

void Logger::Emerg(std::string str) { Logger::Log(LogEmerg, str); }

void Logger::Trace(std::string tracePoint, std::string str) {
  Logger::Log(LogTrace, tracePoint + ":" + str);
}

// class static methods
int Logger::GetLogLevel() { return currentLogLevel; }

void Logger::SetLogLevel(int log_level) { Logger::currentLogLevel = log_level; }

int Logger::GenFacility() { return LOG_USER; }

int Logger::GenOption() { return LOG_CONS | LOG_PID; }

void Logger::Log(int logLevel, std::string str) {
  if (logLevel < currentLogLevel) return;

  std::string identStr;
  if (logLevel == LogDebug) identStr = "[DEBUG]";
  if (logLevel == LogInfo) identStr = "[ INFO]";
  if (logLevel == LogWarn) identStr = "[ WARN]";
  if (logLevel == LogError) identStr = "[ERROR]";
  if (logLevel == LogEmerg) identStr = "[ERERG]";
  identStr = ident + identStr;

  openlog(identStr.c_str(), option, LOG_USER);
  syslog(LOG_USER | logLevel, "%s", str.c_str());
  closelog();
}

}  // namespace minikv