#ifndef E57INSPECTOR_E57READERIMPL_H
#define E57INSPECTOR_E57READERIMPL_H

#include <set>
#include <string>

#include <E57Format.h>

#include <e57inspector/E57Node.h>

class E57ReaderImpl
{
public:
    explicit E57ReaderImpl(const std::string& filename);
    [[nodiscard]] const E57RootPtr& root() const;
    [[nodiscard]] std::vector<uint8_t> blobData(uint32_t blobId) const;

private:
    e57::ImageFile m_imageFile;
    E57RootPtr m_root;
    std::vector<e57::BlobNode> m_blobs;

    void parseNodeTree();

    void parseFields(E57NodePtr result, const e57::StructureNode& node,
                     const std::set<std::string>& ignoreFields = {});
    E57RootPtr parseRoot(const e57::StructureNode& node);
    E57Image2DPtr parseImage2D(const e57::StructureNode& node);
    E57Data3DPtr parseData3D(const e57::StructureNode& node);

    uint32_t registerBlob(e57::BlobNode& blob);
};

#endif // E57INSPECTOR_E57READERIMPL_H
