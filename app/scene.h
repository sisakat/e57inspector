#ifndef SCENE_H
#define SCENE_H

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
    virtual void configureShader();

    void addChild(Ptr node);

    [[nodiscard]] uint32_t id() const;
    [[nodiscard]] std::vector<Ptr>& children();

    [[nodiscard]] const std::string& name() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

    [[nodiscard]] const std::vector<Ptr>& children() const;
    [[nodiscard]] Matrix4d modelMatrix() const;
    [[nodiscard]] Matrix4d pose() const;

    void setPose(const Matrix4d& newPose);

    [[nodiscard]] Scene* scene();
    [[nodiscard]] const Scene* scene() const;

    [[nodiscard]] SceneNode* parent();
    [[nodiscard]] const SceneNode* parent() const;

    [[nodiscard]] BoundingBox boundingBox() const;

    std::optional<int> getCurrentShaderProgram();
    std::optional<int> getUniformLocation(const std::string& name);

    [[nodiscard]] bool transparent() const;

    friend class Scene;

protected:
    uint32_t m_id;
    std::string m_name;
    std::vector<Ptr> m_childNodes;
    Scene* m_scene;
    SceneNode* m_parent{nullptr};
    BoundingBox m_boundingBox{};
    bool m_transparent{false};

    void setScene(Scene* scene);

private:
    Matrix4d m_pose{IdentityMatrix4d};
};

class Scene : public QObject
{
    Q_OBJECT
public:
    using Ptr = std::shared_ptr<Scene>;
    using BufferCache =
        SiLRUCache<uint32_t, std::shared_ptr<OpenGLArrayBuffer>, size_t>;

    Scene();
    void render();
    void render2D(QPainter& painter);
    void addNode(SceneNode::Ptr node);
    void removeNode(SceneNode* node);

    bool invokeAgain() const { return m_invokeAgain; }
    void setInvokeAgain(bool invokeAgain) { m_invokeAgain = invokeAgain; }
    
    [[nodiscard]] BufferCache& bufferCache();
    [[nodiscard]] const BufferCache& bufferCache() const;

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

    void setDevicePixelRatio(float value) { m_devicePixelRatio = value; }
    [[nodiscard]] float devicePixelRatio() const { return m_devicePixelRatio; }

    const Matrix4d& getPose() const;
    void setPose(const Matrix4d& pose);

signals:
    void update();

private:
    BufferCache m_bufferCache;
    std::vector<SceneNode::Ptr> m_nodes;
    Matrix4d m_pose{IdentityMatrix4d};

    bool m_invokeAgain{false};

    float m_devicePixelRatio{1.0};
};

#endif // SCENE_H
