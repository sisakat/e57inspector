#include "E57PropertyTree.h"

#include <utility>

#include "utils.h"

const int COLUMN_KEY = 0;
const int COLUMN_VALUE = 1;
const char* COLUMN_NAMES[]{"Key", "Value"};
const int COLUMN_COUNT = sizeof(COLUMN_NAMES) / sizeof(char*);

E57PropertyTree::E57PropertyTree(QWidget* parent) : QTreeWidget(parent)
{
    prepare();
    m_boldFont.setBold(true);
    m_italicFont.setItalic(true);
    setIconSize(QSize(16, 16));
    setHeaderHidden(true);
}

void E57PropertyTree::init(TE57Node* node)
{
    setHeaderHidden(false);
    clear();
    prepare();

    if (!node)
        return;

    auto e57Node = node->node();

    addRow(nullptr, "GUID", e57Node->getString("guid", "n.v."), true);

    addData3DData(node);
    addImage2DData(node);
    addRawData(invisibleRootItem(), e57Node);

    expandAll();
    resizeColumnToContents(0);
    // resizeColumnToContents(1);
}

void E57PropertyTree::prepare()
{
    setColumnCount(COLUMN_COUNT);
    auto* header = headerItem();
    for (int i = 0; i < COLUMN_COUNT; ++i)
    {
        header->setText(i, COLUMN_NAMES[i]);
    }
}

void E57PropertyTree::addData3DData(TE57Node* node)
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
    if (!node)
        return;
    addFields(parent, node);
    for (const auto& child : node->children())
    {
        auto* item = new QTreeWidgetItem();
        item->setText(COLUMN_KEY, QString::fromStdString(
                                      camelCaseToPascalCase(child->name())));
        if (child->name() == "pose")
        {
            item->setIcon(0, QIcon(":/icons/Pose.png"));
        }
        parent->addChild(item);
        addRawData(item, child);
        item->sortChildren(0, Qt::SortOrder::AscendingOrder);
    }
}

void E57PropertyTree::addRow(QTreeWidgetItem* parent, const std::string& key,
                             const std::string& value, bool bold)
{
    auto* item = new QTreeWidgetItem();
    item->setText(COLUMN_KEY, QString::fromStdString(key));
    item->setText(COLUMN_VALUE, QString::fromStdString(value));

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
        item->setText(COLUMN_KEY,
                      QString::fromStdString(camelCaseToPascalCase(key)));
        item->setText(COLUMN_VALUE, QString::fromStdString(value));
        item->setIcon(0, QIcon(":/icons/String.png"));
        rawData->addChild(item);
    }

    for (const auto& [key, value] : node->integers())
    {
        if (m_fields.contains(to_lower(key)))
            continue;
        auto* item = new QTreeWidgetItem();
        item->setText(COLUMN_KEY,
                      QString::fromStdString(camelCaseToPascalCase(key)));
        item->setText(COLUMN_VALUE,
                      QString::fromStdString(std::to_string(value)));
        item->setIcon(0, QIcon(":/icons/Integer.png"));
        rawData->addChild(item);
    }

    for (const auto& [key, value] : node->floats())
    {
        if (m_fields.contains(to_lower(key)))
            continue;
        auto* item = new QTreeWidgetItem();
        item->setText(COLUMN_KEY,
                      QString::fromStdString(camelCaseToPascalCase(key)));
        item->setText(COLUMN_VALUE,
                      QString::fromStdString(std::to_string(value)));
        item->setIcon(0, QIcon(":/icons/Float.png"));
        rawData->addChild(item);
    }

    for (const auto& [key, value] : node->blobs())
    {
        if (m_fields.contains(to_lower(key)))
            continue;
        auto* item = new QTreeWidgetItem();
        item->setText(COLUMN_KEY,
                      QString::fromStdString(camelCaseToPascalCase(key)));
        item->setText(COLUMN_VALUE, "(available)");
        item->setFont(COLUMN_VALUE, m_italicFont);
        rawData->addChild(item);
    }
}

void E57PropertyTree::mouseDoubleClickEvent(QMouseEvent* event)
{
    QTreeView::mouseDoubleClickEvent(event);
    qDebug() << "Double click";
}

void E57PropertyTree::addImage2DData(TE57Node* node)
{
    auto e57Node = std::dynamic_pointer_cast<E57Image2D>(node->node());
    if (!e57Node)
        return;

    if (e57Node->pinholeRepresentation())
    {
        auto* node = new QTreeWidgetItem();
        node->setText(COLUMN_KEY, QString("Pinhole Representation"));
        node->setFont(0, m_boldFont);
        addTopLevelItem(node);
        addRawData(node, e57Node->pinholeRepresentation());
    }

    if (e57Node->sphericalRepresentation())
    {
        auto* node = new QTreeWidgetItem();
        node->setText(COLUMN_KEY, QString("Spherical Representation"));
        node->setFont(0, m_boldFont);
        addTopLevelItem(node);
        addRawData(node, e57Node->sphericalRepresentation());
    }

    if (e57Node->cylindricalRepresentation())
    {
        auto* node = new QTreeWidgetItem();
        node->setText(COLUMN_KEY, QString("Cylindrical Representation"));
        node->setFont(0, m_boldFont);
        addTopLevelItem(node);
        addRawData(node, e57Node->cylindricalRepresentation());
    }
}
