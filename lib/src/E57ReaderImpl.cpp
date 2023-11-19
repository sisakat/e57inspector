#include "E57ReaderImpl.h"

#include <filesystem>
#include <set>

template <typename T>
void parseFields(const std::shared_ptr<T>& result,
                 const e57::StructureNode& node,
                 const std::set<std::string>& ignoreFields = {})
{
    result->setName(node.elementName());

    if (node.isDefined("name"))
    {
        result->setName(e57::StringNode(node.get("name")).value());
    }

    for (int64_t i = 0; i < node.childCount(); ++i)
    {
        auto child = node.get(i);
        if (ignoreFields.contains(child.elementName()))
        {
            continue;
        }
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
            auto item = std::make_shared<E57Node>();
            parseFields<E57Node>(item, e57::StructureNode(child));
            result->addChild(item);
        }
    }
}

E57Data3DPtr parseData3D(const e57::StructureNode& node)
{
    auto result = std::make_shared<E57Data3D>();
    parseFields<E57Data3D>(result, node);
    return result;
}

E57Image2DPtr parseImage2D(const e57::StructureNode& node)
{
    auto result = std::make_shared<E57Image2D>();

    if (node.isDefined("pinholeRepresentation"))
    {
        auto pinholeRepr = std::make_shared<E57PinholeRepresentation>();
        parseFields<E57PinholeRepresentation>(
            pinholeRepr, e57::StructureNode(node.get("pinholeRepresentation")));
        result->setPinholeRepresentation(pinholeRepr);
    }

    if (node.isDefined("sphericalRepresentation"))
    {
        auto sphericalRepr = std::make_shared<E57SphericalRepresentation>();
        parseFields<E57SphericalRepresentation>(
            sphericalRepr, e57::StructureNode(node.get("sphericalRepr")));
        result->setSphericalRepresentation(sphericalRepr);
    }

    if (node.isDefined("cylindricalRepresentation"))
    {
        auto cylindricalRepr = std::make_shared<E57CylindricalRepresentation>();
        parseFields<E57CylindricalRepresentation>(
            cylindricalRepr, e57::StructureNode(node.get("cylindricalRepr")));
        result->setCylindricalRepresentation(cylindricalRepr);
    }

    parseFields<E57Image2D>(result, node,
                            {"pinholeRepresentation", "sphericalRepresentation",
                             "cylindricalRepresentation"});
    return result;
}

E57RootPtr parseRoot(const e57::StructureNode& node)
{
    auto result = std::make_shared<E57Root>();
    parseFields<E57Root>(result, node);

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
    m_root->setName(
        std::filesystem::path(m_imageFile.fileName()).stem().string());
}
