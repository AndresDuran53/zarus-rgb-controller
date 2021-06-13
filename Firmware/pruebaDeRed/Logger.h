#ifndef Logger_h
#define Logger_h

#include <Arduino.h>

class Logger {
  private:
  public:
    const static int NONE_LOG = 0;
    const static int INFO_LOG = 1;
    const static int DEBUG_LOG = 2;
    static int systemLogLevel;

    static String generateDebugLevelTag(int levelTag);
    static void log(String message, int messageLevel);

};
#endif
