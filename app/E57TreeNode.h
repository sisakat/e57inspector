#ifndef E57INSPECTOR_E57TREENODE_H
#define E57INSPECTOR_E57TREENODE_H

#include <QTreeWidgetItem>
#include <e57inspector/E57Node.h>

class TNode : public QTreeWidgetItem
{
public:
    template <typename T> T* findParent()
    {
        QTreeWidgetItem* parent = this->parent();
        while (parent != nullptr)
        {
            if (dynamic_cast<T*>(parent))
            {
                return dynamic_cast<T*>(parent);
            }
            parent = parent->parent();
        }
        return nullptr;
    }
};

class TE57Node : public TNode
{
public:
    explicit TE57Node(E57NodePtr node);
    E57NodePtr node();

private:
    E57NodePtr m_node;
};

class TNodeE57 : public TE57Node
{
public:
    explicit TNodeE57(const E57RootPtr& root);
};

class TNodeImages : public TNode
{
public:
    TNodeImages();
};

class TNodeData3D : public TE57Node
{
public:
    explicit TNodeData3D(const E57Data3DPtr& node);

    TNodeImages* images() { return m_images; }

private:
    TNodeImages* m_images;
};

class TNodeImage2D : public TE57Node
{
public:
    explicit TNodeImage2D(const E57Image2DPtr& node);
};

TNode* createTNode(const E57NodePtr& e57Node);

#endif // E57INSPECTOR_E57TREENODE_H
