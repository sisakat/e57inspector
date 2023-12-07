#ifndef SCENE_H
#define SCENE_H

#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_3_3_Core>
#include <memory>
#include <optional>
#include <vector>

#include "openglarraybuffer.h"
#include "shader.h"
#include "silrucache.h"

class Scene; // forward declaration

class SceneNode : protected QOpenGLFunctions_3_3_Core
{
public:
    using Ptr = std::shared_ptr<SceneNode>;

    explicit SceneNode(SceneNode* parent = nullptr);
    virtual ~SceneNode() = default;

    virtual void render();

    uint32_t id() { return m_id; }

    void addChild(Ptr node)
    {
        node->setScene(m_scene);
        m_childNodes.emplace_back(std::move(node));
    }

    std::vector<Ptr>& children() { return m_childNodes; }

    const std::vector<Ptr>& children() const { return m_childNodes; }

    [[nodiscard]] QMatrix4x4 pose() const
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

    void setPose(const QMatrix4x4& newPose) { m_pose = newPose; }

    Scene* scene() { return m_scene; }

    const Scene* scene() const { return m_scene; }

    const SceneNode* parent() const { return m_parent; }
    SceneNode* parent() { return m_parent; }

    friend class Scene;

protected:
    uint32_t m_id;
    std::vector<Ptr> m_childNodes;
    QMatrix4x4 m_pose;
    Scene* m_scene;

    void setScene(Scene* scene)
    {
        m_scene = scene;
        for (auto& child : m_childNodes)
        {
            child->setScene(scene);
        }
    }

    SceneNode* m_parent{nullptr};
};

class Scene
{
public:
    using Ptr = std::shared_ptr<Scene>;
    using BufferCache =
        SiLRUCache<uint32_t, std::shared_ptr<OpenGLArrayBuffer>, size_t>;

    Scene();

    void addNode(SceneNode::Ptr node)
    {
        node->setScene(this);
        node->m_parent = nullptr;
        m_nodes.push_back(std::move(node));
    }

    void render();

    BufferCache& bufferCache() { return m_bufferCache; }

    const BufferCache& bufferCache() const { return m_bufferCache; }

    float getDepth(int u, int v);

    std::optional<QVector3D> findDepth(int u, int v, int viewportX,
                                       int viewportY, int viewportWidth,
                                       int viewportHeight);

    const Shader::Ptr& shader() { return m_shader; }
    void setShader(const Shader::Ptr& shader) { m_shader = shader; }

    std::vector<SceneNode::Ptr>& nodes() { return m_nodes; }
    const std::vector<SceneNode::Ptr>& nodes() const { return m_nodes; }

private:
    BufferCache m_bufferCache;
    std::vector<SceneNode::Ptr> m_nodes;
    Shader::Ptr m_shader;
};

#endif // SCENE_H
