#include "SceneView.h"

#include <QFile>
#include <queue>

SceneView::SceneView(QWidget* parent)
    : QOpenGLWidget(parent)
{
    // to receive necessary events
    setFocusPolicy(Qt::StrongFocus);
    setFocusPolicy(Qt::WheelFocus);

    // set the OpenGL profile
    QSurfaceFormat format;
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setOption(QSurfaceFormat::DebugContext);
    setFormat(format);
}

SceneView::~SceneView() {}

void SceneView::reset()
{
    m_octree = Octree();
    m_render = false;
}

void SceneView::insert(const std::vector<PointData>& data)
{
    m_octree.insert(data);
}

void SceneView::doneInserting()
{
    makeCurrent();
    m_octree.finalize();
    m_pointCloud = std::make_shared<PointCloud>(m_octree);
    m_scene->addNode(m_pointCloud);
    m_render = true;
}

Scene& SceneView::scene() { return *m_scene; }

void SceneView::initializeGL()
{
    initializeOpenGLFunctions();
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);

    m_openGLLogger.initialize();

    connect(&m_openGLLogger, &QOpenGLDebugLogger::messageLogged, this,
            &SceneView::onMessageLogged);

    m_openGLLogger.enableMessages();
    m_openGLLogger.startLogging();

    makeCurrent();
    setupScene();
    setupShaders();
}

void SceneView::paintGL()
{
    if (!m_render)
        return;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_scene->shader()->use();
    m_scene->render();
}

void SceneView::resizeGL(int width, int height)
{
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

void SceneView::onMessageLogged(
    const QOpenGLDebugMessage& debugMessage)
{
    qDebug() << debugMessage.message();
}

void SceneView::setupScene()
{
    m_scene = std::make_shared<Scene>();
    m_camera = std::make_shared<Camera>();
    m_scene->addNode(m_camera);
}

void SceneView::setupShaders()
{
    m_shader = std::make_shared<Shader>(":/shaders/default_vertex.glsl",
                                        ":/shaders/default_fragment.glsl");
    m_shader->use();
    m_scene->setShader(m_shader);
}
