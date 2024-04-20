#include "ErrorCodes.hpp"

const std::string ErrorCodes::get_description(int error_code) const
{
    error_code_map::const_iterator it = _error_codes.find(error_code);
    if (it != _error_codes.end())
        return it->second;
    return "Unknown Error";
}

