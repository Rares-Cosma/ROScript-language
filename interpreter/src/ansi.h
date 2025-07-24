#pragma once
#include <string>

void enableANSI();
enum class Color {
    Reset = 0,
    Red = 31,
    Green = 32,
    Yellow = 33,
    Blue = 34,
    Magenta = 35,
    Cyan = 36,
    White = 37,
    BrightRed = 91,
    BrightGreen = 92,
    BrightYellow = 93,
    BrightBlue = 94,
    BrightMagenta = 95,
    BrightCyan = 96,
};
inline std::string colorize(const std::string& text, Color color, bool bold) {
    std::string code = "\033[";
    if (bold) code += "1;";
    code += std::to_string(static_cast<int>(color)) + "m";
    return code + text + "\033[0m";
}
