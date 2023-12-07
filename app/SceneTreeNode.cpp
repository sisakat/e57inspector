#include "SceneTreeNode.h"

SceneCameraTreeNode::SceneCameraTreeNode(Camera* camera) : m_camera{camera}
{
    setText(0, "Camera");
}

SceneNode* SceneCameraTreeNode::sceneNode() { return m_camera; }

ScenePointcloudTreeNode::ScenePointcloudTreeNode(PointCloud* pointcloud)
    : m_pointcloud(pointcloud)
{
    setText(0, QString::fromStdString(m_pointcloud->data3D().name()));
}

SceneNode* ScenePointcloudTreeNode::sceneNode() { return m_pointcloud; }
