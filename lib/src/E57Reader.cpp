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

std::vector<E57DataInfo> E57Reader::dataInfo(uint32_t dataId) const
{
    return m_impl->dataInfo(dataId);
}

E57DataReader E57Reader::dataReader(uint32_t dataId) const
{
    return E57DataReader(m_impl->dataReader(dataId));
}

void E57DataReader::bindBuffer(const std::string& identifier, float* buffer,
                               uint32_t bufferSize, uint32_t stride)
{
    m_impl->bindBuffer(identifier, buffer, bufferSize, stride);
}

void E57DataReader::bindBuffer(const std::string& identifier, double* buffer,
                               uint32_t bufferSize, uint32_t stride)
{
    m_impl->bindBuffer(identifier, buffer, bufferSize, stride);
}

void E57DataReader::bindBuffer(const std::string& identifier, int8_t* buffer,
                               uint32_t bufferSize, uint32_t stride)
{
    m_impl->bindBuffer(identifier, buffer, bufferSize, stride);
}

void E57DataReader::bindBuffer(const std::string& identifier, int16_t* buffer,
                               uint32_t bufferSize, uint32_t stride)
{
    m_impl->bindBuffer(identifier, buffer, bufferSize, stride);
}

void E57DataReader::bindBuffer(const std::string& identifier, int32_t* buffer,
                               uint32_t bufferSize, uint32_t stride)
{
    m_impl->bindBuffer(identifier, buffer, bufferSize, stride);
}

void E57DataReader::bindBuffer(const std::string& identifier, int64_t* buffer,
                               uint32_t bufferSize, uint32_t stride)
{
    m_impl->bindBuffer(identifier, buffer, bufferSize, stride);
}

void E57DataReader::bindBuffer(const std::string& identifier, uint8_t* buffer,
                               uint32_t bufferSize, uint32_t stride)
{
    m_impl->bindBuffer(identifier, buffer, bufferSize, stride);
}

void E57DataReader::bindBuffer(const std::string& identifier, uint16_t* buffer,
                               uint32_t bufferSize, uint32_t stride)
{
    m_impl->bindBuffer(identifier, buffer, bufferSize, stride);
}

void E57DataReader::bindBuffer(const std::string& identifier, uint32_t* buffer,
                               uint32_t bufferSize, uint32_t stride)
{
    m_impl->bindBuffer(identifier, buffer, bufferSize, stride);
}

uint64_t E57DataReader::read() { return m_impl->read(); }
