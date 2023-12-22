#include "SceneView.h"

#include <QFile>
#include <QPainter>
#include <queue>

SceneView::SceneView(QWidget* parent) : QOpenGLWidget(parent)
{
    // to receive necessary events
    setFocusPolicy(Qt::WheelFocus);

    // set the OpenGL profile
    QSurfaceFormat format;
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
#ifdef DEBUG
    format.setOption(QSurfaceFormat::DebugContext);
#endif
    setFormat(format);
}

SceneView::~SceneView() = default;

Scene& SceneView::scene()
{
    return *m_scene;
}

void SceneView::initializeGL()
{
    initializeOpenGLFunctions();
    glClear(GL_COLOR_BUFFER_BIT);

#ifdef DEBUG
    m_openGLLogger.initialize();
    connect(&m_openGLLogger, &QOpenGLDebugLogger::messageLogged, this,
            &SceneView::onMessageLogged);
    m_openGLLogger.enableMessages();
    m_openGLLogger.startLogging();
#endif

    makeCurrent();
    setupScene();
}

void SceneView::paintGL()
{
    QPainter painter(this);

    painter.beginNativePainting();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_scene->render();
    painter.endNativePainting();

    glDisable(GL_DEPTH_TEST);
    m_scene->render2D(painter);
}

void SceneView::resizeGL(int width, int height)
{
    width = static_cast<int>(width * devicePixelRatio());
    height = static_cast<int>(height * devicePixelRatio());
    m_camera->setViewportWidth(width);
    m_camera->setViewportHeight(height);
    update();
}

void SceneView::mousePressEvent(QMouseEvent* event)
{
    makeCurrent();
    m_camera->mousePressEvent(event);
    update();
}

void SceneView::mouseReleaseEvent(QMouseEvent* event)
{
    makeCurrent();
    m_camera->mouseReleaseEvent(event);
    update();
}

void SceneView::mouseMoveEvent(QMouseEvent* event)
{
    makeCurrent();
    m_camera->mouseMoveEvent(event);
    update();
}

void SceneView::wheelEvent(QWheelEvent* event)
{
    makeCurrent();
    m_camera->wheelEvent(event);
    update();
}

void SceneView::keyPressEvent(QKeyEvent* event)
{
    makeCurrent();
    m_camera->keyPressEvent(event);
    update();
}

void SceneView::keyReleaseEvent(QKeyEvent* event)
{
    makeCurrent();
    m_camera->keyReleaseEvent(event);
    update();
}

void SceneView::onMessageLogged(const QOpenGLDebugMessage& debugMessage)
{
    qDebug() << debugMessage.message();
}

void SceneView::setupScene()
{
    m_scene = std::make_shared<Scene>();
    m_camera = std::make_shared<Camera>();
    m_scene->setDevicePixelRatio(static_cast<float>(devicePixelRatio()));
    m_scene->addNode(m_camera);
}
