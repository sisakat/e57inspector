#include "E57Tree.h"

E57Tree::E57Tree(QWidget* parent) : QTreeWidget(parent)
{
    setColumnCount(1);
    setHeaderHidden(true);
    setIconSize(QSize(16, 16));
}

void E57Tree::init(const E57RootPtr& root)
{
    clear();

    auto* topLevelNode = dynamic_cast<TNodeE57*>(createTNode(root));
    addTopLevelItem(topLevelNode);

    auto* data3DNode = new QTreeWidgetItem();
    data3DNode->setText(0, "Data 3D");
    data3DNode->setIcon(0, QIcon(":/icons/Data3D.png"));
    topLevelNode->addChild(data3DNode);

    for (const auto& data3D : root->data3D())
    {
        auto* node = createTNode(data3D);
        data3DNode->addChild(node);
    }

    auto* image2DNode = new QTreeWidgetItem();
    image2DNode->setText(0, "Images 2D");
    image2DNode->setIcon(0, QIcon(":/icons/Image2D.png"));
    topLevelNode->addChild(image2DNode);

    for (const auto& image2D : root->images2D())
    {
        auto* node = createTNode(image2D);
        if (image2D->strings().contains("associatedData3DGuid"))
        {
            auto* data3DNode =
                findData3DNode(this->invisibleRootItem(),
                               image2D->strings().at("associatedData3DGuid"));
            data3DNode->images()->addChild(node);
        }
        else
        {
            image2DNode->addChild(node);
        }
    }

    topLevelNode->setExpanded(true);
    data3DNode->setExpanded(true);
    image2DNode->setExpanded(true);
    resizeColumnToContents(0);
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
    else
    {
        emit nodeSelected(nullptr);
    }
}

TNodeData3D* E57Tree::findData3DNode(QTreeWidgetItem* item,
                                     const std::string& guid) const
{
    if (dynamic_cast<TE57Node*>(item))
    {
        auto* node = dynamic_cast<TE57Node*>(item);
        if (node->node()->strings().contains("guid"))
        {
            if (node->node()->strings().at("guid") == guid)
            {
                return dynamic_cast<TNodeData3D*>(node);
            }
        }
    }

    for (int i = 0; i < item->childCount(); ++i)
    {
        auto* result = findData3DNode(item->child(i), guid);
        if (result)
        {
            return result;
        }
    }

    return nullptr;
}
