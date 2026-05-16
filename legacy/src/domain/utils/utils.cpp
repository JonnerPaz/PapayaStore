#include "domain/utils/utils.hpp"

#include <cctype>

std::string DomainUtils::trim(const std::string& value)
{
    size_t start = 0;
    while (start < value.size() && std::isspace(static_cast<unsigned char>(value[start]))) {
        ++start;
    }

    size_t end = value.size();
    while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1]))) {
        --end;
    }

    return value.substr(start, end - start);
}

std::string DomainUtils::sanitizeName(const std::string& value)
{
    const std::string trimmed = DomainUtils::trim(value);

    std::string sanitized;
    sanitized.reserve(trimmed.size());

    bool lastWasSpace = false;
    for (char c : trimmed) {
        const unsigned char uc = static_cast<unsigned char>(c);
        if (std::isspace(uc)) {
            if (!lastWasSpace) {
                sanitized.push_back(' ');
                lastWasSpace = true;
            }
            continue;
        }

        sanitized.push_back(c);
        lastWasSpace = false;
    }

    return sanitized;
}

std::string DomainUtils::normalizeName(const std::string& value)
{
    std::string normalized = sanitizeName(value);
    for (char& c : normalized) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }

    return normalized;
}
