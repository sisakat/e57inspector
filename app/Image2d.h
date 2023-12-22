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

    void setImage2D(E57Image2DPtr image2D) { m_image2D = std::move(image2D); }

    void
    setPinholeRepresentation(E57PinholeRepresentationPtr pinholeRepresentation)
    {
        m_pinholeRepresentation = std::move(pinholeRepresentation);
    }

    E57Image2DPtr image2D() const { return m_image2D; }

    void setImage(const QImage& image);

    float coneLength() const { return m_coneLength; }
    void setConeLength(float coneLength) { m_coneLength = coneLength; }

private:
    Shader m_shader;

    E57Image2DPtr m_image2D;
    E57PinholeRepresentationPtr m_pinholeRepresentation;
    QImage m_image;

    GLuint m_texture;

    float m_coneLength{1.0f};
};

#endif // E57INSPECTOR_IMAGE2D_H
