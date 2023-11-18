#ifndef E57INSPECTOR_E57NODE_H
#define E57INSPECTOR_E57NODE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class E57Node
{
public:
    using strings_t = std::unordered_map<std::string, std::string>;
    using integers_t = std::unordered_map<std::string, int64_t>;
    using floats_t = std::unordered_map<std::string, double>;

    E57Node() = default;
    virtual ~E57Node() = default;

    [[nodiscard]] std::string name() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

    [[nodiscard]] const strings_t& strings() const { return m_strings; }
    [[nodiscard]] const integers_t& integers() const { return m_integers; }
    [[nodiscard]] const floats_t& floats() const { return m_floats; }

    [[nodiscard]] strings_t& strings() { return m_strings; }
    [[nodiscard]] integers_t& integers() { return m_integers; }
    [[nodiscard]] floats_t& floats() { return m_floats; }

    [[nodiscard]] std::string
    getString(const std::string& name,
              const std::string& defaultValue = "") const
    {
        if (m_strings.find(name) != m_strings.end())
        {
            return m_strings.at(name);
        }
        return defaultValue;
    }

    [[nodiscard]] int64_t getInteger(const std::string& name) const
    {
        return m_integers.at(name);
    }

    [[nodiscard]] double getDouble(const std::string& name) const
    {
        return m_floats.at(name);
    }

    [[nodiscard]] const std::vector<std::shared_ptr<E57Node>>& children() const
    {
        return m_children;
    }

    [[nodiscard]] size_t childCount() const { return m_children.size(); }

    void addChild(const std::shared_ptr<E57Node>& node)
    {
        m_children.push_back(node);
    }

private:
    std::string m_name;
    strings_t m_strings;
    integers_t m_integers;
    floats_t m_floats;
    std::vector<std::shared_ptr<E57Node>> m_children;
};

class E57Data3D : public E57Node
{
public:
private:
};

class E57Image2D : public E57Node
{
public:
private:
};

using E57NodePtr = std::shared_ptr<E57Node>;
using E57Data3DPtr = std::shared_ptr<E57Data3D>;
using E57Image2DPtr = std::shared_ptr<E57Image2D>;

class E57Root : public E57Node
{
public:
    [[nodiscard]] const std::vector<E57Data3DPtr>& data3D() const
    {
        return m_data3d;
    }

    [[nodiscard]] std::vector<E57Data3DPtr>& data3D() { return m_data3d; }

    [[nodiscard]] const std::vector<E57Image2DPtr>& images2D() const
    {
        return m_images2d;
    }

    [[nodiscard]] std::vector<E57Image2DPtr>& images2D() { return m_images2d; }

private:
    std::vector<E57Data3DPtr> m_data3d;
    std::vector<E57Image2DPtr> m_images2d;
};

using E57RootPtr = std::shared_ptr<E57Root>;

#endif // E57INSPECTOR_E57NODE_H
