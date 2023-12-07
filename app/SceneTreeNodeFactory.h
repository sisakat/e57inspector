#ifndef E57INSPECTOR_SCENETREENODEFACTORY_H
#define E57INSPECTOR_SCENETREENODEFACTORY_H

#include "SceneTreeNode.h"

class SceneTreeNodeFactory
{
public:
    static SceneTreeNode* createSceneTreeNode(SceneNode* sceneNode);
};

#endif // E57INSPECTOR_SCENETREENODEFACTORY_H
