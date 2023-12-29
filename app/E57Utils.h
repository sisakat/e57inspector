#ifndef E57INSPECTOR_E57UTILS_H
#define E57INSPECTOR_E57UTILS_H

#include <optional>

#include <QImage>
#include <e57inspector/E57Reader.h>
#include <e57inspector/E57Node.h>

#include "geometry.h"

struct PointCloudData
{
    std::vector<std::array<float, 3>> xyz;
    std::vector<std::array<float, 3>> normal;
    std::vector<float> intensity;
    std::vector<std::array<float, 4>> rgba;
};

class E57Utils
{
public:
    enum ImageFormat
    {
        JPEG = 0,
        PNG
    };

    struct ImageParameters
    {
        uint32_t width;
        uint32_t height;
        double pixelWidth;
        double pixelHeight;
        double focalLength;
        bool isSpherical{false};
    };

    explicit E57Utils(const E57Reader& reader);

    std::optional<E57NodePtr> getImageRepresentation(const E57Image2D& image2D) const;
    std::optional<QImage> getImage(const E57Image2D& image2D) const;
    std::optional<uint32_t> getImageBlobId(const E57NodePtr& node) const;
    std::optional<ImageFormat> getImageFormat(const E57NodePtr& node) const;
    std::optional<ImageParameters> getImageParameters(const E57Image2D& image2D) const;
    std::optional<PointCloudData> getData3D(E57Data3D& data3D) const;

    static Matrix4d getPose(const E57Data3D& node) ;
    static Matrix4d getPose(const E57Image2D& node) ;

private:
    const E57Reader& m_reader;
};

#endif // E57INSPECTOR_E57UTILS_H
