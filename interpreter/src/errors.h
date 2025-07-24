#pragma once
#include <string>
#include <exception>

extern std::string CURRENT_FILE;

class Error : public std::exception {
private:
    std::string full_message;

public:
    Error(const std::string& msg, const std::string& file, int line, const std::string& raw);

    const char* what() const noexcept override;
};
