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
    void init(TE57Node* node);

protected:
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    std::set<std::string> m_fields;

    void prepare();

    void addRow(QTreeWidgetItem* parent, const std::string& key,
                const std::string& value, bool bold = false);

    void addData3DData(TE57Node* node);
    void addImage2DData(TE57Node* node);
    void addRawData(QTreeWidgetItem* parent, E57NodePtr node);
    void addFields(QTreeWidgetItem* parent, E57NodePtr node);

    QFont m_boldFont;
    QFont m_italicFont;
};

#endif // E57INSPECTOR_E57PROPERTYTREE_H
