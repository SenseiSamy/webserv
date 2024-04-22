#pragma once

#include <map>
#include <string>

class ErrorCodes
{
  private:
    std::map<int, std::string> _error_codes;

  public:
    ErrorCodes();
    ErrorCodes(const ErrorCodes& other);
    ~ErrorCodes();
    ErrorCodes& operator=(const ErrorCodes& other);

    const std::map<int, std::string>& get_error_codes() const;
    const std::string get_description(int error_code) const;
};
