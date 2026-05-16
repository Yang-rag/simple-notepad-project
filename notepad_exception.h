pragma once
#include <stdexcept>
#include <string>
 
// Base exception for all notepad errors
class notepad_exception : public std::runtime_error {
public:
    explicit notepad_exception(const std::string& message)
        : std::runtime_error(message) {}
};

// Thrown when a file cannot be found
class file_not_found_exception : public notepad_exception {
public:
    explicit file_not_found_exception(const std::string& filename)
        : notepad_exception("File not found: " + filename) {}
};