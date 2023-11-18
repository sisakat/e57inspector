#ifndef E57INSPECTOR_E57READER_H
#define E57INSPECTOR_E57READER_H

#include <string>

#include "E57Node.h"

class E57ReaderImpl;
class E57Reader
{
public:
    explicit E57Reader(const std::string& filename);
    ~E57Reader();

    [[nodiscard]] const E57RootPtr& root() const;

private:
    E57ReaderImpl* m_impl;
};

#endif // E57INSPECTOR_E57READER_H
