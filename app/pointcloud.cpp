#include "pointcloud.h"
#include "camera.h"

#include <queue>
#include <utility>

PointCloud::PointCloud(SceneNode* parent, E57Data3DPtr data3D)
    : SceneNode(parent), m_data3D(std::move(data3D)), m_octree(),
      m_shader(":/shaders/default_vertex.glsl",
               ":/shaders/default_fragment.glsl")
{
}

PointCloud::~PointCloud() = default;

void PointCloud::render()
{
    SceneNode::render();
    m_shader.use();

    if (!visible())
        return;

    configureShader();
    auto* camera = scene()->findNode<Camera>();
    if (camera)
    {
        camera->configureShader();
    }

    for (auto& child : m_childNodes)
    {
        child->render();
    }
    m_shader.release();
}

void PointCloud::render2D(QPainter& painter)
{
    auto* camera = scene()->findNode<Camera>();
    if (!camera)
        return;

    auto origin = Vector4d(0.0f, 0.0f, 0.0f, 1.0f);
    origin = m_pose * origin;
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

void PointCloud::insertPoints(const std::vector<PointData>& data)
{
    m_octree.insert(data);
}

void PointCloud::doneInserting()
{
    m_octree.finalize();
    std::queue<OctreeNode*> visit;
    visit.push(&m_octree.root());
    while (!visit.empty())
    {
        auto* node = visit.front();
        visit.pop();
        if (!node)
        {
            continue;
        }
        if (node->isLeaf())
        {
            m_octreeNodes.push_back(node);
        }
        for (int i = 0; i < 8; ++i)
        {
            visit.push(node->child(i));
        }
    }

    for (auto* node : m_octreeNodes)
    {
        addChild(std::make_shared<PointCloudOctreeNode>(this, node));
    }
}

PointCloudOctreeNode::PointCloudOctreeNode(SceneNode* parent, OctreeNode* node)
    : SceneNode(parent), m_node{node}
{
    glGenVertexArrays(1, &m_vao);
    m_boundingBox = m_node->boundingBox();
}

PointCloudOctreeNode::~PointCloudOctreeNode()
{
    glDeleteVertexArrays(1, &m_vao);
}

void PointCloudOctreeNode::render()
{
    SceneNode::render();
    OpenGLArrayBuffer::Ptr buffer;
    auto& sc = *scene();

    if (!scene()->bufferCache().contains(id()))
    {
        buffer = std::make_shared<OpenGLArrayBuffer>(
            m_node->elements().data(), GL_FLOAT, 7,
            (int)m_node->elements().size(), GL_STATIC_DRAW);
        scene()->bufferCache().addItem(id(), buffer);
    }
    else
    {
        buffer = scene()->bufferCache().getItem(id()).value();
    }

    GLsizei stride = 7 * sizeof(GLfloat);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->buffer());

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                          (char*)0 + 0 * sizeof(GLfloat));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                          (char*)0 + 3 * sizeof(GLfloat));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride,
                          (char*)0 + 6 * sizeof(GLfloat));
    glEnableVertexAttribArray(2);

    glDrawArrays(GL_POINTS, 0, buffer->elementCount());
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
