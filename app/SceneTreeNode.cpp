#include "SceneTreeNode.h"

SceneCameraTreeNode::SceneCameraTreeNode(Camera* camera) : m_camera{camera}
{
    setText(0, "Camera");
}

SceneNode* SceneCameraTreeNode::sceneNode()
{
    return m_camera;
}

ScenePointcloudTreeNode::ScenePointcloudTreeNode(PointCloud* pointcloud)
    : m_pointcloud(pointcloud)
{
    setText(0, QString::fromStdString(m_pointcloud->data3D().name()));
    setIcon(0, QIcon(":/icons/Pointcloud.png"));

    m_contextMenu.addAction(
        "Remove",
        [this]()
        {
            auto* parent = this->parent();
            if (!parent)
            {
                // remove from tree
                this->treeWidget()->invisibleRootItem()->removeChild(this);

                // remove from render scene
                this->sceneNode()->scene()->removeNode(this->sceneNode());
            }
            else
            {
                parent->removeChild(this);
            }
        });
}

SceneNode* ScenePointcloudTreeNode::sceneNode()
{
    return m_pointcloud;
}

SceneImage2dTreeNode::SceneImage2dTreeNode(Image2d* image2d)
    : m_image2d(image2d)
{
    setText(0, QString::fromStdString(m_image2d->name()));
    if (m_image2d->isSpherical())
    {
        setIcon(0, QIcon(":/icons/Spherical.png"));

        m_contextMenu.addAction(
            "Set camera to sphere center",
            [this]() {
                dynamic_cast<Image2d*>(this->sceneNode())->cameraToImageView();
            });
    }
    else
    {
        setIcon(0, QIcon(":/icons/Image.png"));
    }
}

SceneNode* SceneImage2dTreeNode::sceneNode()
{
    return m_image2d;
}
