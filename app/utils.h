#ifndef E57INSPECTOR_UTILS_H
#define E57INSPECTOR_UTILS_H

#include <algorithm>
#include <string>

std::string to_lower(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(),
                   [](char c) { return std::tolower(c); });
    return str;
}

std::string camelCaseToPascalCase(const std::string& str,
                                  const char* separator = " ")
{
    std::string result;
    bool first = true;
    for (const auto& ch : str)
    {
        if (std::tolower(ch) != ch || first)
        {
            first = false;
            if (result.size() > 0)
            {
                result.append(separator);
            }
            result += (char)std::toupper(ch);
        }
        else
        {
            result += ch;
        }
    }
    return result;
}

#endif // E57INSPECTOR_UTILS_H
