#ifndef SIPOINTCLOUDRENDERER_H
#define SIPOINTCLOUDRENDERER_H

#include <QWidget>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLDebugLogger>
#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <vector>
#include <memory>

#include "octree.h"
#include "scene.h"
#include "camera.h"
#include "pointcloud.h"
#include "shader.h"

class SceneView : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit SceneView(QWidget *parent = nullptr);
    ~SceneView() override;

    void reset();

    Scene& scene();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void onMessageLogged(const QOpenGLDebugMessage &debugMessage);

private:
    QOpenGLDebugLogger m_openGLLogger;

    QMatrix4x4 m_model;
    QMatrix4x4 m_view;
    QMatrix4x4 m_projection;

    Scene::Ptr m_scene;
    Shader::Ptr m_shader;
    Camera::Ptr m_camera;

    void setupScene();
    void setupShaders();
};

#endif // SIPOINTCLOUDRENDERER_H
