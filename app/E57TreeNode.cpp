#include "E57TreeNode.h"

#include <utility>

TNode::TNode(E57NodePtr node) : m_node(std::move(node)) {}

TNodeE57::TNodeE57(const E57RootPtr& root) : TNode(root)
{
    setText(0, QString::fromStdString(root->name()));
}

TNodeData3D::TNodeData3D(const E57Data3DPtr& node) : TNode(node)
{
    setText(0, QString::fromStdString(node->name()));
}

TNode* createTNode(const E57NodePtr& e57Node)
{
    if (std::dynamic_pointer_cast<E57Root>(e57Node) != nullptr)
    {
        return new TNodeE57(std::dynamic_pointer_cast<E57Root>(e57Node));
    }
    else if (std::dynamic_pointer_cast<E57Data3D>(e57Node))
    {
        return new TNodeData3D(std::dynamic_pointer_cast<E57Data3D>(e57Node));
    }

    return nullptr;
}
