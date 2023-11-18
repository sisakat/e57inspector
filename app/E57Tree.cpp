#include "E57Tree.h"

E57Tree::E57Tree(QWidget* parent) : QTreeWidget(parent) {}

void E57Tree::init(const E57RootPtr& root)
{
    clear();

    auto* topLevelNode = dynamic_cast<TNodeE57*>(createTNode(root));
    addTopLevelItem(topLevelNode);

    auto* data3DNode = new QTreeWidgetItem();
    data3DNode->setText(0, "Data 3D");
    topLevelNode->addChild(data3DNode);

    for (const auto& data3D : root->data3D())
    {
        auto* node = createTNode(data3D);
        data3DNode->addChild(node);
    }

    auto* image2DNode = new QTreeWidgetItem();
    image2DNode->setText(0, "Images 2D");
    topLevelNode->addChild(image2DNode);

    for (const auto& image2D : root->images2D())
    {
        auto* node = createTNode(image2D);
        image2DNode->addChild(node);
    }
}

void E57Tree::selectionChanged(const QItemSelection& selected,
                               const QItemSelection& deselected)
{
    QTreeView::selectionChanged(selected, deselected);

    if (selectedItems().empty())
        return;

    auto* selectedItem = dynamic_cast<TNode*>(selectedItems().first());
    if (selectedItem)
    {
        emit nodeSelected(selectedItem);
    }
}
