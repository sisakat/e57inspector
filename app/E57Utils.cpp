#include "E57Utils.h"
#include <QImageReader>

static const int BUFFER_SIZE = 10000;

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

std::optional<QImage> E57Utils::getImageMask(const E57Image2D& image2D) const
{
    auto imageRepresentation = getImageRepresentation(image2D);
    if (!imageRepresentation)
        return std::nullopt;
    auto blobId = getBlobId(*imageRepresentation, "imageMask");
    if (!blobId)
        return std::nullopt;

    auto rawData = m_reader.blobData(*blobId);
    return imageFromRawData(reinterpret_cast<const uchar*>(rawData.data()),
                            rawData.size(), ImageFormat::PNG);
}

std::optional<uint32_t> E57Utils::getBlobId(const E57NodePtr& node,
                                            const std::string& name) const
{
    if (node->blobs().contains(name))
    {
        return node->blobs().at(name);
    }
    return std::nullopt;
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
        parameters.principalPointX = 
            representation.value()->getDouble("principalPointX");
        parameters.principalPointY = 
            representation.value()->getDouble("principalPointY");
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

std::optional<PointCloudData> E57Utils::getData3D(E57Data3D& data3D) const
{
    if (!data3D.data().contains("points"))
    {
        return std::nullopt;
    }

    auto dataInfo = m_reader.dataInfo(data3D.data().at("points"));

    auto hasAttribute = [&dataInfo](const std::string& name)
    {
        return std::any_of(dataInfo.begin(), dataInfo.end(),
                           [&name](const auto& info)
                           { return info.identifier == name; });
    };

    auto sphericalToCartesian = [](std::array<float, 3> rtp)
    {
        const float r = rtp[0];
        const float sinTheta = std::sin(rtp[1]);
        const float sinPhi = std::sin(rtp[2]);
        const float cosTheta = std::cos(rtp[1]);
        const float cosPhi = std::cos(rtp[2]);
        return std::array<float, 3>{r * cosTheta * cosPhi,
                                    r * cosTheta * sinPhi, r * sinTheta};
    };

    auto dataReader = m_reader.dataReader(data3D.data().at("points"));

    bool isSpherical = false;
    std::vector<std::array<float, 3>> xyz(BUFFER_SIZE);
    if (hasAttribute("cartesianX") && hasAttribute("cartesianY") &&
        hasAttribute("cartesianZ"))
    {
        dataReader.bindBuffer("cartesianX", (float*)&xyz[0][0], xyz.size(),
                              3 * sizeof(float));
        dataReader.bindBuffer("cartesianY", (float*)&xyz[0][1], xyz.size(),
                              3 * sizeof(float));
        dataReader.bindBuffer("cartesianZ", (float*)&xyz[0][2], xyz.size(),
                              3 * sizeof(float));
    }
    else if (hasAttribute("sphericalRange") &&
             hasAttribute("sphericalAzimuth") &&
             hasAttribute("sphericalElevation"))
    {
        isSpherical = true;
        dataReader.bindBuffer("sphericalRange", (float*)&xyz[0][0], xyz.size(),
                              3 * sizeof(float));
        dataReader.bindBuffer("sphericalElevation", (float*)&xyz[0][1],
                              xyz.size(), 3 * sizeof(float));
        dataReader.bindBuffer("sphericalAzimuth", (float*)&xyz[0][2],
                              xyz.size(), 3 * sizeof(float));
    }

    bool hasInvalidPoints = false;
    std::vector<int> invalid(0);
    if (hasAttribute("cartesianInvalidState"))
    {
        hasInvalidPoints = true;
        invalid.resize(BUFFER_SIZE);
        dataReader.bindBuffer("cartesianInvalidState", (int*)&invalid[0],
                              invalid.size(), 1 * sizeof(int));
    }
    else if (hasAttribute("sphericalInvalidState"))
    {
        hasInvalidPoints = true;
        invalid.resize(BUFFER_SIZE);
        dataReader.bindBuffer("sphericalInvalidState", (int*)&invalid[0],
                              invalid.size(), 1 * sizeof(int));
    }

    bool hasColor = false;
    bool hasIntensity = false;
    std::vector<std::array<float, 3>> rgb(0);
    if (hasAttribute("colorRed") && hasAttribute("colorGreen") &&
        hasAttribute("colorBlue"))
    {
        rgb.resize(BUFFER_SIZE);
        hasColor = true;
        dataReader.bindBuffer("colorRed", (float*)&rgb[0][0], rgb.size(),
                              3 * sizeof(float));
        dataReader.bindBuffer("colorGreen", (float*)&rgb[0][1], rgb.size(),
                              3 * sizeof(float));
        dataReader.bindBuffer("colorBlue", (float*)&rgb[0][2], rgb.size(),
                              3 * sizeof(float));
    }

    std::vector<float> intensity(0);
    if (hasAttribute("intensity"))
    {
        intensity.resize(BUFFER_SIZE);
        hasIntensity = true;
        dataReader.bindBuffer("intensity", (float*)&intensity[0],
                              intensity.size());
    }

    uint64_t count;
    uint64_t totalCount = 0;
    PointCloudData data;
    while ((count = dataReader.read()) > 0)
    {
        for (size_t i = 0; i < count; ++i)
        {
            if (hasInvalidPoints && invalid[i] > 0)
            {
                continue;
            }

            if (isSpherical)
            {
                data.xyz.push_back(sphericalToCartesian(xyz[i]));
            }
            else
            {
                data.xyz.push_back(xyz[i]);
            }

            if (hasColor)
            {
                data.rgba.push_back({rgb[i][0] / 255.0f, rgb[i][1] / 255.0f,
                                     rgb[i][2] / 255.0f, 1.0f});
            }

            if (hasIntensity)
            {
                data.intensity.push_back(intensity[i]);
            }
            else
            {
                data.intensity.push_back(1.0f);
            }
        }
    }

    if (!data.intensity.empty())
    {
        float minIntensity =
            *std::min_element(data.intensity.begin(), data.intensity.end());
        float maxIntensity =
            *std::max_element(data.intensity.begin(), data.intensity.end());
        std::for_each(data.intensity.begin(), data.intensity.end(),
                      [minIntensity, maxIntensity](auto& intensity)
                      {
                          intensity -= minIntensity;
                          intensity /= (maxIntensity - minIntensity);
                      });
    }

    return data;
}
