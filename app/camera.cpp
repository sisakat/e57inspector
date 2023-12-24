#include "camera.h"

#include <array>
#include <queue>
#include <set>

const float ZOOM_FRAC = 0.25;
const float WHEEL_ZOOM_FRAC = 0.25;

Camera::Camera()
{
    initializeOpenGLFunctions();
}

void Camera::render()
{
    if (m_topView)
    {
        topView();
    }
    SceneNode::render();
    configureShader();
}

void Camera::render2D(QPainter& painter)
{
    if (m_pickpointNavigation)
    {
        auto pp = pickpoint();
        const int x =
            static_cast<int>(pp.x - 2.0 * scene()->devicePixelRatio());
        const int y =
            static_cast<int>(pp.y - 2.0 * scene()->devicePixelRatio());
        const int d = static_cast<int>(4 * scene()->devicePixelRatio());
        painter.fillRect(x, y, d, d, Qt::red);
    }
}

void Camera::renderBoundingBox(QPainter& painter,
                               const BoundingBox& boundingBox)
{
    auto toWindow = [this](const Vector3d& window) {
        return Vector3d{window.x, m_viewportHeight - 1 - window.y, window.z};
    };

    auto bbPoints = boundingBox.points();

    for (const auto& point : bbPoints)
    {
        auto pointScreen = toWindow(project(Vector4d(point, 1.0f)));
        painter.fillRect(pointScreen.x - 5, pointScreen.y - 5, 5, 5, Qt::blue);
    }
}

void Camera::configureShader()
{
    m_view =
        glm::lookAt(Vector3d(m_position), Vector3d(m_center), Vector3d(m_up));
    updateNearFar();
    m_projection = glm::perspective(deg2rad(m_fieldOfView),
                                    1.0f * m_viewportWidth / m_viewportHeight,
                                    m_near, m_far);

    if (auto location = getUniformLocation("view"))
    {
        glUniformMatrix4fv(location.value(), 1, GL_FALSE, &m_view[0][0]);
    }

    if (auto location = getUniformLocation("projection"))
    {
        glUniformMatrix4fv(location.value(), 1, GL_FALSE, &m_projection[0][0]);
    }
}
void Camera::yaw(float angle)
{
    Vector3d rotationAxis = m_constrainedCamera ? Z_AXIS : Vector3d(m_up);
    Matrix4d transformation = TranslationMatrix(m_pickpoint) *
                              RotationMatrix(angle, rotationAxis) *
                              TranslationMatrix(VectorNegate(m_pickpoint));
    m_position = transformation * m_position;
    m_center = transformation * m_center;
    m_up = transformation * m_up;
}

void Camera::pitch(float angle)
{
    Vector3d viewVec = VectorNormalize(m_position - m_center);
    Matrix4d transformation =
        TranslationMatrix(m_pickpoint) *
        RotationMatrix(angle, VectorCross(Vector3d(m_up), viewVec)) *
        TranslationMatrix(VectorNegate(m_pickpoint));
    Vector3d newUp = transformation * m_up;
    if (!m_constrainedCamera || VectorDot(Z_AXIS, newUp) >= 0.0f)
    {
        m_position = transformation * m_position;
        m_center = transformation * m_center;
        m_up = Vector4d(newUp, 0.0f);
    }
}

void Camera::updatePickpoint(const Vector2d& window)
{
    if (!m_pickpointNavigation)
    {
        m_pickpoint = m_position;
        return;
    }

    auto u = window.x;
    auto v = m_viewportHeight - 1 - window.y;
    auto depth = findDepth(u, v);
    if (depth)
    {
        m_pickpoint = unproject(depth.value());
    }
}

Vector4d Camera::unproject(const Vector3d& window) const
{
    Vector4d result;
    result.x = (window.x - static_cast<float>(m_viewportX)) /
                   static_cast<float>(m_viewportWidth) * 2.0f -
               1.0f;
    result.y = (window.y - static_cast<float>(m_viewportY)) /
                   static_cast<float>(m_viewportHeight) * 2.0f -
               1.0f;
    result.z = window.z * 2.0f - 1.0f;
    result.w = 1.0f;

    result = InverseMatrix(m_projection) * result;
    result = InverseMatrix(m_view) * result;
    result /= result.w;

    return result;
}

Vector3d Camera::project(const Vector4d& object) const
{
    Vector4d result(object);
    result.w = 1.0f;

    result = m_view * object;
    result = m_projection * result;
    result /= result.w;

    result.x = ((result.x + 1.0f) / 2.0f + static_cast<float>(m_viewportX)) *
               static_cast<float>(m_viewportWidth);
    result.y = ((result.y + 1.0f) / 2.0f + static_cast<float>(m_viewportY)) *
               static_cast<float>(m_viewportHeight);
    result.z = (result.z + 1.0f) / 2.0f;
    return result;
}

float Camera::getDepth(int u, int v)
{
    GLfloat depth = 0.0;
    glReadPixels(u, v, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
    if ((depth > 0.0) && (depth < 1.0))
    {
        return depth;
    }
    return -1;
}

std::optional<Vector3d> Camera::findDepth(int u, int v)
{
    std::set<std::tuple<int, int>> visited;
    std::queue<std::tuple<int, int>> pixels;
    pixels.emplace(u, v);

    float depth = getDepth(u, v);
    if (depth > 0.0)
    {
        return Vector3d{static_cast<float>(u), static_cast<float>(v), depth};
    }

    std::vector<float> depthBuffer(m_viewportWidth * m_viewportHeight);
    glReadPixels(m_viewportX, m_viewportY, m_viewportWidth, m_viewportHeight,
                 GL_DEPTH_COMPONENT, GL_FLOAT, depthBuffer.data());

    while (!pixels.empty())
    {
        auto pixel = pixels.front();
        pixels.pop();

        depth = depthBuffer.at(std::get<1>(pixel) * m_viewportWidth +
                               std::get<0>(pixel));
        if (depth > 0.0f && depth < 1.0f)
        {
            return Vector3d{static_cast<float>(std::get<0>(pixel)),
                            static_cast<float>(std::get<1>(pixel)), depth};
        }

        for (int i = -1; i <= 1; ++i)
        {
            for (int j = -1; j <= 1; ++j)
            {
                if (i == 0 && j == 0)
                    continue;
                int newU = std::get<0>(pixel) + i;
                int newV = std::get<1>(pixel) + j;
                std::tuple<int, int> newPixel(newU, newV);
                if (visited.find(newPixel) == visited.end() && newU >= 0 &&
                    newU < m_viewportWidth && newV >= 0 &&
                    newV < m_viewportHeight)
                {
                    visited.insert(newPixel);
                    pixels.push(std::move(newPixel));
                }
            }
        }
    }

    return std::nullopt;
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

    m_originalMousePosition.x = event->pos().x();
    m_originalMousePosition.y = event->pos().y();
    updatePickpoint(m_originalMousePosition);
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

    m_originalMousePosition.x = event->pos().x();
    m_originalMousePosition.y = event->pos().y();
}

void Camera::mouseMoveEvent(QMouseEvent* event)
{
    Vector2d pos(event->pos().x(), event->pos().y());

    if (m_mouseDown)
    {
        auto delta = pos - m_originalMousePosition;
        pitch(deg2rad(static_cast<float>(-delta.y)));
        yaw(deg2rad(static_cast<float>(-delta.x)));
    }

    if (m_zooming && m_pickpointNavigation)
    {
        auto deltaY = pos.y - m_originalMousePosition.y;
        auto viewDir = VectorNormalize(m_position - m_pickpoint);
        auto lengthToTarget = VectorLength(m_position - m_pickpoint);
        if (deltaY < 0)
        {
            m_position -= lengthToTarget * viewDir * ZOOM_FRAC * 0.5f;
            m_center -= lengthToTarget * viewDir * ZOOM_FRAC * 0.5f;
        }
        else if (deltaY > 0)
        {
            m_position += lengthToTarget * viewDir * ZOOM_FRAC * 0.5f;
            m_center += lengthToTarget * viewDir * ZOOM_FRAC * 0.5f;
        }
    }

    if (m_panning && m_pickpointNavigation)
    {
        int u = event->pos().x();
        int v = m_viewportHeight - 1 - event->pos().y();

        int uOriginal = m_originalMousePosition.x;
        int vOriginal = m_viewportHeight - 1 - m_originalMousePosition.y;

        auto projectedPickpoint = project(m_pickpoint);
        Vector3d currentMousePos(static_cast<float>(u), static_cast<float>(v),
                                 projectedPickpoint.z);
        Vector3d originalMousePos(static_cast<float>(uOriginal),
                                  static_cast<float>(vOriginal),
                                  projectedPickpoint.z);
        Vector4d currentMousePosObject = unproject(currentMousePos);
        Vector4d originalMousePosObject = unproject(originalMousePos);
        Vector4d delta(currentMousePosObject - originalMousePosObject);

        m_position -= delta;
        m_center -= delta;
    }

    m_originalMousePosition.x = event->pos().x();
    m_originalMousePosition.y = event->pos().y();
}

void Camera::wheelEvent(QWheelEvent* event)
{
    if (!m_pickpointNavigation)
        return;

    auto qPoint = event->position().toPoint();
    Vector2d point(qPoint.x(), qPoint.y());
    updatePickpoint(point);
    auto viewDir = VectorNormalize(m_position - m_pickpoint);
    auto lengthToTarget = VectorLength(m_position - m_pickpoint);
    if (event->angleDelta().y() > 0)
    {
        m_position -= lengthToTarget * viewDir * WHEEL_ZOOM_FRAC;
        m_center -= lengthToTarget * viewDir * WHEEL_ZOOM_FRAC;
    }
    else if (event->angleDelta().y() < 0)
    {
        m_position += lengthToTarget * viewDir * WHEEL_ZOOM_FRAC;
        m_center += lengthToTarget * viewDir * WHEEL_ZOOM_FRAC;
    }
}

void Camera::keyPressEvent(QKeyEvent* event) {}

void Camera::keyReleaseEvent(QKeyEvent* event) {}

void Camera::updateNearFar()
{
    Vector3d cameraPosition = m_position;
    Vector3d cameraCenter = m_center;

    auto bb = scene()->boundingBox();
    auto bbPoints = bb.points();
    float maxLength = std::numeric_limits<float>::min();
    float minLength = std::numeric_limits<float>::max();
    for (const auto& point : bbPoints)
    {
        auto dot = VectorDot(point - cameraPosition,
                             VectorNormalize(cameraCenter - cameraPosition));
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

Vector2d Camera::pickpoint() const
{
    auto projectedPickpoint = project(m_pickpoint);
    return {static_cast<int>(projectedPickpoint.x),
            static_cast<int>(m_viewportHeight - 1 -
                             static_cast<int>(projectedPickpoint.y))};
}

void Camera::topView()
{
    auto bb = scene()->boundingBox();
    Vector3d bbCenter = (bb.min + bb.max) / 2.0f; // Bounding box center point
    Vector3d bbExtents =
        (bb.max - bb.min) / 2.0f; // Bounding box half width and height

    float aspect = 1.0f * m_viewportWidth / m_viewportHeight;
    float tanVfov2 = std::tan(deg2rad(m_fieldOfView) / 2.0f);
    float tanHfov2 = tanVfov2 * aspect;

    // find height above bb center point so that the camera frustum sees all
    // bounding box corners
    float length1 = bbExtents.y * 1.0f / tanHfov2;
    float length2 = bbExtents.x * 1.0f / tanVfov2;
    float length = std::max(length1, length2) + bbExtents.z;

    m_position = Vector4d(bbCenter + Z_AXIS * length, 1.0f);
    m_center = Vector4d(bbCenter, 1.0f);
    m_up = Vector4d(X_AXIS, 0.0f);
}
