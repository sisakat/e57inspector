#include "E57PropertyTree.h"

#include <utility>

#include "utils.h"

E57PropertyTree::E57PropertyTree(QWidget* parent) : QTreeWidget(parent)
{
    prepare();
    m_boldFont.setBold(true);
}

void E57PropertyTree::init(TNode* node)
{
    clear();
    prepare();

    if (!node)
        return;

    auto e57Node = node->node();

    addRow(nullptr, "GUID", e57Node->getString("guid", "n.v."), true);

    addData3DData(node);
    addRawData(invisibleRootItem(), e57Node);

    expandAll();
    resizeColumnToContents(0);
}

void E57PropertyTree::prepare()
{
    setColumnCount(2);
    auto* header = headerItem();
    header->setText(0, "Key");
    header->setText(1, "Value");
}

void E57PropertyTree::addData3DData(TNode* node)
{
    auto e57Node = std::dynamic_pointer_cast<E57Data3D>(node->node());
    if (!e57Node)
        return;

    addRow(nullptr, "Name", e57Node->getString("name", "n.v."), true);
    addRow(nullptr, "Description", e57Node->getString("description", "n.v."),
           true);
}

void E57PropertyTree::addRawData(QTreeWidgetItem* parent, E57NodePtr node)
{
    addFields(parent, node);
    for (const auto& child : node->children())
    {
        auto* item = new QTreeWidgetItem();
        item->setText(0, QString::fromStdString(camelCaseToPascalCase(child->name())));
        parent->addChild(item);
        addRawData(item, child);
    }
}

void E57PropertyTree::addRow(QTreeWidgetItem* parent, const std::string& key,
                             const std::string& value, bool bold)
{
    auto* item = new QTreeWidgetItem();
    item->setText(0, QString::fromStdString(key));
    item->setText(1, QString::fromStdString(value));
    m_fields.insert(to_lower(key));

    if (bold)
    {
        item->setFont(0, m_boldFont);
    }
    if (parent)
    {
        parent->addChild(item);
    }
    else
    {
        addTopLevelItem(item);
    }
}

void E57PropertyTree::addFields(QTreeWidgetItem* parent, E57NodePtr node)
{
    QTreeWidgetItem* rawData = nullptr;
    if (parent)
    {
        rawData = parent;
    }
    else
    {
        rawData = this->invisibleRootItem();
    }

    for (const auto& [key, value] : node->strings())
    {
        if (m_fields.contains(to_lower(key)))
            continue;
        auto* item = new QTreeWidgetItem();
        item->setText(0, QString::fromStdString(camelCaseToPascalCase(key)));
        item->setText(1, QString::fromStdString(value));
        rawData->addChild(item);
    }

    for (const auto& [key, value] : node->integers())
    {
        if (m_fields.contains(to_lower(key)))
            continue;
        auto* item = new QTreeWidgetItem();
        item->setText(0, QString::fromStdString(camelCaseToPascalCase(key)));
        item->setText(1, QString::fromStdString(std::to_string(value)));
        rawData->addChild(item);
    }

    for (const auto& [key, value] : node->floats())
    {
        if (m_fields.contains(to_lower(key)))
            continue;
        auto* item = new QTreeWidgetItem();
        item->setText(0, QString::fromStdString(camelCaseToPascalCase(key)));
        item->setText(1, QString::fromStdString(std::to_string(value)));
        rawData->addChild(item);
    }
}
