#ifndef E57INSPECTOR_SCENETREENODE_H
#define E57INSPECTOR_SCENETREENODE_H

#include <QMenu>
#include <QTreeWidgetItem>

#include "camera.h"
#include "pointcloud.h"
#include "scene.h"

class SceneTreeNode : public QTreeWidgetItem
{
public:
    virtual SceneNode* sceneNode() = 0;

    [[nodiscard]] QMenu& contextMenu() { return m_contextMenu; }

protected:
    QMenu m_contextMenu;
};

class SceneCameraTreeNode : public SceneTreeNode
{
public:
    explicit SceneCameraTreeNode(Camera* camera);

    SceneNode* sceneNode() override;

private:
    Camera* m_camera;
};

class ScenePointcloudTreeNode : public SceneTreeNode
{
public:
    explicit ScenePointcloudTreeNode(PointCloud* pointcloud);

    SceneNode* sceneNode() override;

private:
    PointCloud* m_pointcloud;
};

#endif // E57INSPECTOR_SCENETREENODE_H
