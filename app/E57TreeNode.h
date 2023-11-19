#ifndef E57INSPECTOR_E57TREENODE_H
#define E57INSPECTOR_E57TREENODE_H

#include <QTreeWidgetItem>
#include <e57inspector/E57Node.h>

class TNode : public QTreeWidgetItem
{
public:
    explicit TNode(E57NodePtr node);
    E57NodePtr node();

private:
    E57NodePtr m_node;
};

class TNodeE57 : public TNode
{
public:
    explicit TNodeE57(const E57RootPtr& root);
};

class TNodeData3D : public TNode
{
public:
    explicit TNodeData3D(const E57Data3DPtr& node);
};

class TNodeImage2D : public TNode
{
public:
    explicit TNodeImage2D(const E57Image2DPtr& node);
};

class TNodePinholeRepresentation : public TNode
{
public:
    explicit TNodePinholeRepresentation(const E57PinholeRepresentationPtr& node);
};

class TNodeSphericalRepresentation : public TNode
{
public:
    explicit TNodeSphericalRepresentation(const E57SphericalRepresentationPtr& node);
};

class TNodeCylindricalRepresentation : public TNode
{
public:
    explicit TNodeCylindricalRepresentation(const E57CylindricalRepresentationPtr& node);
};

TNode* createTNode(const E57NodePtr& e57Node);

#endif // E57INSPECTOR_E57TREENODE_H
