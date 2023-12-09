#include "boundingbox.h"

BoundingBox BoundingBox::combine(const BoundingBox& other) const
{
    BoundingBox result{};
    result.min.setX(std::min(min.x(), other.min.x()));
    result.min.setY(std::min(min.y(), other.min.y()));
    result.min.setZ(std::min(min.z(), other.min.z()));
    result.max.setX(std::max(max.x(), other.max.x()));
    result.max.setY(std::max(max.y(), other.max.y()));
    result.max.setZ(std::max(max.z(), other.max.z()));
    return result;
}

void BoundingBox::setToInfinite()
{
    min.setX(std::numeric_limits<float>::max());
    min.setY(std::numeric_limits<float>::max());
    min.setZ(std::numeric_limits<float>::max());

    max.setX(std::numeric_limits<float>::min());
    max.setY(std::numeric_limits<float>::min());
    max.setZ(std::numeric_limits<float>::min());
}

void BoundingBox::transform(const QMatrix4x4& mat)
{
    min = mat * min;
    max = mat * max;
}

std::array<QVector3D, 8> BoundingBox::points() const
{
    return {{{min.x(), min.y(), min.z()},
             {min.x(), min.y(), max.z()},
             {min.x(), max.y(), min.z()},
             {min.x(), max.y(), max.z()},
             {max.x(), min.y(), min.z()},
             {max.x(), min.y(), max.z()},
             {max.x(), max.y(), min.z()},
             {max.x(), max.y(), max.z()}}};
}
