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
