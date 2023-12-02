#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>
#include <QMatrix4x4>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_3_3_Core>

#include "openglarraybuffer.h"
#include "silrucache.h"

class Scene; // forward declaration

class SceneNode : protected QOpenGLFunctions_3_3_Core {
public:
    using Ptr = std::shared_ptr<SceneNode>;

    SceneNode();
    virtual ~SceneNode() = default;

    virtual void render();

    uint32_t id() {
        return m_id;
    }

    void addChild(Ptr node) {
        node->setScene(m_scene);
        m_childNodes.emplace_back(std::move(node));
    }

    std::vector<Ptr>& children() {
       return m_childNodes;
    }

    const std::vector<Ptr>& children() const {
       return m_childNodes;
    }

    QMatrix4x4 pose() const {
       return m_pose;
    }

    void setPose(const QMatrix4x4& newPose) {
       m_pose = newPose;
    }

    Scene* scene() {
       return m_scene;
    }

    const Scene* scene() const  {
       return m_scene;
    }

    friend class Scene;

protected:
    uint32_t m_id;
    std::vector<Ptr> m_childNodes;
    QMatrix4x4 m_pose;
    Scene* m_scene;

    void setScene(Scene* scene) {
       m_scene = scene;
       for (auto& child : m_childNodes) {
           child->setScene(scene);
       }
    }
};

class Scene {
public:
    using Ptr = std::shared_ptr<Scene>;
    using BufferCache = SiLRUCache<uint32_t, std::shared_ptr<OpenGLArrayBuffer>, size_t>;

    Scene();

    void addNode(SceneNode::Ptr node) {
       node->setScene(this);
       m_nodes.push_back(std::move(node));
    }

    void render();

    BufferCache& bufferCache() {
       return m_bufferCache;
    }

    const BufferCache& bufferCache() const {
       return m_bufferCache;
    }

private:
    BufferCache m_bufferCache;
    std::vector<SceneNode::Ptr> m_nodes;
};

#endif // SCENE_H