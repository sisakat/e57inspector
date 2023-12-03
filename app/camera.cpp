#include "camera.h"

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

    m_projection.setToIdentity();
    m_projection.perspective(45, 1.0f * m_viewportWidth / m_viewportHeight,
                             0.001f, 1000.0f);

    glUniformMatrix4fv(m_viewLocation, 1, GL_FALSE, m_view.data());
    glUniformMatrix4fv(m_projectionLocation, 1, GL_FALSE, m_projection.data());
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

void Camera::translateRight(float value)
{
    auto viewVec = (m_position - m_center).normalized();
    auto right = QVector3D::crossProduct(viewVec, m_up);
    m_center += right * value;
    m_position += right * value;
}

void Camera::translateUp(float value)
{
    m_center += m_up * value;
    m_position += m_up * value;
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
        m_pickpoint -= delta.toVector4D();
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
