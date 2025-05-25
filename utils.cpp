#include "utils.h"
#include <iostream>

void printColoredText(const std::string& message, Color color) {
    std::string colorCode;

    switch (color) {
    case Color::Red:     colorCode = "\033[31m"; break;
    case Color::Green:   colorCode = "\033[32m"; break;
    case Color::Yellow:  colorCode = "\033[33m"; break;
    case Color::Blue:    colorCode = "\033[34m"; break;
    case Color::Magenta: colorCode = "\033[35m"; break;
    case Color::Cyan:    colorCode = "\033[36m"; break;
    case Color::White:   colorCode = "\033[37m"; break;
    default:             colorCode = "\033[0m";  break;
    }

    std::cout << colorCode << message << "\033[0m" << std::endl;
}
