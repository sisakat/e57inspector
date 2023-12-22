#include "E57Utils.h"
#include <QImageReader>

const std::string ImageFormatName[] = {"jpeg", "png"};

QImage imageFromRawData(const uchar* data, int size,
                        E57Utils::ImageFormat imageFormat)
{
    // Disable image allocation limit (else 128MB)
    QImageReader::setAllocationLimit(0);

    QImage image;
    image.loadFromData(data, size, ImageFormatName[imageFormat].c_str());
    return image;
}

E57Utils::E57Utils(const E57Reader& reader) : m_reader(reader) {}

std::optional<E57NodePtr>
E57Utils::getImageRepresentation(const E57Image2D& image2D) const
{
    E57NodePtr imageRepresentation = nullptr;

    if (image2D.pinholeRepresentation())
    {
        imageRepresentation = image2D.pinholeRepresentation();
    }
    else if (image2D.sphericalRepresentation())
    {
        imageRepresentation = image2D.sphericalRepresentation();
    }
    else if (image2D.cylindricalRepresentation())
    {
        imageRepresentation = image2D.cylindricalRepresentation();
    }

    if (imageRepresentation)
    {
        return imageRepresentation;
    }
    return std::nullopt;
}

std::optional<QImage> E57Utils::getImage(const E57Image2D& image2D) const
{
    auto imageRepresentation = getImageRepresentation(image2D);
    if (!imageRepresentation)
        return std::nullopt;
    auto blobId = getImageBlobId(*imageRepresentation);
    if (!blobId)
        return std::nullopt;
    auto imageFormat = getImageFormat(*imageRepresentation);
    if (!imageFormat)
        return std::nullopt;

    auto rawData = m_reader.blobData(*blobId);
    return imageFromRawData(reinterpret_cast<const uchar*>(rawData.data()),
                            rawData.size(), *imageFormat);
}

std::optional<uint32_t> E57Utils::getImageBlobId(const E57NodePtr& node) const
{
    if (node->blobs().contains("jpegImage"))
    {
        return node->blobs().at("jpegImage");
    }
    else if (node->blobs().contains("pngImage"))
    {
        return node->blobs().at("pngImage");
    }

    return std::nullopt;
}

std::optional<E57Utils::ImageFormat>
E57Utils::getImageFormat(const E57NodePtr& node) const
{
    if (node->blobs().contains("jpegImage"))
    {
        return ImageFormat::JPEG;
    }
    else if (node->blobs().contains("pngImage"))
    {
        return ImageFormat::PNG;
    }

    return std::nullopt;
}
std::optional<E57Utils::ImageParameters>
E57Utils::getImageParameters(const E57Image2D& image2D) const
{
    auto representation = getImageRepresentation(image2D);
    if (!representation)
        return std::nullopt;

    E57Utils::ImageParameters parameters{};

    // spherical image not supported
    if (std::dynamic_pointer_cast<E57SphericalRepresentation>(
            representation.value()))
    {
        parameters.isSpherical = true;
    }
    else
    {
        parameters.focalLength =
            representation.value()->getDouble("focalLength");
    }

    parameters.width = representation.value()->getInteger("imageWidth");
    parameters.height = representation.value()->getInteger("imageHeight");
    parameters.pixelWidth = representation.value()->getDouble("pixelWidth");
    parameters.pixelHeight = representation.value()->getDouble("pixelHeight");
    return parameters;
}

Matrix4d e57PoseToMatrix4d(const E57Pose& pose)
{
    glm::quat quaternion(static_cast<float>(pose.rotation.w),
                         static_cast<float>(pose.rotation.x),
                         static_cast<float>(pose.rotation.y),
                         static_cast<float>(pose.rotation.z));
    Matrix4d mat(quaternion);
    mat[3] = Vector4d(static_cast<float>(pose.translation[0]),
                      static_cast<float>(pose.translation[1]),
                      static_cast<float>(pose.translation[2]), 1.0f);
    return mat;
}

Matrix4d E57Utils::getPose(const E57Data3D& node)
{
    return e57PoseToMatrix4d(node.pose());
}

Matrix4d E57Utils::getPose(const E57Image2D& node)
{
    return e57PoseToMatrix4d(node.pose());
}
