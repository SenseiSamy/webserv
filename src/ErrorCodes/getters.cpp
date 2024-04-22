#include "ErrorCodes.hpp"

const std::map<int, std::string>& ErrorCodes::get_error_codes() const
{
    return _error_codes;
}

const std::string ErrorCodes::get_description(int error_code) const
{
    std::map<int, std::string>::const_iterator it = _error_codes.find(error_code);
    if (it != _error_codes.end())
        return it->second;
    return "Unknown Error";
}
