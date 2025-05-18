#include "wildcardmatcher.h"
#include <regex>


WildcardMatcher::WildcardMatcher(bool caseInsensitive)
    : caseInsensitive(caseInsensitive) {}

void WildcardMatcher::addPattern(const std::string& wildcard) {
    regexPatterns.emplace_back(wildcardToRegex(wildcard));
}

bool WildcardMatcher::matches(const std::string& filename) const {
    for (const auto& regex : regexPatterns) {
        if (std::regex_match(filename, regex)) {
            return true;
        }
    }
    return false;
}

std::regex WildcardMatcher::wildcardToRegex(const std::string& wildcard) const {
    std::string regexPattern = "^";
    for (char c : wildcard) {
        switch (c) {
            case '*': regexPattern += ".*"; break;
            case '?': regexPattern += "."; break;
            case '.': case '^': case '$': case '+':
            case '{': case '}': case '[': case ']':
            case '|': case '(': case ')': case '\\':
                regexPattern += '\\'; regexPattern += c; break;
            default:
                if (caseInsensitive && std::isalpha(c)) {
                    // ایجاد regex برای case-insensitive مانند [Aa]
                    regexPattern += "[";
                    regexPattern += (char)std::tolower(c);
                    regexPattern += (char)std::toupper(c);
                    regexPattern += "]";
                } else {
                    regexPattern += c;
                }
                break;
        }
    }
    regexPattern += "$";

    std::regex::flag_type flags = std::regex::ECMAScript;
    return std::regex(regexPattern, flags);
}
