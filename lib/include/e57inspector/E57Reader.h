#ifndef E57INSPECTOR_E57READER_H
#define E57INSPECTOR_E57READER_H

#include <string>

#include "E57Node.h"

enum class E57DataType
{
    INTEGER,
    FLOAT,
    DOUBLE
};

struct E57DataInfo
{
    std::string identifier;
    E57DataType dataType;
    double minValue;
    double maxValue;
};

class E57ReaderImpl;
class E57Reader
{
public:
    explicit E57Reader(const std::string& filename);
    ~E57Reader();

    [[nodiscard]] const E57RootPtr& root() const;
    [[nodiscard]] std::vector<uint8_t> blobData(uint32_t blobId) const;
    [[nodiscard]] std::vector<std::array<double, 4>> data(uint32_t dataId) const;
    [[nodiscard]] std::vector<E57DataInfo> dataInfo(uint32_t dataId) const;

private:
    E57ReaderImpl* m_impl;
};

#endif // E57INSPECTOR_E57READER_H
