#pragma once

#include <map>
#include <string>

class ErrorCodes
{
  private:
    typedef std::map<int, std::string> error_code_map;
    error_code_map _error_codes;

  public:
    ErrorCodes();
    ErrorCodes(const ErrorCodes &other);
    ~ErrorCodes();
    ErrorCodes &operator=(const ErrorCodes &other);

    const std::string get_description(int error_code) const;
};
