#ifndef CAMERA_H
#define CAMERA_H

#include "scene.h"

#include <QKeyEvent>
#include <QMatrix4x4>
#include <QOpenGLFunctions>

class Camera : public SceneNode
{
public:
    using Ptr = std::shared_ptr<Camera>;

    Camera();

    void setViewNameInShader(const std::string& name, GLuint location) {
        m_viewNameInShader = name;
        m_viewLocation = location;
    }

    void setProjectionNameInShader(const std::string& name, GLuint location) {
        m_projectionNameInShader = name;
        m_projectionLocation = location;
    }

    void setViewportWidth(uint32_t width) {
        m_viewportWidth = width;
    }

    void setViewportHeight(uint32_t height) {
        m_viewportHeight = height;
    }

    void render() override;

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    GLuint m_viewLocation;
    GLuint m_projectionLocation;

    QVector3D m_position;
    QVector3D m_center;
    QVector3D m_up;

    QMatrix4x4 m_view;
    QMatrix4x4 m_projection;
    QMatrix4x4 m_rotation;

    uint32_t m_viewportWidth{0};
    uint32_t m_viewportHeight{0};

    std::string m_viewNameInShader;
    std::string m_projectionNameInShader;

    bool m_mouseDown;
    QPoint m_originalMousePosition;
};

#endif // CAMERA_H
