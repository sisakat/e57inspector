#include "pointcloud.h"
#include "ShaderFactory.h"
#include "camera.h"

#include <array>
#include <queue>
#include <utility>

PointCloud::PointCloud(SceneNode* parent, E57Data3DPtr data3D)
    : SceneNode(parent), m_data3D(std::move(data3D)),
      m_shader(ShaderFactory::createShader(":/shaders/default_vertex.glsl",
                                           ":/shaders/default_fragment.glsl")),
      m_lineShader(ShaderFactory::createShader(":/shaders/line_vertex.glsl",
                                               ":/shaders/line_fragment.glsl"))
{
}

PointCloud::~PointCloud()
{
    if (m_vao >= 0)
    {
        GLuint vao = m_vao;
        glDeleteVertexArrays(1, &vao);
    }
}

void PointCloud::render()
{
    SceneNode::render();
    m_shader->use();

    if (!visible())
        return;

    configureShader();
    auto* camera = scene()->findNode<Camera>();
    if (camera)
    {
        camera->configureShader();
    }

    if (m_vao > 0)
    {
        glBindVertexArray(m_vao);
        glDrawArrays(GL_POINTS, 0, static_cast<int>(m_pointCount));
        glBindVertexArray(0);
    }

    m_shader->release();
}

void PointCloud::render2D(QPainter& painter)
{
    if (!visible())
        return;

    auto* camera = scene()->findNode<Camera>();
    if (!camera)
        return;

    auto origin = Vector4d(0.0f, 0.0f, 0.0f, 1.0f);
    origin = modelMatrix() * origin;
    auto positionScreen = camera->project(origin);

    if (positionScreen.z < 1.0f)
    {
        painter.setPen(Qt::black);
        painter.setBrush(Qt::white);
        painter.drawEllipse(
            static_cast<int>((positionScreen.x) - 5) /
                scene()->devicePixelRatio(),
            static_cast<int>((camera->viewportHeight() - 1 - positionScreen.y) /
                             scene()->devicePixelRatio()) -
                5,
            10, 10);

        painter.setPen(Qt::white);
        painter.drawText(
            static_cast<int>(positionScreen.x / scene()->devicePixelRatio()) +
                10,
            static_cast<int>((camera->viewportHeight() - 1 - positionScreen.y) /
                             scene()->devicePixelRatio()),
            QString::fromStdString(m_data3D->name()));
    }
}

void PointCloud::configureShader()
{
    SceneNode::configureShader();

    if (auto location = getUniformLocation("pointSize"))
    {
        glUniform1i(location.value(), m_pointSize);
    }

    if (auto location = getUniformLocation("viewType"))
    {
        glUniform1i(location.value(), static_cast<int>(m_viewType));
    }

    if (auto location = getUniformLocation("singleColor"))
    {
        float rgb[]{m_singleColor.redF(), m_singleColor.greenF(),
                    m_singleColor.blueF()};
        glUniform3fv(location.value(), 1, rgb);
    }
}

void PointCloud::setPointCloudData(const PointCloudData& pointCloudData)
{
    if (m_vao < 0)
    {
        GLuint vao;
        glGenVertexArrays(1, &vao);
        m_vao = vao;
    }

    glBindVertexArray(m_vao);

    if (!pointCloudData.xyz.empty())
    {
        m_bufferXYZ = std::make_shared<OpenGLArrayBuffer>(
            pointCloudData.xyz.data(), GL_FLOAT, 3, pointCloudData.xyz.size(),
            GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, m_bufferXYZ->buffer());
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                              (char*)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        m_pointCount = pointCloudData.xyz.size();

        m_boundingBox.reset();
        for (const auto& point : pointCloudData.xyz)
        {
            m_boundingBox.update(Vector3d(point[0], point[1], point[2]));
        }
    }

    if (!pointCloudData.normal.empty())
    {
        m_bufferNormal = std::make_shared<OpenGLArrayBuffer>(
            pointCloudData.normal.data(), GL_FLOAT, 3,
            pointCloudData.normal.size(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, m_bufferNormal->buffer());
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                              (char*)0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (!pointCloudData.intensity.empty())
    {
        m_bufferIntensity = std::make_shared<OpenGLArrayBuffer>(
            pointCloudData.intensity.data(), GL_FLOAT, 1,
            pointCloudData.intensity.size(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, m_bufferIntensity->buffer());
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(GLfloat),
                              (char*)0);
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (!pointCloudData.rgba.empty())
    {
        m_bufferRGBA = std::make_shared<OpenGLArrayBuffer>(
            pointCloudData.rgba.data(), GL_FLOAT, 4, pointCloudData.rgba.size(),
            GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, m_bufferRGBA->buffer());
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat),
                              (char*)0);
        glEnableVertexAttribArray(3);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    glBindVertexArray(0);
}
