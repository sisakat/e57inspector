#ifndef SCENE_H
#define SCENE_H

#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_3_3_Core>
#include <QPainter>
#include <memory>
#include <optional>
#include <vector>

#include "boundingbox.h"
#include "openglarraybuffer.h"
#include "shader.h"
#include "silrucache.h"

class Scene; // forward declaration

class SceneNode : protected QOpenGLFunctions_3_3_Core
{
public:
    using Ptr = std::shared_ptr<SceneNode>;

    explicit SceneNode(SceneNode* parent = nullptr);
    ~SceneNode() override = default;

    virtual void render();
    virtual void render2D(QPainter& painter);
    void addChild(Ptr node);

    [[nodiscard]] uint32_t id() const;
    [[nodiscard]] std::vector<Ptr>& children();

    [[nodiscard]] const std::vector<Ptr>& children() const;
    [[nodiscard]] QMatrix4x4 pose() const;

    void setPose(const QMatrix4x4& newPose);

    [[nodiscard]] Scene* scene();
    [[nodiscard]] const Scene* scene() const;

    [[nodiscard]] SceneNode* parent();
    [[nodiscard]] const SceneNode* parent() const;

    [[nodiscard]] BoundingBox boundingBox() const;

    friend class Scene;

protected:
    uint32_t m_id;
    std::vector<Ptr> m_childNodes;
    QMatrix4x4 m_pose;
    Scene* m_scene;
    SceneNode* m_parent{nullptr};
    BoundingBox m_boundingBox{};

    void setScene(Scene* scene);
};

class Scene
{
public:
    using Ptr = std::shared_ptr<Scene>;
    using BufferCache =
        SiLRUCache<uint32_t, std::shared_ptr<OpenGLArrayBuffer>, size_t>;

    Scene();
    void render();
    void render2D(QPainter& painter);
    void addNode(SceneNode::Ptr node);
    void removeNode(SceneNode* node);

    [[nodiscard]] BufferCache& bufferCache();
    [[nodiscard]] const BufferCache& bufferCache() const;

    float getDepth(int u, int v);
    [[nodiscard]] std::optional<QVector3D>
    findDepth(int u, int v, int viewportX, int viewportY, int viewportWidth,
              int viewportHeight);

    [[nodiscard]] const Shader::Ptr& shader();
    void setShader(const Shader::Ptr& shader);

    [[nodiscard]] std::vector<SceneNode::Ptr>& nodes();
    [[nodiscard]] const std::vector<SceneNode::Ptr>& nodes() const;

    BoundingBox boundingBox() const;

    template <typename T> T* findNode()
    {
        for (const auto& node : m_nodes)
        {
            if (dynamic_cast<T*>(node.get()))
            {
                return dynamic_cast<T*>(node.get());
            }
        }
        return nullptr;
    }

private:
    BufferCache m_bufferCache;
    std::vector<SceneNode::Ptr> m_nodes;
    Shader::Ptr m_shader;
};

#endif // SCENE_H
