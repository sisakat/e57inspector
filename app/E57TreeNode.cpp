#include "E57TreeNode.h"

#include "E57Tree.h"
#include <utility>

TE57Node::TE57Node(E57NodePtr node) : m_node(std::move(node))
{
    setText(0, QString::fromStdString(m_node->name()));
}

E57NodePtr TE57Node::node() const
{
    return m_node;
}

TNodeE57::TNodeE57(const E57RootPtr& root) : TE57Node(root)
{
    setText(0, QString::fromStdString(root->name()));
    setIcon(0, QIcon(":/icons/E57.png"));

    m_contextMenu.addAction("Show XML dump",
                            [this]()
                            {
                                auto* tree =
                                    dynamic_cast<E57Tree*>(this->treeWidget());
                                tree->onAction(this, NodeAction::opXmlDump);
                            });
}

TNodeData3D::TNodeData3D(const E57Data3DPtr& node) : TE57Node(node)
{
    setText(0, QString::fromStdString(node->name()));
    setIcon(0, QIcon(":/icons/Pointcloud.png"));

    m_images = new TNodeImages();
    addChild(m_images);

    m_contextMenu.addAction("Open in 3D view",
                            [this]()
                            {
                                auto* tree =
                                    dynamic_cast<E57Tree*>(this->treeWidget());
                                tree->onAction(this, NodeAction::opView3d);
                            });
    m_contextMenu.addAction(
        "Open scan panorama",
        [this]()
        {
            auto* tree = dynamic_cast<E57Tree*>(this->treeWidget());
            tree->onAction(this, NodeAction::opScanPanorama);
        });
}

TNodeImages::TNodeImages() : TNode()
{
    setText(0, "Images");
    setIcon(0, QIcon(":/icons/Image2D.png"));
}

TNodeImage2D::TNodeImage2D(const E57Image2DPtr& node) : TE57Node(node)
{
    setText(0, QString::fromStdString(node->name()));
    setIcon(0, QIcon(":/icons/Image.png"));

    m_contextMenu.addAction("Open in 2D view",
                            [this]()
                            {
                                auto* tree =
                                    dynamic_cast<E57Tree*>(this->treeWidget());
                                tree->onAction(this, NodeAction::opView2d);
                            });
    m_contextMenu.addAction("Open in 3D view",
                            [this]()
                            {
                                auto* tree =
                                    dynamic_cast<E57Tree*>(this->treeWidget());
                                tree->onAction(this, NodeAction::opView3d);
                            });
    m_contextMenu.addAction("Save image...",
                            [this]() {
                                auto* tree = dynamic_cast<E57Tree*>(this->treeWidget());
                                tree->onAction(this, NodeAction::opSaveImage);
                            });
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

    return nullptr;
}
