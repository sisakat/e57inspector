#include "E57ReaderImpl.h"

template <typename T>
std::shared_ptr<T> convertE57StructureNode(const e57::StructureNode& node)
{
    std::shared_ptr<T> result = std::make_shared<T>();
    result->setName(node.elementName());

    if (node.isDefined("name"))
    {
        result->setName(e57::StringNode(node.get("name")).value());
    }

    for (int64_t i = 0; i < node.childCount(); ++i)
    {
        auto child = node.get(i);
        if (child.type() == e57::TypeString)
        {
            result->strings()[child.elementName()] =
                e57::StringNode(child).value();
        }
        else if (child.type() == e57::TypeInteger)
        {
            result->integers()[child.elementName()] =
                e57::IntegerNode(child).value();
        }
        else if (child.type() == e57::TypeFloat)
        {
            result->floats()[child.elementName()] =
                e57::FloatNode(child).value();
        }
        else if (child.type() == e57::TypeStructure)
        {
            result->addChild(
                convertE57StructureNode<E57Node>(e57::StructureNode(child)));
        }
    }

    return result;
}

E57Data3DPtr parseData3D(const e57::StructureNode& node)
{
    auto result = convertE57StructureNode<E57Data3D>(node);
    return result;
}

E57Image2DPtr parseImage2D(const e57::StructureNode& node)
{
    auto result = convertE57StructureNode<E57Image2D>(node);
    return result;
}

E57RootPtr parseRoot(const e57::StructureNode& node)
{
    auto result = convertE57StructureNode<E57Root>(node);

    if (node.isDefined("data3D"))
    {
        auto vectorData3D = e57::VectorNode(node.get("data3D"));
        for (int64_t i = 0; i < vectorData3D.childCount(); ++i)
        {
            result->data3D().emplace_back(
                parseData3D(e57::StructureNode(vectorData3D.get(i))));
        }
    }

    if (node.isDefined("images2D"))
    {
        auto vectorImages2D = e57::VectorNode(node.get("images2D"));
        for (int64_t i = 0; i < vectorImages2D.childCount(); ++i)
        {
            result->images2D().emplace_back(
                parseImage2D(e57::StructureNode(vectorImages2D.get(i))));
        }
    }

    return result;
}

E57ReaderImpl::E57ReaderImpl(const std::string& filename)
    : m_imageFile(filename, "r")
{
    parseNodeTree();
}

const E57RootPtr& E57ReaderImpl::root() const { return m_root; }

void E57ReaderImpl::parseNodeTree()
{
    e57::StructureNode root = m_imageFile.root();
    m_root = parseRoot(root);
    m_root->setName(m_imageFile.fileName());
}
