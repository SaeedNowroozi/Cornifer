#ifndef FILEMATCHER_H
#define FILEMATCHER_H

#include <string>
#include <vector>
#include <regex>

class FileMatcher {
public:
    FileMatcher(const std::vector<std::string>& patterns, bool case_insensitive = false);
    bool match(const std::string& filename) const;

private:
    static std::string wildcard_to_regex(const std::string& pattern);
    std::vector<std::regex> regex_patterns_;
};

#endif // FILEMATCHER_H


