#ifndef E57INSPECTOR_E57UTILS_H
#define E57INSPECTOR_E57UTILS_H

#include <E57Format.h>

inline bool isDefined(const e57::StructureNode& node, const std::string& path)
{
    try
    {
        return node.isDefined(path);
    }
    catch (...)
    {
        return false;
    }
}

#endif // E57INSPECTOR_E57UTILS_H
