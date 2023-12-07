#ifndef OCTREE_H
#define OCTREE_H

#include <array>
#include <memory>
#include <vector>

struct UVW
{
    uint32_t u;
    uint32_t v;
    uint32_t w;
};

struct PointData
{
    std::array<float, 3> xyz;
    std::array<float, 3> rgb;
    float intensity;
};

class OctreeElement
{
public:
    PointData data;

    [[nodiscard]] UVW getUVW(const double resolution,
                             const uint32_t depth) const
    {
        const uint32_t offset = 1 << 31;
        uint32_t u = static_cast<uint32_t>(data.xyz[0] / resolution + offset) >>
                         (31 - depth) &
                     1;
        uint32_t v = static_cast<uint32_t>(data.xyz[1] / resolution + offset) >>
                         (31 - depth) &
                     1;
        uint32_t w = static_cast<uint32_t>(data.xyz[2] / resolution + offset) >>
                         (31 - depth) &
                     1;
        return {u, v, w};
    }
};

class OctreeNode
{
public:
    OctreeNode(uint32_t elementLimit, double resolution);

    void insert(const std::vector<OctreeElement>& elements,
                uint32_t currentDepth);
    bool isLeaf() const;
    void finalize();

    OctreeNode* child(uint32_t index) { return m_childNodes[index].get(); }

    [[nodiscard]] const OctreeNode* child(uint32_t index) const
    {
        return m_childNodes[index].get();
    }

    std::vector<OctreeElement>& elements() { return m_elements; }

private:
    uint32_t m_elementLimit;
    double m_resolution;

    std::array<std::unique_ptr<OctreeNode>, 8> m_childNodes;
    std::vector<OctreeElement> m_elements;

    [[nodiscard]] int getOnlyChildIndex() const;
};

class Octree
{
public:
    Octree(uint32_t elementLimit = 10000, double resolution = 0.001);

    void insert(const std::vector<PointData>& elements);
    void finalize();

    OctreeNode& root();
    const OctreeNode& root() const;

private:
    uint32_t m_elementLimit;
    double m_resolution;
    std::unique_ptr<OctreeNode> m_root;
};

#endif // OCTREE_H
