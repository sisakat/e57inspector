#ifndef E57INSPECTOR_BOUNDINGBOX_H
#define E57INSPECTOR_BOUNDINGBOX_H

#include <QMatrix4x4>
#include <QVector3D>
#include <algorithm>
#include <numeric>
#include <array>

struct BoundingBox
{
    QVector4D min{0.0f, 0.0f, 0.0f, 1.0f};
    QVector4D max{0.0f, 0.0f, 0.0f, 1.0f};

    void setToInfinite();

    [[nodiscard]] BoundingBox combine(const BoundingBox& other) const;
    void transform(const QMatrix4x4& mat);
    [[nodiscard]] std::array<QVector3D, 8> points() const;
};

#endif // E57INSPECTOR_BOUNDINGBOX_H
