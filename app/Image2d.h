#ifndef E57INSPECTOR_IMAGE2D_H
#define E57INSPECTOR_IMAGE2D_H

#include "openglarraybuffer.h"
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
    void setConeLength(float coneLength)
    {
        m_coneLength = coneLength;
        ++m_revision;
    }

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

    bool isVisible() const;
    void setVisible(bool visible);

    bool isShowCoordinateSystemAxes() const { return m_showCoordinateSystemAxes; }
    void setShowCoordinateSystemAxes(bool showCoordinateAxes)
    {
        m_showCoordinateSystemAxes = showCoordinateAxes;
        ++m_revision;
    }

    int getSegments() const { return m_segments; }
    void setSegments(int segments)
    {
        m_segments = segments;
        ++m_revision;
    }

    bool isSpherical() const { return m_isSpherical; }
    void setIsSpherical(bool isSpherical)
    {
        m_isSpherical = isSpherical;
        ++m_revision;
    }

    bool isBackfaceCulling() const { return m_backfaceCulling; }
    void setBackfaceCulling(bool backfaceCulling)
    {
        m_backfaceCulling = backfaceCulling;
    }

    bool isFullPanorama() const;

    void cameraToImageView();

private:
    Shader::Ptr m_shader;
    QImage m_image;
    GLuint m_texture;
    float m_coneLength{1.0f};
    bool m_visible{true};
    bool m_showCoordinateSystemAxes{true};
    int m_lastRevision{-1};
    int m_revision{0};
    int m_segments{20};
    bool m_backfaceCulling{false};

    OpenGLArrayBuffer::Ptr m_lineBuffer;
    OpenGLArrayBuffer::Ptr m_triangleBuffer;

    uint32_t m_imageWidth;
    uint32_t m_imageHeight;
    double m_pixelWidth;
    double m_pixelHeight;
    double m_focalLength;
    bool m_isSpherical{false};

    struct VertexData
    {
        Vector3d xyz;
        Vector3d rgb;
        Vector2d tex;
    };

    void createBuffers();
    void createViewConeLines();
    void createViewConeImage();
    void createViewConeLinesSpherical();
    void createViewConeImageSpherical();
};

#endif // E57INSPECTOR_IMAGE2D_H
