#pragma once

#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define GRAY "\033[90m"

#define ITALIC "\033[3m"
#define BOLD "\033[1m"
#define UNDERLINE "\033[4m"

enum log_level
{
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL
};

void log(int level, const std::string &message);
