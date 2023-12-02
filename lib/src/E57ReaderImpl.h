#ifndef E57INSPECTOR_E57READERIMPL_H
#define E57INSPECTOR_E57READERIMPL_H

#include <optional>
#include <set>
#include <string>

#include <E57Format.h>

#include <e57inspector/E57Node.h>
#include <e57inspector/E57Reader.h>

class E57DataReaderImpl
{
public:
    E57DataReaderImpl(e57::StructureNode parent,
                      e57::CompressedVectorNode node);
    ~E57DataReaderImpl();

    void bindBuffer(const std::string& identifier, float* buffer,
                    uint32_t bufferSize, uint32_t stride = 0);
    uint64_t read();

private:
    e57::StructureNode m_parent;
    e57::CompressedVectorNode m_node;
    std::optional<e57::CompressedVectorReader> m_reader;
    std::vector<e57::SourceDestBuffer> m_sourceDestBuffers;
};

class E57ReaderImpl
{
public:
    explicit E57ReaderImpl(const std::string& filename);
    [[nodiscard]] const E57RootPtr& root() const;
    [[nodiscard]] std::vector<uint8_t> blobData(uint32_t blobId) const;
    [[nodiscard]] std::vector<std::array<double, 4>>
    data(uint32_t dataId) const;
    [[nodiscard]] std::vector<E57DataInfo> dataInfo(uint32_t dataId) const;
    std::shared_ptr<E57DataReaderImpl> dataReader(uint32_t dataId);

private:
    e57::ImageFile m_imageFile;
    E57RootPtr m_root;
    std::vector<e57::BlobNode> m_blobs;
    std::vector<e57::CompressedVectorNode> m_data;

    void parseNodeTree();

    void parseFields(E57NodePtr result, const e57::StructureNode& node,
                     const std::set<std::string>& ignoreFields = {});
    E57RootPtr parseRoot(const e57::StructureNode& node);
    E57Image2DPtr parseImage2D(const e57::StructureNode& node);
    E57Data3DPtr parseData3D(const e57::StructureNode& node);

    uint32_t registerBlob(e57::BlobNode& blob);
    uint32_t registerData(e57::CompressedVectorNode& data);
};

#endif // E57INSPECTOR_E57READERIMPL_H
