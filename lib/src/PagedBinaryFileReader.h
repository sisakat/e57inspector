//
// Created by Stefan Isak on 03.05.2024.
//

#ifndef E57INSPECTOR_PAGEDBINARYFILEREADER_H
#define E57INSPECTOR_PAGEDBINARYFILEREADER_H

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

class PagedBinaryFileReader
{
public:
    explicit PagedBinaryFileReader(const std::string& filename,
                                   int64_t pageSize = 1024,
                                   int64_t payloadSize = 1020);

    bool readBytes(std::vector<char>& bytes, int64_t numBytes);
    void seek(std::streampos pos);

private:
    std::ifstream m_file;
    int64_t m_pageSize = 1024;
    int64_t m_payloadSize = 1020;

    std::vector<char> m_buffer;
    int m_currentPayloadIndex{};
    bool m_endOfFile{};

    void fillBuffer();
};

#endif // E57INSPECTOR_PAGEDBINARYFILEREADER_H
