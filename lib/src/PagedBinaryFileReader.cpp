#include "PagedBinaryFileReader.h"

PagedBinaryFileReader::PagedBinaryFileReader(const std::string& filename,
                                             int64_t pageSize,
                                             int64_t payloadSize)
    : m_file(filename, std::ios::binary), m_pageSize(pageSize),
      m_payloadSize(payloadSize)
{
    if (!m_file)
    {
        throw std::runtime_error("Error opening file for reading.");
    }
    fillBuffer();
}

bool PagedBinaryFileReader::readBytes(std::vector<char>& bytes,
                                      int64_t numBytes)
{
    bytes.clear();
    while (numBytes > 0 && !m_endOfFile)
    {
        int64_t bytesToRead = numBytes < m_payloadSize - m_currentPayloadIndex
                                  ? numBytes
                                  : m_payloadSize - m_currentPayloadIndex;
        for (int i = 0; i < bytesToRead; ++i)
        {
            bytes.push_back(m_buffer[m_currentPayloadIndex]);
            ++m_currentPayloadIndex;
        }
        numBytes -= bytesToRead;
        if (m_currentPayloadIndex == m_payloadSize)
        {
            fillBuffer();
        }
    }
    return !bytes.empty();
}

void PagedBinaryFileReader::seek(std::streampos pos)
{
    std::streampos pageStart = pos / m_pageSize * m_pageSize;
    std::streampos pageOffset = pos % m_pageSize;

    m_file.seekg(pageStart, std::ios::beg);
    fillBuffer();

    this->m_currentPayloadIndex += pageOffset;
}

void PagedBinaryFileReader::fillBuffer()
{
    m_buffer.clear();
    char byte;
    for (int i = 0; i < m_pageSize; ++i)
    {
        m_file.get(byte);
        m_buffer.push_back(byte);
    }
    m_currentPayloadIndex = 0;
    m_endOfFile = m_file.eof();
}
