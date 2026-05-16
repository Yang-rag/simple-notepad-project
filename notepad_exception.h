pragma once
#include <stdexcept>
#include <string>
 
// Base exception for all notepad errors
class notepad_exception : public std::runtime_error {
public:
    explicit notepad_exception(const std::string& message)
        : std::runtime_error(message) {}
};