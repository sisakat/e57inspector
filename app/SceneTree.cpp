#include "SceneTree.h"

SceneTree::SceneTree(QWidget* parent) : QTreeWidget(parent)
{
    connect(this, &QTreeWidget::itemClicked, this, &SceneTree::onItemClicked);
    setHeaderHidden(true);
}

void SceneTree::init(Scene& scene)
{
    clear();
    auto* root = invisibleRootItem();
    auto& nodes = scene.nodes();
    for (auto& node : nodes)
    {
        root->addChild(SceneTreeNodeFactory::createSceneTreeNode(node.get()));
    }
}

void SceneTree::setScenePropertyEditor(ScenePropertyEditor* scenePropertyEditor)
{
    m_scenePropertyEditor = scenePropertyEditor;
}

void SceneTree::onItemClicked(QTreeWidgetItem* item, int column)
{
    if (m_scenePropertyEditor)
    {
        if (dynamic_cast<SceneTreeNode*>(item))
        {
            m_scenePropertyEditor->init(
                dynamic_cast<SceneTreeNode*>(item)->sceneNode());
        }
    }
}
