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

    void setViewportWidth(uint32_t width) { m_viewportWidth = width; }
    void setViewportHeight(uint32_t height) { m_viewportHeight = height; }
    [[nodiscard]] uint32_t viewportWidth() const { return m_viewportWidth; }
    [[nodiscard]] uint32_t viewportHeight() const { return m_viewportHeight; }
    [[nodiscard]] float fieldOfView() const { return m_fieldOfView; }
    void setFieldOfView(float fieldOfView) { m_fieldOfView = fieldOfView; }

    void render() override;

    void yaw(float angle);
    void pitch(float angle);

    void updatePickpoint(const QPoint& window);

    [[nodiscard]] QVector4D unproject(const QVector3D& window) const;
    [[nodiscard]] QVector3D project(const QVector4D& object) const;

    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);

private:
    QVector3D m_position;
    QVector3D m_center;
    QVector3D m_up;
    QVector4D m_pickpoint;

    QMatrix4x4 m_view;
    QMatrix4x4 m_projection;

    uint32_t m_viewportX{0};
    uint32_t m_viewportY{0};
    uint32_t m_viewportWidth{0};
    uint32_t m_viewportHeight{0};

    bool m_mouseDown{false};
    bool m_panning{false};
    bool m_zooming{false};
    QPoint m_originalMousePosition;

    float m_fieldOfView{90.0};
};

#endif // CAMERA_H
