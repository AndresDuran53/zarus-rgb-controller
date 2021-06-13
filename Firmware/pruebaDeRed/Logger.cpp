#include "Logger.h"

int Logger::systemLogLevel = Logger::NONE_LOG;

String Logger::generateDebugLevelTag(int levelTag) {
  String resultTag = "[UNDEFINED] ";
  switch (levelTag) {
    case INFO_LOG:
      resultTag = "[INFO_LOG] ";
      break;
    case DEBUG_LOG:
      resultTag = "[DEBUG_LOG] ";
      break;
    default:
      resultTag = "[UNKNOW] ";
  }
  return resultTag;
}

void Logger::log(String message, int messageLevel) {
  String messageLevelTag = Logger::generateDebugLevelTag(messageLevel);
  if (systemLogLevel >= messageLevel) {
    Serial.print(messageLevelTag);
    Serial.print("| ");
    Serial.println(message);
  }
}
