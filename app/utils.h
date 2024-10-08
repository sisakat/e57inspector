#ifndef E57INSPECTOR_UTILS_H
#define E57INSPECTOR_UTILS_H

#include <algorithm>
#include <string>

inline std::string to_lower(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(),
                   [](char c) { return std::tolower(c); });
    return str;
}

inline std::string camelCaseToPascalCase(const std::string& str,
                                  const char* separator = " ")
{
    std::string result;
    bool first = true;
    char previous = ' ';
    for (const auto& ch : str)
    {
        if ((std::tolower(ch) != ch || first) &&
            (previous < '0' || previous > '9'))
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
        previous = ch;
    }
    return result;
}

inline std::string makeFilename(const std::string& filename)
{
    std::string s = filename;
    std::string illegalChars = "\\/:?\"<>|";
    for (auto it = s.begin(); it < s.end(); ++it)
    {
        bool found = illegalChars.find(*it) != std::string::npos;
        if (found)
        {
            *it = ' ';
        }
    }
    return s;
}

#endif // E57INSPECTOR_UTILS_H
