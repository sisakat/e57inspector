#include "scene.h"

#include <queue>
#include <set>
#include <tuple>

Scene::Scene() : m_bufferCache(1024) {}

void Scene::render()
{
    for (auto& child : m_nodes)
    {
        child->render();
    }
}

void Scene::render2D(QPainter& painter)
{
    for (auto& child : m_nodes)
    {
        child->render2D(painter);
    }
}

SceneNode::SceneNode(SceneNode* parent) : m_parent{parent}
{
    static uint32_t globalId = 0;
    m_id = globalId++;
    if (!initializeOpenGLFunctions())
    {
        throw std::runtime_error("Could not initialize OpenGL functions!");
    }
    m_pose.setToIdentity();
}

void SceneNode::render()
{
    scene()->shader()->setUniformMat4("model", pose().data());
}

void SceneNode::render2D(QPainter& painter) {}

void SceneNode::addChild(SceneNode::Ptr node)
{
    node->setScene(m_scene);
    m_childNodes.emplace_back(std::move(node));
}

QMatrix4x4 SceneNode::pose() const
{
    QMatrix4x4 result(m_pose);
    SceneNode* parent = m_parent;
    while (parent != nullptr)
    {
        result = parent->m_pose * result;
        parent = parent->parent();
    }
    return result;
}

void SceneNode::setScene(Scene* scene)
{
    m_scene = scene;
    for (auto& child : m_childNodes)
    {
        child->setScene(scene);
    }
}

std::vector<SceneNode::Ptr>& SceneNode::children()
{
    return m_childNodes;
}

const std::vector<SceneNode::Ptr>& SceneNode::children() const
{
    return m_childNodes;
}

void SceneNode::setPose(const QMatrix4x4& newPose)
{
    m_pose = newPose;
}

Scene* SceneNode::scene()
{
    return m_scene;
}

const Scene* SceneNode::scene() const
{
    return m_scene;
}

SceneNode* SceneNode::parent()
{
    return m_parent;
}

const SceneNode* SceneNode::parent() const
{
    return m_parent;
}

uint32_t SceneNode::id() const
{
    return m_id;
}

BoundingBox SceneNode::boundingBox() const
{
    BoundingBox result{m_boundingBox};
    for (const auto& child : m_childNodes)
    {
        auto bb = child->boundingBox();
        bb.transform(m_pose);
        result = result.combine(bb);
    }
    return result;
}

float Scene::getDepth(int u, int v)
{
    GLfloat depth = 0.0;
    glReadPixels(u, v, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
    if ((depth > 0.0) && (depth < 1.0))
    {
        return depth;
    }
    return -1;
}

std::optional<QVector3D> Scene::findDepth(int u, int v, int viewportX,
                                          int viewportY, int viewportWidth,
                                          int viewportHeight)
{

    std::set<std::tuple<int, int>> visited;
    std::queue<std::tuple<int, int>> pixels;
    pixels.emplace(u, v);

    float depth = getDepth(u, v);
    if (depth > 0.0)
    {
        return QVector3D{static_cast<float>(u), static_cast<float>(v), depth};
    }

    std::vector<float> depthBuffer(viewportWidth * viewportHeight);
    glReadPixels(viewportX, viewportY, viewportWidth, viewportHeight,
                 GL_DEPTH_COMPONENT, GL_FLOAT, depthBuffer.data());

    while (!pixels.empty())
    {
        auto pixel = pixels.front();
        pixels.pop();

        depth = depthBuffer.at(std::get<1>(pixel) * viewportWidth +
                               std::get<0>(pixel));
        if (depth > 0.0f && depth < 1.0f)
        {
            return QVector3D{static_cast<float>(std::get<0>(pixel)),
                             static_cast<float>(std::get<1>(pixel)), depth};
        }

        for (int i = -1; i <= 1; ++i)
        {
            for (int j = -1; j <= 1; ++j)
            {
                if (i == 0 && j == 0)
                    continue;
                int newU = std::get<0>(pixel) + i;
                int newV = std::get<1>(pixel) + j;
                std::tuple<int, int> newPixel(newU, newV);
                if (!visited.contains(newPixel) && newU >= 0 &&
                    newU < viewportWidth && newV >= 0 && newV < viewportHeight)
                {
                    visited.insert(newPixel);
                    pixels.push(std::move(newPixel));
                }
            }
        }
    }

    return std::nullopt;
}

void Scene::addNode(SceneNode::Ptr node)
{
    node->setScene(this);
    node->m_parent = nullptr;
    m_nodes.push_back(std::move(node));
}

Scene::BufferCache& Scene::bufferCache()
{
    return m_bufferCache;
}

const Scene::BufferCache& Scene::bufferCache() const
{
    return m_bufferCache;
}

const Shader::Ptr& Scene::shader()
{
    return m_shader;
}

void Scene::setShader(const Shader::Ptr& shader)
{
    m_shader = shader;
}

std::vector<SceneNode::Ptr>& Scene::nodes()
{
    return m_nodes;
}

const std::vector<SceneNode::Ptr>& Scene::nodes() const
{
    return m_nodes;
}

BoundingBox Scene::boundingBox() const
{
    BoundingBox result;
    result.setToInfinite();
    for (const auto& node : m_nodes)
    {
        result = result.combine(node->boundingBox());
    }
    return result;
}

void Scene::removeNode(SceneNode* node)
{
    std::queue<SceneNode*> nodes;

    std::erase_if(m_nodes, [node](auto& ptr) { return ptr.get() == node; });
}
