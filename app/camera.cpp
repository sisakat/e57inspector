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
    transformation.rotate(angle, zAxis);
    m_position = (transformation * m_position.toVector4D()).toVector3D();
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
    transformation.rotate(angle, QVector3D::crossProduct(m_up, viewVec));
    m_position = (transformation * m_position.toVector4D()).toVector3D();
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
        m_originalMousePosition = event->pos();
    }
    else if (event->button() == Qt::MouseButton::MiddleButton)
    {
        m_panning = true;
        m_originalMousePosition = event->pos();
    }

    int u = event->pos().x();
    int v = static_cast<int>(m_viewportHeight) - 1 - event->pos().y();
    auto depth = scene()->findDepth(u, v);
    if (depth)
    {
        auto objectSpace = unproject(*depth);
        auto windowSpace = project(objectSpace);
        int x = 3;
        qDebug() << "";
    }
}

void Camera::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MouseButton::LeftButton)
    {
        m_mouseDown = false;
        m_originalMousePosition = event->pos();
    }
    else if (event->button() == Qt::MouseButton::MiddleButton)
    {
        m_panning = false;
        m_originalMousePosition = event->pos();
    }
}

void Camera::mouseMoveEvent(QMouseEvent* event)
{
    if (m_mouseDown)
    {
        auto delta = event->pos() - m_originalMousePosition;
        pitch(static_cast<float>(delta.y()));
        yaw(static_cast<float>(delta.x()));
        m_originalMousePosition = event->pos();
    }
    else if (m_panning)
    {
        auto delta = event->pos() - m_originalMousePosition;
        translateRight(0.5f * delta.x());
        translateUp(0.2f * delta.y());
        m_originalMousePosition = event->pos();
    }
}

void Camera::wheelEvent(QWheelEvent* event)
{
    auto viewDir = (m_center - m_position).normalized();
    auto lengthToTarget = (m_center - m_position).length();
    if (event->angleDelta().y() > 0)
    {
        m_position += lengthToTarget * viewDir * ZOOM_FRAC;
    }
    else if (event->angleDelta().y() < 0)
    {
        m_position += -1.0 * lengthToTarget * viewDir * ZOOM_FRAC;
    }
}

void Camera::keyPressEvent(QKeyEvent* event) {}

void Camera::keyReleaseEvent(QKeyEvent* event) {}
