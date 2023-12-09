#include "SceneTree.h"
#include <QMenu>

SceneTree::SceneTree(QWidget* parent) : QTreeWidget(parent)
{
    setHeaderHidden(true);
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &QTreeWidget::itemClicked, this, &SceneTree::onItemClicked);
    connect(this, &QTreeWidget::customContextMenuRequested, this,
            &SceneTree::onCustomContextMenuRequested);
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

void SceneTree::onCustomContextMenuRequested(const QPoint& pos)
{
    QTreeWidgetItem* item = itemAt(pos);
    if (!item)
        return;

    if (!dynamic_cast<SceneTreeNode*>(item))
        return;

    auto* sceneTreeNode = dynamic_cast<SceneTreeNode*>(item);
    auto& menu = sceneTreeNode->contextMenu();
    menu.exec(viewport()->mapToGlobal(pos));
}
