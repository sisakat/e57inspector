#include "E57TreeNode.h"

#include <utility>

TNode::TNode(E57NodePtr node) : m_node(std::move(node))
{
    setText(0, QString::fromStdString(m_node->name()));
}

E57NodePtr TNode::node() { return m_node; }

TNodeE57::TNodeE57(const E57RootPtr& root) : TNode(root)
{
    setText(0, QString::fromStdString(root->name()));
}

TNodeData3D::TNodeData3D(const E57Data3DPtr& node) : TNode(node)
{
    setText(0, QString::fromStdString(node->name()));
}

TNodeImage2D::TNodeImage2D(const E57Image2DPtr& node) : TNode(node)
{
    setText(0, QString::fromStdString(node->name()));
}

TNodePinholeRepresentation::TNodePinholeRepresentation(
    const E57PinholeRepresentationPtr& node)
    : TNode(node)
{
    setText(0, QString("Pinhole"));
}

TNodeSphericalRepresentation::TNodeSphericalRepresentation(
    const E57SphericalRepresentationPtr& node)
    : TNode(node)
{
    setText(0, QString("Spherical"));
}

TNodeCylindricalRepresentation::TNodeCylindricalRepresentation(
    const E57CylindricalRepresentationPtr& node)
    : TNode(node)
{
    setText(0, QString("Cylindrical"));
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
    else if (std::dynamic_pointer_cast<E57Image2D>(e57Node))
    {
        return new TNodeImage2D(std::dynamic_pointer_cast<E57Image2D>(e57Node));
    }
    else if (std::dynamic_pointer_cast<E57PinholeRepresentation>(e57Node))
    {
        return new TNodePinholeRepresentation(
            std::dynamic_pointer_cast<E57PinholeRepresentation>(e57Node));
    }
    else if (std::dynamic_pointer_cast<E57SphericalRepresentation>(e57Node))
    {
        return new TNodeSphericalRepresentation(
            std::dynamic_pointer_cast<E57SphericalRepresentation>(e57Node));
    }

    return nullptr;
}
