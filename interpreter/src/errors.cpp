#include "errors.h"
#include "ansi.h"

std::string CURRENT_FILE;

Error::Error(const std::string& msg, const std::string& file, int line, const std::string& raw) {
    full_message = colorize(file + ":" + std::to_string(line) + ": ", Color::Cyan, 0) + msg + colorize("\n>> " + raw, Color::Yellow, 0) + "\n";
}

const char* Error::what() const noexcept {
    return full_message.c_str();
}
