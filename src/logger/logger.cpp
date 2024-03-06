#include "logger.hpp"
#include <cstdlib>

void log(int level, const std::string &message)
{
    std::string level_str;
    std::string color;

    switch (level)
    {
    case TRACE:
        level_str = "TRACE";
        color = GREEN;
        break;
    case DEBUG:
        level_str = "DEBUG";
        color = GREEN;
        break;
    case INFO:
        level_str = "INFO";
        color = GREEN;
        break;
    case WARN:
        level_str = "WARN";
        color = YELLOW;
        break;
    case ERROR:
        level_str = "ERROR";
        color = RED;
        break;
    case FATAL:
        level_str = "FATAL";
        color = RED;
        break;
    default:
        level_str = "UNKNOWN";
        color = RESET;
        break;
    }

    std::string log_message = std::string("[") + GRAY + std::string(__TIME__) + RESET "]" + color + "[" + level_str +
                              "]" + RESET ": " + message + "\n";
    std::cout << log_message;

    if (level == FATAL)
        _exit(EXIT_FAILURE);
}
