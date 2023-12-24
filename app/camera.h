#ifndef CAMERA_H
#define CAMERA_H

#include "geometry.h"
#include "scene.h"

#include <QKeyEvent>
#include <QOpenGLFunctions>
#include <QPainter>

class Camera : public SceneNode
{
public:
    using Ptr = std::shared_ptr<Camera>;

    Camera();

    void setViewportWidth(uint32_t width) { m_viewportWidth = width; }
    void setViewportHeight(uint32_t height) { m_viewportHeight = height; }
    uint32_t viewportWidth() const { return m_viewportWidth; }
    uint32_t viewportHeight() const { return m_viewportHeight; }
    float fieldOfView() const { return m_fieldOfView; }
    void setFieldOfView(float fieldOfView) { m_fieldOfView = fieldOfView; }

    void render() override;
    void render2D(QPainter& painter) override;
    void renderBoundingBox(QPainter& painter, const BoundingBox& boundingBox);
    void configureShader() override;

    void yaw(float angle);
    void pitch(float angle);

    Vector2d pickpoint() const;
    void updatePickpoint(const Vector2d& window);

    Vector4d unproject(const Vector3d& window) const;
    Vector3d project(const Vector4d& object) const;

    float getDepth(int u, int v);
    std::optional<Vector3d> findDepth(int u, int v);

    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);

    bool constrained() const { return m_constrainedCamera; }
    void setConstrained(bool constrained)
    {
        m_constrainedCamera = constrained;
        m_position = NullPoint4d;
        m_center = Vector4d(X_AXIS, 1.0f);
        m_up = Vector4d(Z_AXIS, 0.0f);
    }

    void topView();

protected:
    void updateNearFar();

private:
    Vector4d m_position{0.0f, 0.0f, 0.0f, 1.0f};
    Vector4d m_center{1.0f, 0.0f, 0.0f, 1.0f};
    Vector4d m_up{0.0f, 0.0f, 1.0f, 0.0f};
    Vector4d m_pickpoint{0.0f, 0.0f, 0.0f, 1.0f};

    Matrix4d m_view;
    Matrix4d m_projection;

    int m_viewportX{0};
    int m_viewportY{0};
    int m_viewportWidth{0};
    int m_viewportHeight{0};

    bool m_mouseDown{false};
    bool m_panning{false};
    bool m_zooming{false};
    Vector2d m_originalMousePosition;

    float m_fieldOfView{45.0};
    float m_near{0.001f};
    float m_far{1000.0f};

    bool m_constrainedCamera{true};
};

#endif // CAMERA_H
