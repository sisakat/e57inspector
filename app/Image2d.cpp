#include "Image2d.h"
#include "ShaderFactory.h"
#include "camera.h"

Image2d::Image2d(SceneNode* parent)
    : SceneNode(parent),
      m_shader(ShaderFactory::createShader(":/shaders/line_vertex.glsl",
                                           ":/shaders/line_fragment.glsl"))
{
}

void Image2d::render()
{
    if (!isVisible())
        return;

    SceneNode::render();
    m_shader->use();
    configureShader();

    auto* camera = scene()->findNode<Camera>();
    if (camera)
    {
        camera->configureShader();
    }

    double imageWidthMeter = m_imageWidth * m_pixelWidth;
    double imageHeightMeter = m_imageHeight * m_pixelHeight;
    double coneLength = m_coneLength;

    double tanX = (imageWidthMeter / 2.0) / m_focalLength;
    double tanY = (imageHeightMeter / 2.0) / m_focalLength;

    std::vector<Vector3d> lines{
        {0.0f, 0.0f, 0.0f},
        {tanX * coneLength, tanY * coneLength, -coneLength},
        {0.0f, 0.0f, 0.0f},
        {-tanX * coneLength, tanY * coneLength, -coneLength},
        {0.0f, 0.0f, 0.0f},
        {tanX * coneLength, -tanY * coneLength, -coneLength},
        {0.0f, 0.0f, 0.0f},
        {-tanX * coneLength, -tanY * coneLength, -coneLength},

        {tanX * coneLength, tanY * coneLength, -coneLength},
        {-tanX * coneLength, tanY * coneLength, -coneLength},
        {-tanX * coneLength, tanY * coneLength, -coneLength},
        {-tanX * coneLength, -tanY * coneLength, -coneLength},
        {-tanX * coneLength, -tanY * coneLength, -coneLength},
        {tanX * coneLength, -tanY * coneLength, -coneLength},
        {tanX * coneLength, -tanY * coneLength, -coneLength},
        {tanX * coneLength, tanY * coneLength, -coneLength}};

    std::vector<Vector3d> triangles{lines.at(7), lines.at(5), lines.at(1),
                                    lines.at(7), lines.at(1), lines.at(3)};

    std::vector<Vector2d> textureCoordinates{{0.0f, 0.0f}, {1.0f, 0.0f},
                                             {1.0f, 1.0f}, {0.0f, 0.0f},
                                             {1.0f, 1.0f}, {0.0f, 1.0f}};

    if (auto location = getUniformLocation("useTexture"))
    {
        glUniform1i(location.value(), 0);
    }

    GLuint vao;
    GLuint vbo;
    GLuint vto;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, lines.size() * 3 * sizeof(GLfloat),
                 lines.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                          (char*)0);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_LINES, 0, lines.size());

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    if (m_image.sizeInBytes() > 0)
    {
        if (auto location = getUniformLocation("useTexture"))
        {
            glUniform1i(location.value(), 1);
        }

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, triangles.size() * 3 * sizeof(GLfloat),
                     triangles.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                              (char*)0);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &vto);
        glBindBuffer(GL_ARRAY_BUFFER, vto);
        glBufferData(GL_ARRAY_BUFFER,
                     textureCoordinates.size() * 2 * sizeof(GLfloat),
                     textureCoordinates.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat),
                              (char*)0);
        glEnableVertexAttribArray(1);

        if (auto location = getUniformLocation("imageTexture"))
        {
            glUniform1i(location.value(), 0);
        }

        glBindTexture(GL_TEXTURE_2D, m_texture);
        glDrawArrays(GL_TRIANGLES, 0, triangles.size());
        glBindTexture(GL_TEXTURE_2D, 0);

        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &vto);
        glDeleteVertexArrays(1, &vao);
    }

    m_shader->release();
}

void Image2d::render2D(QPainter& painter)
{
    if (!isVisible())
        return;

    SceneNode::render2D(painter);
    auto* camera = scene()->findNode<Camera>();
    if (!camera)
        return;

    auto origin = Vector4d(0.0f, 0.0f, 0.0f, 1.0f);
    origin = pose() * origin;
    auto positionScreen = camera->project(origin);

    if (positionScreen.z < 1.0f)
    {
        painter.setPen(Qt::black);
        painter.setBrush(Qt::white);
        painter.drawEllipse(
            static_cast<int>(positionScreen.x) - 5,
            static_cast<int>(camera->viewportHeight() - 1 - positionScreen.y) -
                5,
            10, 10);

        painter.setPen(Qt::white);
        painter.drawText(
            static_cast<int>(positionScreen.x) + 10,
            static_cast<int>(camera->viewportHeight() - 1 - positionScreen.y),
            QString::fromStdString(name()));
    }
}

void Image2d::configureShader()
{
    SceneNode::configureShader();
}

void Image2d::setImage(const QImage& image)
{
    m_image = image.convertToFormat(QImage::Format_RGBA8888);
    m_image.mirror(false, true);
    if (m_image.sizeInBytes() <= 0)
        return;

    glGenTextures(1, &m_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_image.width(), m_image.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, m_image.bits());
    glBindTexture(GL_TEXTURE_2D, 0);
}

Image2d::~Image2d()
{
    if (m_image.sizeInBytes() > 0)
    {
        glDeleteTextures(1, &m_texture);
    }
}

bool Image2d::isVisible() const
{
    return m_visible;
}

void Image2d::setVisible(bool visible)
{
    m_visible = visible;
}
