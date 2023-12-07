#include "pointcloud.h"

#include <queue>

PointCloud::PointCloud(Octree& octree) : SceneNode(), m_octree{octree}
{
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
        addChild(std::make_shared<PointCloudOctreeNode>(node));
    }

    m_pose.setToIdentity();
}

PointCloud::~PointCloud() {}

void PointCloud::render()
{
    SceneNode::render();
    scene()->shader()->setUniformInt("pointSize", m_pointSize);
    for (auto& child : m_childNodes)
    {
        child->render();
    }
}

PointCloudOctreeNode::PointCloudOctreeNode(OctreeNode* node)
    : SceneNode(), m_node{node}
{
    glGenVertexArrays(1, &m_vao);
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
            m_node->elements().data(), GL_FLOAT, 6,
            (int)m_node->elements().size(), GL_STATIC_DRAW);
        scene()->bufferCache().addItem(id(), buffer);
    }
    else
    {
        buffer = scene()->bufferCache().getItem(id()).value();
    }

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->buffer());

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                          (char*)0 + 0 * sizeof(GLfloat));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                          (char*)0 + 3 * sizeof(GLfloat));
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_POINTS, 0, buffer->elementCount());
    glBindVertexArray(0);
}
