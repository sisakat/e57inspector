#include "SceneTreeNodeFactory.h"

#include "pointcloud.h"

SceneTreeNode* SceneTreeNodeFactory::createSceneTreeNode(SceneNode* sceneNode)
{
    if (dynamic_cast<Camera*>(sceneNode))
    {
        return new SceneCameraTreeNode(dynamic_cast<Camera*>(sceneNode));
    }
    else if (dynamic_cast<PointCloud*>(sceneNode))
    {
        return new ScenePointcloudTreeNode(
            dynamic_cast<PointCloud*>(sceneNode));
    }

    return nullptr;
}
