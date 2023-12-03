#include "sipointcloudrenderer.h"

#include <QFile>
#include <queue>

SiPointCloudRenderer::SiPointCloudRenderer(QWidget* parent)
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

SiPointCloudRenderer::~SiPointCloudRenderer() {}

void SiPointCloudRenderer::reset()
{
    m_octree = Octree();
    m_render = false;
}

void SiPointCloudRenderer::insert(const std::vector<PointData>& data)
{
    m_octree.insert(data);
}

void SiPointCloudRenderer::doneInserting()
{
    makeCurrent();
    m_octree.finalize();
    m_pointCloud = std::make_shared<PointCloud>(m_octree);
    m_scene->addNode(m_pointCloud);
    m_render = true;
}

void SiPointCloudRenderer::initializeGL()
{
    initializeOpenGLFunctions();
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);

    m_openGLLogger.initialize();

    connect(&m_openGLLogger, &QOpenGLDebugLogger::messageLogged, this,
            &SiPointCloudRenderer::onMessageLogged);

    m_openGLLogger.enableMessages();
    m_openGLLogger.startLogging();

    makeCurrent();
    setupScene();
    setupShaders();
    setupBuffers();
}

void SiPointCloudRenderer::paintGL()
{
    if (!m_render)
        return;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_scene->shader()->use();
    m_scene->render();
}

void SiPointCloudRenderer::resizeGL(int width, int height)
{
    m_camera->setViewportWidth(width);
    m_camera->setViewportHeight(height);
    update();
}

void SiPointCloudRenderer::mousePressEvent(QMouseEvent* event)
{
    makeCurrent();
    m_camera->mousePressEvent(event);
    update();
}

void SiPointCloudRenderer::mouseReleaseEvent(QMouseEvent* event)
{
    makeCurrent();
    m_camera->mouseReleaseEvent(event);
    update();
}

void SiPointCloudRenderer::mouseMoveEvent(QMouseEvent* event)
{
    makeCurrent();
    m_camera->mouseMoveEvent(event);
    update();
}

void SiPointCloudRenderer::wheelEvent(QWheelEvent* event)
{
    makeCurrent();
    m_camera->wheelEvent(event);
    update();
}

void SiPointCloudRenderer::keyPressEvent(QKeyEvent* event)
{
    makeCurrent();
    m_camera->keyPressEvent(event);
    update();
}

void SiPointCloudRenderer::keyReleaseEvent(QKeyEvent* event)
{
    makeCurrent();
    m_camera->keyReleaseEvent(event);
    update();
}

void SiPointCloudRenderer::onMessageLogged(
    const QOpenGLDebugMessage& debugMessage)
{
    qDebug() << debugMessage.message();
}

void SiPointCloudRenderer::setupScene()
{
    m_scene = std::make_shared<Scene>();
    m_camera = std::make_shared<Camera>();
    m_scene->addNode(m_camera);
}

void SiPointCloudRenderer::setupShaders()
{
    m_shader = std::make_shared<Shader>(":/shaders/default_vertex.glsl",
                                        ":/shaders/default_fragment.glsl");
    m_shader->use();
    m_scene->setShader(m_shader);
}

void SiPointCloudRenderer::setupBuffers() {}
