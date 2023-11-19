#ifndef E57INSPECTOR_E57TREE_H
#define E57INSPECTOR_E57TREE_H

#include <QTreeWidget>

#include "E57TreeNode.h"

#include <e57inspector/E57Node.h>

class E57Tree : public QTreeWidget
{
    Q_OBJECT

public:
    E57Tree(QWidget* parent = nullptr);

    void init(const E57RootPtr& root);

signals:
    void nodeSelected(TNode* node);
    void nodeDoubleClick(TNode* node);

protected:
    void selectionChanged(const QItemSelection& selected,
                          const QItemSelection& deselected) override;
};

#endif // E57INSPECTOR_E57TREE_H
