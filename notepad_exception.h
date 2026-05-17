#pragma once
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

// Thrown when a file cannot be read
class file_read_exception : public notepad_exception {
public:
    explicit file_read_exception(const std::string& filename)
        : notepad_exception("Failed to read file: " + filename) {}
};

// Thrown when a file cannot be written
class file_write_exception : public notepad_exception {
public:
    explicit file_write_exception(const std::string& filename)
        : notepad_exception("Failed to write file: " + filename) {}
};