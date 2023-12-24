#ifndef E57INSPECTOR_BOUNDINGBOX_H
#define E57INSPECTOR_BOUNDINGBOX_H

#include "geometry.h"

#include <algorithm>
#include <array>
#include <numeric>

struct BoundingBox
{
    Vector4d min{NullPoint4d};
    Vector4d max{NullPoint4d};

    /**
     * Combines two bounding boxes together. Assumes that both bounding boxes
     * are within the same coordinate system.
     * @param other
     * @return
     */
    [[nodiscard]] inline BoundingBox combine(const BoundingBox& other) const
    {
        if (!isValid())
            return other;
        if (!other.isValid())
            return *this;

        BoundingBox result{*this};
        result.min.x = std::min(result.min.x, other.min.x);
        result.min.y = std::min(result.min.y, other.min.y);
        result.min.z = std::min(result.min.z, other.min.z);
        result.max.x = std::max(result.max.x, other.max.x);
        result.max.y = std::max(result.max.y, other.max.y);
        result.max.z = std::max(result.max.z, other.max.z);
        return result;
    }

    /**
     * Transforms the axis-aligned bounding box using the given transformation.
     * Re-calculating the bounding box with respect to the new coordinate
     * system.
     * @param mat Transformation matrix
     * @return Transformed bounding box
     */
    [[nodiscard]] inline BoundingBox transform(const Matrix4d& mat) const
    {
        if (!isValid())
            return {};
        auto bbPoints = points();
        std::for_each(bbPoints.begin(), bbPoints.end(),
                      [&mat](auto& point)
                      { point = Vector3d(mat * Vector4d(point, 1.0f)); });
        BoundingBox result{};
        std::for_each(bbPoints.begin(), bbPoints.end(),
                      [&result](const auto& point)
                      {
                          result.min.x = std::min(result.min.x, point.x);
                          result.min.y = std::min(result.min.y, point.y);
                          result.min.z = std::min(result.min.z, point.z);
                          result.max.x = std::max(result.max.x, point.x);
                          result.max.y = std::max(result.max.y, point.y);
                          result.max.z = std::max(result.max.z, point.z);
                      });
        return result;
    }

    /**
     * Returns the bounding box corners, starting with the bottom plane
     * with the lower-left corner in counter-clockwise direction.
     * @return Bounding box corner points
     */
    [[nodiscard]] inline std::array<Vector3d, 8> points() const
    {
        return {{{min.x, min.y, min.z},
                 {max.x, min.y, min.z},
                 {max.x, max.y, min.z},
                 {min.x, max.y, min.z},
                 {min.x, min.y, max.z},
                 {max.x, min.y, max.z},
                 {max.x, max.y, max.z},
                 {min.x, max.y, max.z}}};
    }

    [[nodiscard]] inline bool isValid() const
    {
        if (std::abs(min.x) < std::numeric_limits<float>::epsilon() &&
            std::abs(min.y) < std::numeric_limits<float>::epsilon() &&
            std::abs(min.z) < std::numeric_limits<float>::epsilon() &&
            std::abs(max.x) < std::numeric_limits<float>::epsilon() &&
            std::abs(max.y) < std::numeric_limits<float>::epsilon() &&
            std::abs(max.z) < std::numeric_limits<float>::epsilon())
        {
            return false;
        }
        return true;
    }

    inline void update(const Vector3d& point)
    {
        min.x = std::min(min.x, point.x);
        min.y = std::min(min.y, point.y);
        min.z = std::min(min.z, point.z);
        max.x = std::max(max.x, point.x);
        max.y = std::max(max.y, point.y);
        max.z = std::max(max.z, point.z);
    }

    inline void reset() { *this = {}; }
};

#endif // E57INSPECTOR_BOUNDINGBOX_H
