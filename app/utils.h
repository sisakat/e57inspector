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

std::string formatE57XMLName(const std::string& str,
                             const char* separator = " ")
{
    std::string result;
    std::string token = str;
    auto pos = token.find(":");
    if (pos != std::string::npos)
    {
        std::string ns = token.substr(0, pos); // token before namespace
        for (auto& c : ns)
            c = (char)std::toupper(c);
        token = token.substr(pos + 1);
        result = "(" + ns + ") ";
    }

    bool first = true;
    char previous = ' ';
    for (const auto& ch : token)
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

#endif // E57INSPECTOR_UTILS_H
