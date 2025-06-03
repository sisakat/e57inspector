#include "scene.h"

#include <queue>
#include <tuple>

Scene::Scene() : m_bufferCache(1024) {}

void Scene::render()
{
    for (auto& child : m_nodes)
    {
        if (!child->transparent())
        {
            child->render();
        }
    }
    for (auto& child : m_nodes)
    {
        if (child->transparent())
        {
            child->render();
        }
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
}

void SceneNode::render()
{
    configureShader();
}

void SceneNode::render2D(QPainter& painter) {}

void SceneNode::configureShader()
{
    if (auto location = getUniformLocation("model"))
    {
        glUniformMatrix4fv(location.value(), 1, GL_FALSE, &modelMatrix()[0][0]);
    }
}

void SceneNode::addChild(SceneNode::Ptr node)
{
    node->setScene(m_scene);
    m_childNodes.emplace_back(std::move(node));
}

Matrix4d SceneNode::modelMatrix() const
{
    Matrix4d result(m_pose);
    SceneNode* parent = m_parent;
    while (parent != nullptr)
    {
        result = parent->m_pose * result;
        parent = parent->parent();
    }
    result = scene()->getPose() * result;
    return result;
}

Matrix4d SceneNode::pose() const
{
    return m_pose;
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

void SceneNode::setPose(const Matrix4d& newPose)
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
    BoundingBox result = m_boundingBox;
    for (const auto& child : m_childNodes)
    {
        auto bb = child->boundingBox().transform(child->pose());
        result = result.combine(bb);
    }
    return result;
}

std::optional<int> SceneNode::getCurrentShaderProgram()
{
    GLint program = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);

    if (program == 0)
    {
        return std::nullopt;
    }

    return program;
}

std::optional<int> SceneNode::getUniformLocation(const std::string& name)
{
    auto shaderProgram = getCurrentShaderProgram();
    if (!shaderProgram)
        return std::nullopt;

    GLint location = glGetUniformLocation(shaderProgram.value(), name.c_str());
    if (location < 0)
    {
        return std::nullopt;
    }
    return location;
}

bool SceneNode::transparent() const
{
    return m_transparent;
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
    for (const auto& node : m_nodes)
    {
        result = result.combine(node->boundingBox().transform(node->pose()));
    }
    result = result.transform(getPose());
    return result;
}

void Scene::removeNode(SceneNode* node)
{
    std::queue<SceneNode*> nodes;

    std::erase_if(m_nodes, [node](auto& ptr) { return ptr.get() == node; });
}

const Matrix4d& Scene::getPose() const
{
    return m_pose;
}

void Scene::setPose(const Matrix4d& pose)
{
    m_pose = pose;
}
