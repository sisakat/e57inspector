#ifndef E57INSPECTOR_E57TREE_H
#define E57INSPECTOR_E57TREE_H

#include <QTreeWidget>

#include "E57TreeNode.h"
#include "NodeAction.h"

#include <e57inspector/E57Node.h>

class E57Tree : public QTreeWidget
{
    Q_OBJECT

public:
    explicit E57Tree(QWidget* parent = nullptr);

    void init(const E57RootPtr& root);

    TNodeData3D* findData3DNode(QTreeWidgetItem* item,
                                const std::string& guid) const;

signals:
    void nodeSelected(TNode* node);
    void nodeDoubleClick(TNode* node);
    void onAction(const TNode* node, NodeAction action);

protected:
    void selectionChanged(const QItemSelection& selected,
                          const QItemSelection& deselected) override;
    void onCustomContextMenuRequested(const QPoint& pos);
    void emitOnAction(const TNode* node, NodeAction action);
};

#endif // E57INSPECTOR_E57TREE_H
