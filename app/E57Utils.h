#ifndef E57INSPECTOR_E57UTILS_H
#define E57INSPECTOR_E57UTILS_H

#include <optional>

#include <QImage>
#include <e57inspector/E57Reader.h>
#include <e57inspector/E57Node.h>

class E57Utils
{
public:
    enum ImageFormat
    {
        JPEG = 0,
        PNG
    };

    explicit E57Utils(const E57Reader& reader);

    std::optional<E57NodePtr> getImageRepresentation(const E57Image2D& image2D) const;
    std::optional<QImage> getImage(const E57Image2D& image2D) const;
    std::optional<uint32_t> getImageBlobId(const E57NodePtr& node) const;
    std::optional<ImageFormat> getImageFormat(const E57NodePtr& node) const;

private:
    const E57Reader& m_reader;
};

#endif // E57INSPECTOR_E57UTILS_H
