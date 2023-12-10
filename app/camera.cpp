#include "camera.h"

#include <array>

const float ZOOM_FRAC = 0.25;

Camera::Camera()
{
    initializeOpenGLFunctions();

    m_view.setToIdentity();
    m_projection.setToIdentity();

    m_position = QVector3D(1.0f, 0.0f, 0.0f);
    m_center = QVector3D(0.0f, 0.0f, 0.0f);
    m_up = QVector3D(0.0f, 0.0f, 1.0f);
}

void Camera::render()
{
    SceneNode::render();
    glViewport(0, 0, m_viewportWidth, m_viewportHeight);

    m_view.setToIdentity();
    m_view.lookAt(m_position, m_center, m_up);

    updateNearFar();
    m_projection.setToIdentity();
    m_projection.perspective(m_fieldOfView / 2.0f,
                             1.0f * m_viewportWidth / m_viewportHeight, m_near,
                             m_far);

    scene()->shader()->setUniformMat4("view", m_view.data());
    scene()->shader()->setUniformMat4("projection", m_projection.data());
}

void Camera::render2D(QPainter& painter)
{
    auto pp = pickpoint();
    const int x = static_cast<int>(pp.x() - 2.0 * scene()->devicePixelRatio());
    const int y = static_cast<int>(pp.y() - 2.0 * scene()->devicePixelRatio());
    const int d = static_cast<int>(4 * scene()->devicePixelRatio());
    painter.fillRect(x, y, d, d, Qt::red);
}

void Camera::yaw(float angle)
{
    const QVector3D zAxis(0.0f, 0.0f, 1.0f);

    QMatrix4x4 transformation;
    transformation.setToIdentity();
    transformation.translate(m_pickpoint.toVector3D());
    transformation.rotate(angle, zAxis);
    transformation.translate(-1.0f * m_pickpoint.toVector3D());

    QVector4D position(m_position);
    position.setW(1.0f);
    position = transformation * position;
    m_position = position.toVector3D();

    QVector4D center(m_center);
    center.setW(1.0f);
    center = transformation * center;
    m_center = center.toVector3D();
}

void Camera::pitch(float angle)
{
    auto viewVec = (m_position - m_center).normalized();
    auto cosViewUp = QVector3D::dotProduct(viewVec, m_up);

    // prevent view vector to align with up vector
    if (std::abs(cosViewUp) > 0.98)
    {
        if (cosViewUp < 0.0f && angle > 0.0f)
        {
            angle = 0.0f;
        }
        else if (cosViewUp > 0.0f && angle < 0.0f)
        {
            angle = 0.0f;
        }
    }

    QMatrix4x4 transformation;
    transformation.setToIdentity();
    transformation.translate(m_pickpoint.toVector3D());
    transformation.rotate(angle, QVector3D::crossProduct(m_up, viewVec));
    transformation.translate(-1.0f * m_pickpoint.toVector3D());

    QVector4D position(m_position);
    position.setW(1.0f);
    position = transformation * position;
    m_position = position.toVector3D();

    QVector4D center(m_center);
    center.setW(1.0f);
    center = transformation * center;
    m_center = center.toVector3D();
}

void Camera::updatePickpoint(const QPoint& window)
{
    auto u = window.x();
    auto v = m_viewportHeight - 1 - window.y();
    auto depth = scene()->findDepth(u, v, m_viewportX, m_viewportY,
                                    m_viewportWidth, m_viewportHeight);
    if (depth)
    {
        m_pickpoint = unproject(depth.value());
    }
}

QVector4D Camera::unproject(const QVector3D& window) const
{
    QVector4D result;
    result.setX((window.x() - static_cast<float>(m_viewportX)) /
                    static_cast<float>(m_viewportWidth) * 2.0f -
                1.0f);
    result.setY((window.y() - static_cast<float>(m_viewportY)) /
                    static_cast<float>(m_viewportHeight) * 2.0f -
                1.0f);
    result.setZ(window.z() * 2.0f - 1.0f);
    result.setW(1.0f);

    result = m_projection.inverted() * result;
    result /= result.w();
    result = m_view.inverted() * result;
    return result;
}

QVector3D Camera::project(const QVector4D& object) const
{
    QVector4D result(object);
    result.setW(1.0f);
    result = m_view * object;
    result = m_projection * result;
    result /= result.w();

    result.setX(((result.x() + 1.0f) / 2.0f + static_cast<float>(m_viewportX)) *
                static_cast<float>(m_viewportWidth));
    result.setY(((result.y() + 1.0f) / 2.0f + static_cast<float>(m_viewportY)) *
                static_cast<float>(m_viewportHeight));
    result.setZ((result.z() + 1.0f) / 2.0f);
    return result.toVector3D();
}

void Camera::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MouseButton::LeftButton)
    {
        m_mouseDown = true;
    }
    if (event->button() == Qt::MouseButton::RightButton)
    {
        m_zooming = true;
    }
    if (event->button() == Qt::MouseButton::MiddleButton)
    {
        m_panning = true;
    }

    m_originalMousePosition = event->pos();
    updatePickpoint(event->pos());
}

void Camera::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MouseButton::LeftButton)
    {
        m_mouseDown = false;
    }
    if (event->button() == Qt::MouseButton::RightButton)
    {
        m_zooming = false;
    }
    if (event->button() == Qt::MouseButton::MiddleButton)
    {
        m_panning = false;
    }

    m_originalMousePosition = event->pos();
}

void Camera::mouseMoveEvent(QMouseEvent* event)
{
    if (m_mouseDown)
    {
        auto delta = event->pos() - m_originalMousePosition;
        delta *= scene()->devicePixelRatio();
        pitch(static_cast<float>(-delta.y()));
        yaw(static_cast<float>(-delta.x()));
    }

    if (m_zooming)
    {
        auto deltaY = event->pos().y() - m_originalMousePosition.y();
        auto viewDir = (m_position - m_pickpoint.toVector3D()).normalized();
        auto lengthToTarget = (m_position - m_pickpoint.toVector3D()).length();
        if (deltaY < 0)
        {
            m_position -= lengthToTarget * viewDir * ZOOM_FRAC * 0.5;
            m_center -= lengthToTarget * viewDir * ZOOM_FRAC * 0.5;
        }
        else if (deltaY > 0)
        {
            m_position += lengthToTarget * viewDir * ZOOM_FRAC * 0.5;
            m_center += lengthToTarget * viewDir * ZOOM_FRAC * 0.5;
        }
    }

    if (m_panning)
    {
        uint32_t u = event->pos().x();
        uint32_t v = m_viewportHeight - 1 - event->pos().y();

        uint32_t uOriginal = m_originalMousePosition.x();
        uint32_t vOriginal = m_viewportHeight - 1 - m_originalMousePosition.y();

        auto projectedPickpoint = project(m_pickpoint);
        QVector3D currentMousePos(static_cast<float>(u), static_cast<float>(v),
                                  projectedPickpoint.z());
        QVector3D originalMousePos(static_cast<float>(uOriginal),
                                   static_cast<float>(vOriginal),
                                   projectedPickpoint.z());
        QVector4D currentMousePosObject = unproject(currentMousePos);
        QVector4D originalMousePosObject = unproject(originalMousePos);
        QVector3D delta =
            (currentMousePosObject - originalMousePosObject).toVector3D();

        m_position -= delta;
        m_center -= delta;
    }

    m_originalMousePosition = event->pos();
}

void Camera::wheelEvent(QWheelEvent* event)
{
    updatePickpoint(event->position().toPoint());
    auto viewDir = (m_position - m_pickpoint.toVector3D()).normalized();
    auto lengthToTarget = (m_position - m_pickpoint.toVector3D()).length();
    if (event->angleDelta().y() > 0)
    {
        m_position -= lengthToTarget * viewDir * ZOOM_FRAC;
        m_center -= lengthToTarget * viewDir * ZOOM_FRAC;
    }
    else if (event->angleDelta().y() < 0)
    {
        m_position += lengthToTarget * viewDir * ZOOM_FRAC;
        m_center += lengthToTarget * viewDir * ZOOM_FRAC;
    }
}

void Camera::keyPressEvent(QKeyEvent* event) {}

void Camera::keyReleaseEvent(QKeyEvent* event) {}

void Camera::updateNearFar()
{
    auto bb = scene()->boundingBox();
    auto bbPoints = bb.points();
    float maxLength = std::numeric_limits<float>::min();
    float minLength = std::numeric_limits<float>::max();
    for (const auto& point : bbPoints)
    {
        auto dot = QVector3D::dotProduct(point - m_position,
                                         (m_center - m_position).normalized());
        float length = std::abs(dot);
        if (length > maxLength)
        {
            maxLength = length;
        }
        if (dot < minLength)
        {
            minLength = dot;
        }
    }

    m_near = minLength < 0.001f ? 0.001f : minLength;
    m_far = maxLength < 0.0f ? 100.0f : maxLength;
}

QPoint Camera::pickpoint() const
{
    auto projectedPickpoint = project(m_pickpoint);
    return {static_cast<int>(projectedPickpoint.x()),
            static_cast<int>(m_viewportHeight - 1 -
                             static_cast<uint32_t>(projectedPickpoint.y()))};
}
