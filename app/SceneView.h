#ifndef SIPOINTCLOUDRENDERER_H
#define SIPOINTCLOUDRENDERER_H

#include <QMatrix4x4>
#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QWidget>
#include <memory>
#include <vector>

#include "camera.h"
#include "octree.h"
#include "pointcloud.h"
#include "scene.h"
#include "shader.h"

class SceneView : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit SceneView(QWidget* parent = nullptr);
    ~SceneView() override;

    Scene& scene();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private slots:
    void onMessageLogged(const QOpenGLDebugMessage& debugMessage);

private:
    QOpenGLDebugLogger m_openGLLogger;

    Scene::Ptr m_scene;
    Shader::Ptr m_shader;
    Camera::Ptr m_camera;

    void setupScene();
    void setupShaders();
};

#endif // SIPOINTCLOUDRENDERER_H
