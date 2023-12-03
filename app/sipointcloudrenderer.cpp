#include "sipointcloudrenderer.h"

#include <queue>

static const char* VERTEX_SHADER =
    "#version 330                                                        \n"
    "layout(location = 0) in vec3 vtx_xyz  ;                             \n"
    "layout(location = 1) in vec3 vtx_rgb  ;                             \n"
    "out vec3 f_rgb;                                                     \n"
    "uniform mat4 model;                                                 \n"
    "uniform mat4 view;                                                  \n"
    "uniform mat4 projection;                                            \n"
    "void main() {                                                       \n"
    "   gl_Position = projection * view * model * vec4(vtx_xyz, 1.0);    \n"
    "   f_rgb = vtx_rgb;                                                 \n"
    "}                                                                   \n";

static const char* FRAGMENT_SHADER =
    "#version 330                                                        \n"
    "in vec3 f_rgb;                                                      \n"
    "layout(location = 0) out vec4 FragColor;                            \n"
    "void main() {                                                       \n"
    "   FragColor = vec4(f_rgb, 1.0);                                    \n"
    "}                                                                   \n";

SiPointCloudRenderer::SiPointCloudRenderer(QWidget *parent) : QOpenGLWidget(parent)
{
    // to receive necessary events
    setFocusPolicy(Qt::StrongFocus);
    setFocusPolicy(Qt::WheelFocus);

    // set the OpenGL profile
    QSurfaceFormat format;
    format.setVersion(3,3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setOption(QSurfaceFormat::DebugContext);
    setFormat(format);
}

SiPointCloudRenderer::~SiPointCloudRenderer()
{
    glDetachShader(m_shaderProgram, m_vertexShader);
    glDetachShader(m_shaderProgram, m_fragmentShader);
    glDeleteShader(m_vertexShader);
    glDeleteShader(m_fragmentShader);
    glDeleteProgram(m_shaderProgram);
}

void SiPointCloudRenderer::reset()
{
    m_octree = Octree();
    m_render = false;
}

void SiPointCloudRenderer::insert(const std::vector<PointData> &data)
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

    m_openGLLogger.initialize();

    connect(&m_openGLLogger, &QOpenGLDebugLogger::messageLogged, this, &SiPointCloudRenderer::onMessageLogged);

    m_openGLLogger.enableMessages();
    m_openGLLogger.startLogging();

    setupScene();
    setupShaders();
    setupBuffers();
    makeCurrent();
}

void SiPointCloudRenderer::paintGL()
{
    if (!m_render) return;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_shaderProgram);
    m_scene->render();
}

void SiPointCloudRenderer::resizeGL(int width, int height)
{
    m_camera->setViewportWidth(width);
    m_camera->setViewportHeight(height);
    update();
}

void SiPointCloudRenderer::mousePressEvent(QMouseEvent *event)
{
    makeCurrent();
    m_camera->mousePressEvent(event);
    update();
}

void SiPointCloudRenderer::mouseReleaseEvent(QMouseEvent *event)
{
    m_camera->mouseReleaseEvent(event);
    update();
}

void SiPointCloudRenderer::mouseMoveEvent(QMouseEvent *event)
{
    m_camera->mouseMoveEvent(event);
    update();
}

void SiPointCloudRenderer::wheelEvent(QWheelEvent *event)
{
    m_camera->wheelEvent(event);
    update();
}

void SiPointCloudRenderer::keyPressEvent(QKeyEvent *event)
{
    m_camera->keyPressEvent(event);
    update();
}

void SiPointCloudRenderer::keyReleaseEvent(QKeyEvent *event)
{
    m_camera->keyReleaseEvent(event);
    update();
}

void SiPointCloudRenderer::onMessageLogged(const QOpenGLDebugMessage &debugMessage)
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
    const char* source;
    int length;
    GLint status;

    source = VERTEX_SHADER;
    length = strlen(VERTEX_SHADER);
    m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_vertexShader, 1, &source, &length);
    glCompileShader(m_vertexShader);
    glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        throw std::runtime_error("Could not compile vertex shader.");
    }

    source = FRAGMENT_SHADER;
    length = strlen(FRAGMENT_SHADER);
    m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_fragmentShader, 1, &source, &length);
    glCompileShader(m_fragmentShader);
    glGetShaderiv(m_fragmentShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        throw std::runtime_error("Could not compile fragment shader.");
    }

    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, m_vertexShader);
    glAttachShader(m_shaderProgram, m_fragmentShader);

    glLinkProgram(m_shaderProgram);
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        throw std::runtime_error("Could not link shaders.");
    }

    glUseProgram(m_shaderProgram);
    m_camera->setViewNameInShader("view", glGetUniformLocation(m_shaderProgram, "view"));
    m_camera->setProjectionNameInShader("projection", glGetUniformLocation(m_shaderProgram, "projection"));
}

void SiPointCloudRenderer::setupBuffers()
{

}
