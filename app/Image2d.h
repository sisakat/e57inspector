#ifndef E57INSPECTOR_IMAGE2D_H
#define E57INSPECTOR_IMAGE2D_H

#include "scene.h"
#include "shader.h"
#include <e57inspector/E57Node.h>

#include <utility>

#include <QImage>

class Image2d : public SceneNode
{
public:
    using Ptr = std::shared_ptr<Image2d>;

    explicit Image2d(SceneNode* parent = nullptr);
    ~Image2d() override;

    void render() override;
    void render2D(QPainter& painter) override;
    void configureShader() override;

    void setImage(const QImage& image);

    float coneLength() const { return m_coneLength; }
    void setConeLength(float coneLength) { m_coneLength = coneLength; }

    uint32_t getImageWidth() const { return m_imageWidth; }
    void setImageWidth(uint32_t imageWidth) { m_imageWidth = imageWidth; }
    uint32_t getImageHeight() const { return m_imageHeight; }
    void setImageHeight(uint32_t imageHeight) { m_imageHeight = imageHeight; }
    double getPixelWidth() const { return m_pixelWidth; }
    void setPixelWidth(double pixelWidth) { m_pixelWidth = pixelWidth; }
    double getPixelHeight() const { return m_pixelHeight; }
    void setPixelHeight(double pixelHeight) { m_pixelHeight = pixelHeight; }
    double getFocalLength() const { return m_focalLength; }
    void setFocalLength(double focalLength) { m_focalLength = focalLength; }

private:
    Shader m_shader;
    QImage m_image;
    GLuint m_texture;
    float m_coneLength{1.0f};

    uint32_t m_imageWidth;
    uint32_t m_imageHeight;
    double m_pixelWidth;
    double m_pixelHeight;
    double m_focalLength;
};

#endif // E57INSPECTOR_IMAGE2D_H
