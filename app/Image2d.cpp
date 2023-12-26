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

    if (auto location = getUniformLocation("useTexture"))
    {
        glUniform1i(location.value(), 0);
    }

    createBuffers();

    auto createVAO = [this](OpenGLArrayBuffer::Ptr& buffer) -> GLuint
    {
        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, buffer->buffer());
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                              (char*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                              (char*)0 + 3 * sizeof(GLfloat));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                              (char*)0 + 6 * sizeof(GLfloat));
        glEnableVertexAttribArray(2);
        return vao;
    };

    GLuint vao;
    vao = createVAO(m_lineBuffer);
    glDrawArrays(GL_LINES, 0, m_lineBuffer->elementCount());
    glDeleteVertexArrays(1, &vao);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (m_image.sizeInBytes() > 0)
    {
        if (auto location = getUniformLocation("useTexture"))
        {
            glUniform1i(location.value(), 1);
        }

        if (auto location = getUniformLocation("imageTexture"))
        {
            glUniform1i(location.value(), 0);
        }

        if (m_backfaceCulling)
        {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
        }

        vao = createVAO(m_triangleBuffer);
        glBindTexture(GL_TEXTURE_2D, m_texture);
        glDrawArrays(GL_TRIANGLES, 0, m_triangleBuffer->elementCount());
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteVertexArrays(1, &vao);

        glCullFace(GL_NONE);
        glDisable(GL_CULL_FACE);
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

    if (!camera->isPickpointNavigation())
        return;

    auto origin = Vector4d(0.0f, 0.0f, 0.0f, 1.0f);
    origin = modelMatrix() * origin;
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

void Image2d::createBuffers()
{
    if (m_lastRevision < m_revision)
    {
        if (isSpherical())
        {
            createViewConeLinesSpherical();
            createViewConeImageSpherical();
        }
        else
        {
            createViewConeLines();
            createViewConeImage();
        }
    }
}

void Image2d::createViewConeLines()
{
    std::vector<Image2d::VertexData> data;

    double imageWidthMeter = m_imageWidth * m_pixelWidth;
    double imageHeightMeter = m_imageHeight * m_pixelHeight;

    double tanX = (imageWidthMeter / 2.0) / m_focalLength;
    double tanY = (imageHeightMeter / 2.0) / m_focalLength;

    Vector3d rgb{1.0f, 1.0f, 1.0f};
    Vector2d tex{0.0f, 0.0f};

    // clang-format off
    // cone lines
    data.push_back({NullPoint3d, rgb, tex});
    data.push_back({{ tanX * m_coneLength,  tanY * m_coneLength, -m_coneLength}, rgb, tex});

    data.push_back({NullPoint3d, rgb, tex});
    data.push_back({{-tanX * m_coneLength,  tanY * m_coneLength, -m_coneLength}, rgb, tex});

    data.push_back({NullPoint3d, rgb, tex});
    data.push_back({{ tanX * m_coneLength, -tanY * m_coneLength, -m_coneLength}, rgb, tex});

    data.push_back({NullPoint3d, rgb, tex});
    data.push_back({{-tanX * m_coneLength, -tanY * m_coneLength, -m_coneLength}, rgb, tex});

    // image rectangle
    data.push_back({{ tanX * m_coneLength, tanY * m_coneLength, -m_coneLength}, rgb, tex});
    data.push_back({{-tanX * m_coneLength, tanY * m_coneLength, -m_coneLength}, rgb, tex});

    data.push_back({{-tanX * m_coneLength,  tanY * m_coneLength, -m_coneLength}, rgb, tex});
    data.push_back({{-tanX * m_coneLength, -tanY * m_coneLength, -m_coneLength}, rgb, tex});

    data.push_back({{-tanX * m_coneLength, -tanY * m_coneLength, -m_coneLength}, rgb, tex});
    data.push_back({{ tanX * m_coneLength, -tanY * m_coneLength, -m_coneLength}, rgb, tex});

    data.push_back({{tanX * m_coneLength, -tanY * m_coneLength, -m_coneLength}, rgb, tex});
    data.push_back({{tanX * m_coneLength,  tanY * m_coneLength, -m_coneLength}, rgb, tex});
    // clang-format on

    m_boundingBox.reset();
    for (const auto& point : data)
    {
        const auto& xyz = point.xyz;
        m_boundingBox.update(xyz);
    }

    if (m_showCoordinateSystemAxes)
    {
        Vector3d red{1.0f, 0.0f, 0.0f};
        Vector3d green{0.0f, 1.0f, 0.0f};
        Vector3d blue{0.0f, 0.0f, 1.0f};

        data.push_back({NullPoint3d, red, tex});
        data.push_back({X_AXIS, red, tex});

        data.push_back({NullPoint3d, green, tex});
        data.push_back({Y_AXIS, green, tex});

        data.push_back({NullPoint3d, blue, tex});
        data.push_back({Z_AXIS, blue, tex});
    }

    m_lineBuffer = std::make_shared<OpenGLArrayBuffer>(
        data.data(), GL_FLOAT, 3 + 3 + 2, data.size(), GL_STATIC_DRAW);
}

void Image2d::createViewConeImage()
{
    std::vector<Image2d::VertexData> data;

    double imageWidthMeter = m_imageWidth * m_pixelWidth;
    double imageHeightMeter = m_imageHeight * m_pixelHeight;

    double tanX = (imageWidthMeter / 2.0) / m_focalLength;
    double tanY = (imageHeightMeter / 2.0) / m_focalLength;

    Vector3d rgb{1.0f, 1.0f, 1.0f};

    // clang-format off
    data.push_back({{-tanX * m_coneLength, -tanY * m_coneLength, -m_coneLength}, rgb, {0.0f, 0.0f}});
    data.push_back({{tanX * m_coneLength, -tanY * m_coneLength, -m_coneLength}, rgb, {1.0f, 0.0f}});
    data.push_back({{tanX * m_coneLength, tanY * m_coneLength, -m_coneLength}, rgb, {1.0f, 1.0f}});

    data.push_back({{-tanX * m_coneLength, -tanY * m_coneLength, -m_coneLength}, rgb, {0.0f, 0.0f}});
    data.push_back({{tanX * m_coneLength, tanY * m_coneLength, -m_coneLength}, rgb, {1.0f, 1.0f}});
    data.push_back({{-tanX * m_coneLength, tanY * m_coneLength, -m_coneLength}, rgb, {0.0f, 1.0f}});
    // clang-format on

    m_triangleBuffer = std::make_shared<OpenGLArrayBuffer>(
        data.data(), GL_FLOAT, 3 + 3 + 2, data.size(), GL_STATIC_DRAW);
}

void Image2d::createViewConeLinesSpherical()
{
    std::vector<Image2d::VertexData> data;

    double imageWidthRadians = m_imageWidth * m_pixelWidth;
    double imageHeightRadians = m_imageHeight * m_pixelHeight;

    Vector3d rtpTopLeft{m_coneLength, imageHeightRadians / 2.0f,
                        imageWidthRadians / 2.0f};
    Vector3d rtpBottomLeft{m_coneLength, -imageHeightRadians / 2.0f,
                           imageWidthRadians / 2.0f};

    Vector3d rtpTopRight{m_coneLength, imageHeightRadians / 2.0f,
                         -imageWidthRadians / 2.0f};
    Vector3d rtpBottomRight{m_coneLength, -imageHeightRadians / 2.0f,
                            -imageWidthRadians / 2.0f};

    auto polarToCartesian = [](const Vector3d& rtp) -> Vector3d
    {
        double sinTheta = std::sin(rtp.y);
        double cosTheta = std::cos(rtp.y);
        double sinPhi = std::sin(rtp.z);
        double cosPhi = std::cos(rtp.z);

        double z = sinTheta * rtp.x;
        double a = cosTheta * rtp.x;

        return {cosPhi * a, sinPhi * a, z};
    };

    Vector3d rgb{1.0f, 1.0f, 1.0f};
    Vector2d tex{0.0f, 0.0f};

    data.push_back({NullPoint3d, rgb, tex});
    data.push_back({polarToCartesian(rtpTopLeft), rgb, tex});

    data.push_back({NullPoint3d, rgb, tex});
    data.push_back({polarToCartesian(rtpBottomLeft), rgb, tex});

    data.push_back({NullPoint3d, rgb, tex});
    data.push_back({polarToCartesian(rtpTopRight), rgb, tex});

    data.push_back({NullPoint3d, rgb, tex});
    data.push_back({polarToCartesian(rtpBottomRight), rgb, tex});

    double phiStart = -imageWidthRadians / 2.0;
    double phiStop = imageWidthRadians / 2.0;
    double increment = (phiStop - phiStart) / m_segments;

    Vector3d rtp{m_coneLength, -imageHeightRadians / 2.0, phiStart};
    data.push_back({polarToCartesian(rtp), rgb, tex});

    for (int i = 1; i <= m_segments; ++i)
    {
        double phi = phiStart + i * increment;
        double theta = -imageHeightRadians / 2.0;

        Vector3d rtp{m_coneLength, theta, phi};
        data.push_back({polarToCartesian(rtp), rgb, tex});
        if (i < m_segments)
        {
            data.push_back({polarToCartesian(rtp), rgb, tex});
        }
    }

    rtp = {m_coneLength, imageHeightRadians / 2.0, phiStart};
    data.push_back({polarToCartesian(rtp), rgb, tex});

    for (int i = 1; i <= m_segments; ++i)
    {
        double phi = phiStart + i * increment;
        double theta = imageHeightRadians / 2.0;

        Vector3d rtp{m_coneLength, theta, phi};
        data.push_back({polarToCartesian(rtp), rgb, tex});
        if (i < m_segments)
        {
            data.push_back({polarToCartesian(rtp), rgb, tex});
        }
    }

    m_boundingBox.reset();
    for (const auto& point : data)
    {
        const auto& xyz = point.xyz;
        m_boundingBox.update(xyz);
    }

    if (m_showCoordinateSystemAxes)
    {
        Vector3d red{1.0f, 0.0f, 0.0f};
        Vector3d green{0.0f, 1.0f, 0.0f};
        Vector3d blue{0.0f, 0.0f, 1.0f};

        data.push_back({NullPoint3d, red, tex});
        data.push_back({X_AXIS, red, tex});

        data.push_back({NullPoint3d, green, tex});
        data.push_back({Y_AXIS, green, tex});

        data.push_back({NullPoint3d, blue, tex});
        data.push_back({Z_AXIS, blue, tex});
    }

    m_lineBuffer = std::make_shared<OpenGLArrayBuffer>(
        data.data(), GL_FLOAT, 3 + 3 + 2, data.size(), GL_STATIC_DRAW);
}

void Image2d::createViewConeImageSpherical()
{
    std::vector<Image2d::VertexData> data;

    double imageWidthRadians = m_imageWidth * m_pixelWidth;
    double imageHeightRadians = m_imageHeight * m_pixelHeight;

    Vector3d rtpTopLeft{m_coneLength, imageHeightRadians / 2.0f,
                        imageWidthRadians / 2.0f};
    Vector3d rtpBottomLeft{m_coneLength, -imageHeightRadians / 2.0f,
                           imageWidthRadians / 2.0f};

    Vector3d rtpTopRight{m_coneLength, imageHeightRadians / 2.0f,
                         -imageWidthRadians / 2.0f};
    Vector3d rtpBottomRight{m_coneLength, -imageHeightRadians / 2.0f,
                            -imageWidthRadians / 2.0f};

    auto polarToCartesian = [](const Vector3d& rtp) -> Vector3d
    {
        double sinTheta = std::sin(rtp.y);
        double cosTheta = std::cos(rtp.y);
        double sinPhi = std::sin(rtp.z);
        double cosPhi = std::cos(rtp.z);

        double z = sinTheta * rtp.x;
        double a = cosTheta * rtp.x;

        return {cosPhi * a, sinPhi * a, z};
    };

    Vector3d rgb{1.0f, 1.0f, 1.0f};

    double phiStart = -imageWidthRadians / 2.0;
    double phiStop = imageWidthRadians / 2.0;
    double thetaStart = -imageHeightRadians / 2.0;
    double thetaStop = imageHeightRadians / 2.0;
    double phiIncrement = (phiStop - phiStart) / m_segments;
    double thetaIncrement = (thetaStop - thetaStart) / m_segments;

    for (int i = 0; i < m_segments; ++i)
    {
        double phi = phiStart + i * phiIncrement;
        for (int j = 0; j < m_segments; ++j)
        {
            double theta = thetaStart + j * thetaIncrement;

            // triangle 1
            Vector3d rtp{m_coneLength, theta, phi};
            data.push_back(
                {polarToCartesian(rtp),
                 rgb,
                 {1.0 - 1.0f / m_segments * i, 1.0f / m_segments * j}});

            rtp = {m_coneLength, theta + thetaIncrement, phi};
            data.push_back(
                {polarToCartesian(rtp),
                 rgb,
                 {1.0 - 1.0f / m_segments * i, 1.0f / m_segments * (j + 1)}});

            rtp = {m_coneLength, theta + thetaIncrement, phi + phiIncrement};
            data.push_back({polarToCartesian(rtp),
                            rgb,
                            {1.0 - 1.0f / m_segments * (i + 1),
                             1.0f / m_segments * (j + 1)}});

            // triangle 2
            rtp = {m_coneLength, theta, phi};
            data.push_back(
                {polarToCartesian(rtp),
                 rgb,
                 {1.0 - 1.0f / m_segments * i, 1.0f / m_segments * j}});

            rtp = {m_coneLength, theta + thetaIncrement, phi + phiIncrement};
            data.push_back({polarToCartesian(rtp),
                            rgb,
                            {1.0 - 1.0f / m_segments * (i + 1),
                             1.0f / m_segments * (j + 1)}});

            rtp = {m_coneLength, theta, phi + phiIncrement};
            data.push_back(
                {polarToCartesian(rtp),
                 rgb,
                 {1.0 - 1.0f / m_segments * (i + 1), 1.0f / m_segments * j}});
        }
    }

    m_boundingBox.reset();
    for (const auto& point : data)
    {
        const auto& xyz = point.xyz;
        m_boundingBox.update(xyz);
    }

    m_triangleBuffer = std::make_shared<OpenGLArrayBuffer>(
        data.data(), GL_FLOAT, 3 + 3 + 2, data.size(), GL_STATIC_DRAW);
}

bool Image2d::isFullPanorama() const
{
    return std::abs((m_imageWidth * m_pixelWidth) - 2 * M_PI) <
           std::numeric_limits<float>::epsilon();
}

void Image2d::cameraToImageView()
{
    auto camera = scene()->findNode<Camera>();
    if (camera)
    {
        camera->setPosition(modelMatrix()[3]);
        camera->setCenter(modelMatrix() *
                          (isSpherical() ? X_AXIS4d : -Z_AXIS4d));
        camera->setPickpointNavigation(false);
        camera->setFieldOfView(75.0f);
        setShowCoordinateSystemAxes(false);
    }
    scene()->update();
}
