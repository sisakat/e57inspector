#ifndef E57INSPECTOR_E57READERIMPL_H
#define E57INSPECTOR_E57READERIMPL_H

#include <string>

#include <E57Format.h>

#include <e57inspector/E57Node.h>

class E57ReaderImpl
{
public:
    explicit E57ReaderImpl(const std::string& filename);
    [[nodiscard]] const E57RootPtr& root() const;

private:
    e57::ImageFile m_imageFile;
    E57RootPtr m_root;

    void parseNodeTree();
};

#endif // E57INSPECTOR_E57READERIMPL_H
