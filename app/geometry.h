#ifndef E57INSPECTOR_GEOMETRY_H
#define E57INSPECTOR_GEOMETRY_H

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

inline float deg2rad(float deg)
{
    return deg * M_PIf / 180.0f;
}

inline float rad2deg(float rad)
{
    return rad * 180.0f / M_PIf;
}

using Vector2d = glm::vec2;
using Vector3d = glm::vec3;
using Vector4d = glm::vec4;
using Matrix3d = glm::mat3;
using Matrix4d = glm::mat4;

inline const Matrix3d IdentityMatrix3d(1.0f);
inline const Matrix4d IdentityMatrix4d(1.0f);

inline const Vector3d NullVector3d(0.0f, 0.0f, 0.0f);
inline const Vector3d NullPoint3d(NullVector3d);
inline const Vector4d NullPoint4d(NullVector3d, 1.0f);
inline const Vector4d NullVector4d(NullVector3d, 0.0f);

inline const Vector3d X_AXIS(1.0f, 0.0f, 0.0f);
inline const Vector3d Y_AXIS(0.0f, 1.0f, 0.0f);
inline const Vector3d Z_AXIS(0.0f, 0.0f, 1.0f);

inline const Vector4d X_AXIS4d(X_AXIS, 0.0f);
inline const Vector4d Y_AXIS4d(Y_AXIS, 0.0f);
inline const Vector4d Z_AXIS4d(Z_AXIS, 0.0f);

inline static Matrix4d RotationMatrixX(float angle)
{
    return glm::rotate(IdentityMatrix4d, angle, X_AXIS);
}

inline static Matrix4d RotationMatrixY(float angle)
{
    return glm::rotate(IdentityMatrix4d, angle, Y_AXIS);
}

inline static Matrix4d RotationMatrixZ(float angle)
{
    return glm::rotate(IdentityMatrix4d, angle, Z_AXIS);
}

inline static Matrix4d RotationMatrix(float angle, const Vector3d& axis)
{
    return glm::rotate(IdentityMatrix4d, angle, axis);
}

inline Matrix4d TranslationMatrix(const Vector3d& translation)
{
    Matrix4d mat(IdentityMatrix4d);
    mat[3] = Vector4d(translation, 1.0f);
    return mat;
}

inline Matrix4d TranslationMatrix(const Vector4d& translation)
{
    assert(translation.w == 1.0f);
    Matrix4d mat(IdentityMatrix4d);
    mat[3] = translation;
    return mat;
}

template <typename Matrix>
inline Matrix InverseMatrix(const Matrix& matrix)
{
    return glm::inverse(matrix);
}

template <typename Vec>
inline auto VectorNormalize(const Vec& vec)
{
    return glm::normalize(vec);
}

template <typename A, typename B>
inline auto VectorDot(const A& a, const B& b)
{
    return glm::dot(a, b);
}

inline Vector3d VectorCross(const Vector3d& a, const Vector3d& b)
{
    return glm::cross(a, b);
}

template <typename Vec>
inline auto VectorLength(const Vec& vec)
{
    return glm::length(vec);
}

inline Vector3d VectorNegate(const Vector3d& vec)
{
    Vector3d result(vec);
    result *= -1.0f;
    return result;
}

inline Vector4d VectorNegate(const Vector4d& vec)
{
    return {VectorNegate(Vector3d(vec)), vec.w};
}

#endif // E57INSPECTOR_GEOMETRY_H
