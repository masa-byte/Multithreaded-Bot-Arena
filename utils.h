#pragma once
#include <string>

// Enum for color codes
enum class Color {
    Default,
    Red,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    White,
	Gray
};

// Custom hash for pair
struct pair_hash {
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ (h2 << 1); // simple XOR combine
    }
};

// Function to print colored text using ANSI codes
void printColoredText(const std::string& message, Color color);
