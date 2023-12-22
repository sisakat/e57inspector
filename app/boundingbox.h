#ifndef E57INSPECTOR_BOUNDINGBOX_H
#define E57INSPECTOR_BOUNDINGBOX_H

#include "geometry.h"

#include <algorithm>
#include <numeric>
#include <array>

struct BoundingBox
{
    Vector4d min{NullPoint4d};
    Vector4d max{NullPoint4d};

    inline void setToInfinite()
    {
        min.x = std::numeric_limits<float>::max();
        min.y = std::numeric_limits<float>::max();
        min.z = std::numeric_limits<float>::max();

        max.x = std::numeric_limits<float>::min();
        max.y = std::numeric_limits<float>::min();
        max.z = std::numeric_limits<float>::min();
    }

    [[nodiscard]] inline BoundingBox combine(const BoundingBox& other) const
    {
        BoundingBox result{};
        result.min.x = std::min(min.x, other.min.x);
        result.min.y = std::min(min.y, other.min.y);
        result.min.z = std::min(min.z, other.min.z);
        result.max.x = std::max(max.x, other.max.x);
        result.max.y = std::max(max.y, other.max.y);
        result.max.z = std::max(max.z, other.max.z);
        return result;
    }

    [[nodiscard]] inline BoundingBox transform(const Matrix4d& mat) const
    {
        BoundingBox result{};
        result.min = mat * min;
        result.max = mat * max;
        return result;
    }

    [[nodiscard]] inline std::array<Vector3d, 8> points() const
    {
        return {{{min.x, min.y, min.z},
                 {min.x, min.y, max.z},
                 {min.x, max.y, min.z},
                 {min.x, max.y, max.z},
                 {max.x, min.y, min.z},
                 {max.x, min.y, max.z},
                 {max.x, max.y, min.z},
                 {max.x, max.y, max.z}}};
    }
};

#endif // E57INSPECTOR_BOUNDINGBOX_H
