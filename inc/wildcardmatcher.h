#ifndef WILDCARDMATCHER_H
#define WILDCARDMATCHER_H


#include <regex>
#include <string>
#include <vector>

class WildcardMatcher {
public:
    explicit WildcardMatcher(bool caseInsensitive = false);

    // اضافه کردن الگوی wildcard مانند "*.txt"
    void addPattern(const std::string& wildcard);

    // بررسی تطابق یک نام فایل با یکی از الگوها
    bool matches(const std::string& filename) const;

private:
    std::vector<std::regex> regexPatterns;
    bool caseInsensitive;

    std::regex wildcardToRegex(const std::string& wildcard) const;
};

#endif // WILDCARDMATCHER_H
