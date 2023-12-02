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
class E57DataReader;
class E57Reader
{
public:
    explicit E57Reader(const std::string& filename);
    ~E57Reader();

    [[nodiscard]] const E57RootPtr& root() const;
    [[nodiscard]] std::vector<uint8_t> blobData(uint32_t blobId) const;
    [[nodiscard]] std::vector<E57DataInfo> dataInfo(uint32_t dataId) const;
    [[nodiscard]] E57DataReader dataReader(uint32_t dataId) const;

private:
    E57ReaderImpl* m_impl;
};

class E57DataReaderImpl;
class E57DataReader
{
public:
    void bindBuffer(const std::string& identifier, float* buffer,
                    uint32_t bufferSize, uint32_t stride = 0);
    void bindBuffer(const std::string& identifier, double* buffer,
                    uint32_t bufferSize, uint32_t stride = 0);

    void bindBuffer(const std::string& identifier, int8_t* buffer,
                    uint32_t bufferSize, uint32_t stride = 0);
    void bindBuffer(const std::string& identifier, int16_t* buffer,
                    uint32_t bufferSize, uint32_t stride = 0);
    void bindBuffer(const std::string& identifier, int32_t* buffer,
                    uint32_t bufferSize, uint32_t stride = 0);
    void bindBuffer(const std::string& identifier, int64_t* buffer,
                    uint32_t bufferSize, uint32_t stride = 0);

    void bindBuffer(const std::string& identifier, uint8_t* buffer,
                    uint32_t bufferSize, uint32_t stride = 0);
    void bindBuffer(const std::string& identifier, uint16_t* buffer,
                    uint32_t bufferSize, uint32_t stride = 0);
    void bindBuffer(const std::string& identifier, uint32_t* buffer,
                    uint32_t bufferSize, uint32_t stride = 0);

    uint64_t read();

    friend class E57Reader;

private:
    std::shared_ptr<E57DataReaderImpl> m_impl;

    explicit E57DataReader(std::shared_ptr<E57DataReaderImpl> impl)
        : m_impl(std::move(impl))
    {
    }
};

#endif // E57INSPECTOR_E57READER_H
