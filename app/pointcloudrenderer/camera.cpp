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

void Camera::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MouseButton::LeftButton)
    {
        m_mouseDown = true;
        m_originalMousePosition = event->pos();
    }
}

void Camera::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MouseButton::LeftButton)
    {
        m_mouseDown = false;
        m_originalMousePosition = event->pos();
    }
}

void Camera::mouseMoveEvent(QMouseEvent* event)
{
    if (m_mouseDown)
    {
        auto delta = event->pos() - m_originalMousePosition;
        auto deltaVec = QVector2D(delta.x(), delta.y());
        auto viewVec = (m_position - m_center).normalized();
        auto up = m_up;
        auto angle = QVector3D::dotProduct(viewVec, m_up);

        // prevent view vector to align with up vector
        if (std::abs(angle) > 0.95)
        {
            if (angle < 0 && deltaVec.y() > 0)
            {
                deltaVec.setY(0);
            }
            else if (angle > 0 && deltaVec.y() < 0)
            {
                deltaVec.setY(0);
            }
        }

        auto rotVec = QVector3D::crossProduct(up, viewVec);

        const QVector3D zAxis(0.0f, 0.0f, 1.0f);

        QMatrix4x4 transformation;
        transformation.setToIdentity();
        transformation.rotate(deltaVec.x(), zAxis);
        transformation.rotate(deltaVec.y(), rotVec);

        m_position = (transformation * m_position.toVector4D()).toVector3D();
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
