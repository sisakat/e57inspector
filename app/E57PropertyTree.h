#ifndef E57INSPECTOR_E57PROPERTYTREE_H
#define E57INSPECTOR_E57PROPERTYTREE_H

#include <QTreeWidget>
#include <set>

#include "E57TreeNode.h"

class E57PropertyTree : public QTreeWidget
{
    Q_OBJECT
public:
    explicit E57PropertyTree(QWidget* parent = nullptr);
    void init(TNode* node);

protected:
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    std::set<std::string> m_fields;

    void prepare();

    void addRow(QTreeWidgetItem* parent, const std::string& key,
                const std::string& value, bool bold = false);

    void addData3DData(TNode* node);
    void addRawData(QTreeWidgetItem* parent, E57NodePtr node);
    void addFields(QTreeWidgetItem* parent, E57NodePtr node);

    QFont m_boldFont;
};

#endif // E57INSPECTOR_E57PROPERTYTREE_H
