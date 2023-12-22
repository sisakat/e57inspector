#include "E57ReaderImpl.h"

#include <exception>
#include <filesystem>

#include "E57Utils.h"

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
        else if (child.type() == e57::TypeScaledInteger)
        {
            result->integers()[child.elementName()] =
                e57::ScaledIntegerNode(child).rawValue();
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
        else if (child.type() == e57::TypeCompressedVector)
        {
            auto compressedVectorNode = e57::CompressedVectorNode(child);
            std::string elementName = compressedVectorNode.elementName();
            elementName[0] = static_cast<char>(std::toupper(elementName[0]));
            result->integers()["Num" + elementName] =
                compressedVectorNode.childCount();
            uint32_t dataId = registerData(compressedVectorNode);
            result->data()[compressedVectorNode.elementName()] = dataId;

            auto item = std::make_shared<E57Node>();
            parseFields(item,
                        e57::StructureNode(compressedVectorNode.prototype()));
            item->setName(compressedVectorNode.elementName());
            result->addChild(item);
        }
    }
}

E57Data3DPtr E57ReaderImpl::parseData3D(const e57::StructureNode& node)
{
    auto result = std::make_shared<E57Data3D>();
    parseFields(result, node);

    if (isDefined(node, "pose"))
    {
        auto pose = e57::StructureNode(node.get("pose"));
        if (isDefined(pose, "translation"))
        {
            result->pose().translation[0] =
                e57::FloatNode(pose.get("translation/x")).value();
            result->pose().translation[1] =
                e57::FloatNode(pose.get("translation/y")).value();
            result->pose().translation[2] =
                e57::FloatNode(pose.get("translation/z")).value();
        }
        else
        {
            result->pose().translation[0] = 0.0;
            result->pose().translation[1] = 0.0;
            result->pose().translation[2] = 0.0;
        }

        if (isDefined(pose, "rotation"))
        {
            result->pose().rotation.x =
                e57::FloatNode(pose.get("rotation/x")).value();
            result->pose().rotation.y =
                e57::FloatNode(pose.get("rotation/y")).value();
            result->pose().rotation.z =
                e57::FloatNode(pose.get("rotation/z")).value();
            result->pose().rotation.w =
                e57::FloatNode(pose.get("rotation/w")).value();
        }
        else
        {
            result->pose().rotation.x = 0.0;
            result->pose().rotation.y = 0.0;
            result->pose().rotation.z = 0.0;
            result->pose().rotation.w = 0.0;
        }
    }

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
                    e57::StructureNode(node.get("sphericalRepresentation")));
        result->setSphericalRepresentation(sphericalRepr);
    }

    if (node.isDefined("cylindricalRepresentation"))
    {
        auto cylindricalRepr = std::make_shared<E57CylindricalRepresentation>();
        parseFields(cylindricalRepr,
                    e57::StructureNode(node.get("cylindricalRepresentation")));
        result->setCylindricalRepresentation(cylindricalRepr);
    }

    parseFields(result, node,
                {"pinholeRepresentation", "sphericalRepresentation",
                 "cylindricalRepresentation"});

    if (isDefined(node, "pose"))
    {
        auto pose = e57::StructureNode(node.get("pose"));
        if (isDefined(pose, "translation"))
        {
            result->pose().translation[0] =
                e57::FloatNode(pose.get("translation/x")).value();
            result->pose().translation[1] =
                e57::FloatNode(pose.get("translation/y")).value();
            result->pose().translation[2] =
                e57::FloatNode(pose.get("translation/z")).value();
        }
        else
        {
            result->pose().translation[0] = 0.0;
            result->pose().translation[1] = 0.0;
            result->pose().translation[2] = 0.0;
        }

        if (isDefined(pose, "rotation"))
        {
            result->pose().rotation.x =
                e57::FloatNode(pose.get("rotation/x")).value();
            result->pose().rotation.y =
                e57::FloatNode(pose.get("rotation/y")).value();
            result->pose().rotation.z =
                e57::FloatNode(pose.get("rotation/z")).value();
            result->pose().rotation.w =
                e57::FloatNode(pose.get("rotation/w")).value();
        }
        else
        {
            result->pose().rotation.x = 0.0;
            result->pose().rotation.y = 0.0;
            result->pose().rotation.z = 0.0;
            result->pose().rotation.w = 0.0;
        }
    }

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

uint32_t E57ReaderImpl::registerData(e57::CompressedVectorNode& data)
{
    m_data.push_back(data);
    return m_data.size() - 1;
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

std::vector<E57DataInfo> E57ReaderImpl::dataInfo(uint32_t dataId) const
{
    if (m_data.size() <= dataId)
        throw std::runtime_error("Cannot retrieve data. Invalid data id.");

    auto data = m_data.at(dataId);
    auto prototype = e57::StructureNode(data.prototype());

    std::vector<E57DataInfo> result;

    for (uint64_t i = 0; i < prototype.childCount(); ++i)
    {
        auto child = prototype.get(i);

        E57DataInfo dataInfo;
        dataInfo.identifier = child.elementName();

        switch (child.type())
        {
        case e57::TypeInteger:
        {
            auto c = e57::IntegerNode(child);
            dataInfo.dataType = E57DataType::INTEGER;
            dataInfo.minValue = c.minimum();
            dataInfo.maxValue = c.maximum();
            break;
        }
        case e57::TypeScaledInteger:
        {
            auto c = e57::ScaledIntegerNode(child);
            dataInfo.dataType = E57DataType::FLOAT;
            dataInfo.minValue = c.scaledMinimum();
            dataInfo.maxValue = c.scaledMaximum();
            break;
        }
        case e57::TypeFloat:
        {
            auto c = e57::FloatNode(child);
            dataInfo.dataType = E57DataType::FLOAT;
            dataInfo.minValue = c.minimum();
            dataInfo.maxValue = c.maximum();
            break;
        }
        default:
            continue;
        }

        result.push_back(dataInfo);
    }

    return result;
}

std::shared_ptr<E57DataReaderImpl> E57ReaderImpl::dataReader(uint32_t dataId)
{
    if (m_data.size() <= dataId)
        throw std::runtime_error("Cannot retrieve data. Invalid data id.");
    auto data = m_data.at(dataId);
    return std::make_shared<E57DataReaderImpl>(
        e57::StructureNode(data.parent()), data);
}

E57DataReaderImpl::E57DataReaderImpl(e57::StructureNode parent,
                                     e57::CompressedVectorNode node)
    : m_parent{std::move(parent)}, m_node{std::move(node)},
      m_reader{std::nullopt}
{
}

void E57DataReaderImpl::bindBuffer(const std::string& identifier, float* buffer,
                                   uint32_t bufferSize, uint32_t stride)
{
    auto prototype = e57::StructureNode(m_node.prototype());

    if (isDefined(prototype, identifier))
    {
        m_sourceDestBuffers.emplace_back(m_node.destImageFile(), identifier,
                                         buffer, bufferSize, true, true,
                                         stride);
    }
}

void E57DataReaderImpl::bindBuffer(const std::string& identifier,
                                   double* buffer, uint32_t bufferSize,
                                   uint32_t stride)
{
    auto prototype = e57::StructureNode(m_node.prototype());

    if (isDefined(prototype, identifier))
    {
        m_sourceDestBuffers.emplace_back(m_node.destImageFile(), identifier,
                                         buffer, bufferSize, true, true,
                                         stride);
    }
}

void E57DataReaderImpl::bindBuffer(const std::string& identifier,
                                   int8_t* buffer, uint32_t bufferSize,
                                   uint32_t stride)
{
    auto prototype = e57::StructureNode(m_node.prototype());

    if (isDefined(prototype, identifier))
    {
        m_sourceDestBuffers.emplace_back(m_node.destImageFile(), identifier,
                                         buffer, bufferSize, true, true,
                                         stride);
    }
}

void E57DataReaderImpl::bindBuffer(const std::string& identifier,
                                   int16_t* buffer, uint32_t bufferSize,
                                   uint32_t stride)
{
    auto prototype = e57::StructureNode(m_node.prototype());

    if (isDefined(prototype, identifier))
    {
        m_sourceDestBuffers.emplace_back(m_node.destImageFile(), identifier,
                                         buffer, bufferSize, true, true,
                                         stride);
    }
}

void E57DataReaderImpl::bindBuffer(const std::string& identifier,
                                   int32_t* buffer, uint32_t bufferSize,
                                   uint32_t stride)
{
    auto prototype = e57::StructureNode(m_node.prototype());

    if (isDefined(prototype, identifier))
    {
        m_sourceDestBuffers.emplace_back(m_node.destImageFile(), identifier,
                                         buffer, bufferSize, true, true,
                                         stride);
    }
}

void E57DataReaderImpl::bindBuffer(const std::string& identifier,
                                   int64_t* buffer, uint32_t bufferSize,
                                   uint32_t stride)
{
    auto prototype = e57::StructureNode(m_node.prototype());

    if (isDefined(prototype, identifier))
    {
        m_sourceDestBuffers.emplace_back(m_node.destImageFile(), identifier,
                                         buffer, bufferSize, true, true,
                                         stride);
    }
}

void E57DataReaderImpl::bindBuffer(const std::string& identifier,
                                   uint8_t* buffer, uint32_t bufferSize,
                                   uint32_t stride)
{
    auto prototype = e57::StructureNode(m_node.prototype());

    if (isDefined(prototype, identifier))
    {
        m_sourceDestBuffers.emplace_back(m_node.destImageFile(), identifier,
                                         buffer, bufferSize, true, true,
                                         stride);
    }
}

void E57DataReaderImpl::bindBuffer(const std::string& identifier,
                                   uint16_t* buffer, uint32_t bufferSize,
                                   uint32_t stride)
{
    auto prototype = e57::StructureNode(m_node.prototype());

    if (isDefined(prototype, identifier))
    {
        m_sourceDestBuffers.emplace_back(m_node.destImageFile(), identifier,
                                         buffer, bufferSize, true, true,
                                         stride);
    }
}

void E57DataReaderImpl::bindBuffer(const std::string& identifier,
                                   uint32_t* buffer, uint32_t bufferSize,
                                   uint32_t stride)
{
    auto prototype = e57::StructureNode(m_node.prototype());

    if (isDefined(prototype, identifier))
    {
        m_sourceDestBuffers.emplace_back(m_node.destImageFile(), identifier,
                                         buffer, bufferSize, true, true,
                                         stride);
    }
}

uint64_t E57DataReaderImpl::read()
{
    if (!m_reader.has_value())
    {
        m_reader = m_node.reader(m_sourceDestBuffers);
    }
    if (!m_reader->isOpen())
    {
        throw std::runtime_error("Reader not open.");
    }
    if (!m_node.isAttached())
    {
        throw std::runtime_error("Node is not attached.");
    }

    return m_reader->read(m_sourceDestBuffers);
}

E57DataReaderImpl::~E57DataReaderImpl()
{
    if (m_reader)
    {
        m_reader->close();
    }
}
