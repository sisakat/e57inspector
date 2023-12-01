#include <e57inspector/E57Reader.h>

#include "E57ReaderImpl.h"

E57Reader::E57Reader(const std::string& filename)
    : m_impl{new E57ReaderImpl(filename)}
{
}

E57Reader::~E57Reader() { delete m_impl; }

const E57RootPtr& E57Reader::root() const { return m_impl->root(); }

std::vector<uint8_t> E57Reader::blobData(uint32_t blobId) const
{
    return m_impl->blobData(blobId);
}

std::vector<std::array<double, 4>> E57Reader::data(uint32_t dataId) const
{
    return m_impl->data(dataId);
}

std::vector<E57DataInfo> E57Reader::dataInfo(uint32_t dataId) const
{
    return m_impl->dataInfo(dataId);
}
