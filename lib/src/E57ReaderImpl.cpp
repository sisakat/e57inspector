#include "E57ReaderImpl.h"

#include <exception>
#include <filesystem>

void E57ReaderImpl::parseFields(E57NodePtr result,
                                const e57::StructureNode& node,
                                const std::set<std::string>& ignoreFields)
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
            parseFields(item, e57::StructureNode(child));
            result->addChild(item);
        }
        else if (child.type() == e57::TypeBlob)
        {
            auto blob = e57::BlobNode(child);
            uint32_t blobId = registerBlob(blob);
            result->blobs()[blob.elementName()] = blobId;
        }
    }
}

E57Data3DPtr E57ReaderImpl::parseData3D(const e57::StructureNode& node)
{
    auto result = std::make_shared<E57Data3D>();
    parseFields(result, node);
    return result;
}

E57Image2DPtr E57ReaderImpl::parseImage2D(const e57::StructureNode& node)
{
    auto result = std::make_shared<E57Image2D>();

    if (node.isDefined("pinholeRepresentation"))
    {
        auto pinholeRepr = std::make_shared<E57PinholeRepresentation>();
        parseFields(pinholeRepr,
                    e57::StructureNode(node.get("pinholeRepresentation")));
        result->setPinholeRepresentation(pinholeRepr);
    }

    if (node.isDefined("sphericalRepresentation"))
    {
        auto sphericalRepr = std::make_shared<E57SphericalRepresentation>();
        parseFields(sphericalRepr,
                    e57::StructureNode(node.get("sphericalRepr")));
        result->setSphericalRepresentation(sphericalRepr);
    }

    if (node.isDefined("cylindricalRepresentation"))
    {
        auto cylindricalRepr = std::make_shared<E57CylindricalRepresentation>();
        parseFields(cylindricalRepr,
                    e57::StructureNode(node.get("cylindricalRepr")));
        result->setCylindricalRepresentation(cylindricalRepr);
    }

    parseFields(result, node,
                {"pinholeRepresentation", "sphericalRepresentation",
                 "cylindricalRepresentation"});
    return result;
}

E57RootPtr E57ReaderImpl::parseRoot(const e57::StructureNode& node)
{
    auto result = std::make_shared<E57Root>();
    parseFields(result, node);

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

uint32_t E57ReaderImpl::registerBlob(e57::BlobNode& blob)
{
    m_blobs.push_back(blob);
    return m_blobs.size() - 1;
}

std::vector<uint8_t> E57ReaderImpl::blobData(uint32_t blobId) const
{
    if (m_blobs.size() <= blobId)
        throw std::runtime_error("Cannot retrieve blob data. Invalid blob id.");

    auto blob = m_blobs.at(blobId);
    std::vector<uint8_t> buffer(blob.byteCount());
    blob.read(buffer.data(), 0, blob.byteCount());
    return buffer;
}
