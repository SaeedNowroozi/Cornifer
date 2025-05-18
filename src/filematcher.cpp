#include "filematcher.h"

FileMatcher::FileMatcher(const std::vector<std::string>& patterns, bool case_insensitive)
{
    std::regex::flag_type flags = std::regex::ECMAScript;
    if (case_insensitive) {
        flags |= std::regex::icase;
    }

    for (const auto& pat : patterns) {
        regex_patterns_.emplace_back(wildcard_to_regex(pat), flags);
    }
}

bool FileMatcher::match(const std::string& filename) const
{
    for (const auto& re : regex_patterns_) {
        if (std::regex_match(filename, re)) {
            return true;
        }
    }
    return false;
}

std::string FileMatcher::wildcard_to_regex(const std::string& pattern)
{
    std::string regex_pattern = "^";
    for (auto c : pattern) {
        switch (c) {
            case '*': regex_pattern += ".*"; break;
            case '?': regex_pattern += "."; break;
            case '.': case '^': case '$': case '+': case '{': case '}':
            case '[': case ']': case '|': case '(': case ')': case '\\':
                regex_pattern += '\\'; regex_pattern += c; break;
            default:
                regex_pattern += c;
        }
    }
    regex_pattern += "$";
    return regex_pattern;
}
