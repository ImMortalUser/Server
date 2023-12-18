#pragma once
#include <string>
#include <regex>

class RegexUtils
{
public:
    static std::string extractFirstArgFromTwo(const std::string& input) {
        std::regex pattern("<(\\w+)> (\\w+) (\\w+)");
        std::smatch match;

        if (std::regex_match(input, match, pattern)) {
            return match[2];
        } else {
            return "";
        }
    }

    static std::string extractSecondArgFromTwo(const std::string& input) {
        std::regex pattern("<(\\w+)> (\\w+) (\\w+)");
        std::smatch match;

        if (std::regex_match(input, match, pattern)) {
            return match[3];
        } else {
            return "";
        }
    }

    static std::string extractArgFromOne(const std::string& input) {
        std::regex pattern("<\\w+> (\\w+)");
        std::smatch match;

        if (std::regex_match(input, match, pattern)) {
            return match[1];
        } else {
            return "";  // или любое другое значение по умолчанию
        }
    }
};


