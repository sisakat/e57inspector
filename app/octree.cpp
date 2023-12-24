#include "octree.h"

#include <cassert>
#include <iterator>
#include <limits>

const auto doubleMin = std::numeric_limits<double>::min();
const auto doubleMax = std::numeric_limits<double>::max();

uint32_t calculateChildIndex(const OctreeElement& element,
                             const double resolution, const uint32_t depth)
{
    UVW uvw = element.getUVW(resolution, depth);
    return 4 * uvw.w + 2 * uvw.v + uvw.u;
}

OctreeNode::OctreeNode(uint32_t elementLimit, double resolution)
    : m_elementLimit{elementLimit}, m_resolution{resolution}
{
}

void OctreeNode::insert(const std::vector<OctreeElement>& elements,
                        uint32_t currentDepth)
{
    if (elements.size() <= m_elementLimit || currentDepth == 31)
    {
        // store the points - making it a leaf
        std::copy(elements.begin(), elements.end(),
                  std::back_inserter(m_elements));
        return;
    }

    uint32_t childPointCounts[8]{};

    for (auto& element : elements)
    {
        const uint32_t i =
            calculateChildIndex(element, m_resolution, currentDepth);
        assert(i < 8);
        childPointCounts[i]++;
    }

    for (size_t i = 0; i < 8; ++i)
    {
        if (!childPointCounts[i])
            continue;

        if (!m_childNodes[i])
            m_childNodes[i] =
                std::make_unique<OctreeNode>(m_elementLimit, m_resolution);

        std::vector<OctreeElement> newElements;
        for (const auto& element : elements)
        {
            if (calculateChildIndex(element, m_resolution, currentDepth) == i)
            {
                newElements.push_back(element);
            }
        }

        m_childNodes[i]->insert(newElements, currentDepth + 1);
    }
}

bool OctreeNode::isLeaf() const
{
    return !m_elements.empty();
}

void OctreeNode::finalize()
{
    for (auto& child : m_childNodes)
    {
        if (child)
            child->finalize();
    }

    int onlyChildIdx = getOnlyChildIndex();
    if (onlyChildIdx != -1)
    {
        std::unique_ptr<OctreeNode> child(
            std::move(m_childNodes[onlyChildIdx]));
        m_childNodes[onlyChildIdx] = nullptr;
        for (size_t i = 0; i < 8; ++i)
        {
            this->m_childNodes[i] = std::move(child->m_childNodes[i]);
        }
    }

    if (isLeaf())
        updateBoundingBox();
}

int OctreeNode::getOnlyChildIndex() const
{
    int result = -1;
    for (int i = 0; i < 8; i++)
    {
        if (result != -1 && m_childNodes[i] != nullptr)
        {
            return -1;
        }
        if (m_childNodes[i] != nullptr)
        {
            result = i;
        }
    }
    return result;
}

void OctreeNode::updateBoundingBox()
{
    m_boundingBox.reset();
    for (const auto& element : m_elements)
    {
        Vector3d xyz(element.data.xyz[0], element.data.xyz[1],
                     element.data.xyz[2]);
        m_boundingBox.update(xyz);
    }
}

Octree::Octree(uint32_t elementLimit, double resolution)
    : m_elementLimit{elementLimit}, m_resolution{resolution},
      m_root{std::make_unique<OctreeNode>(m_elementLimit, m_resolution)}
{
}

void Octree::insert(const std::vector<PointData>& elements)
{
    std::vector<OctreeElement> octreeElements(elements.size());
    for (size_t i = 0; i < elements.size(); ++i)
    {
        octreeElements[i].data = elements[i];
    }
    m_root->insert(octreeElements, 0);
}

void Octree::finalize()
{
    m_root->finalize();
}

OctreeNode& Octree::root()
{
    return *m_root;
}

const OctreeNode& Octree::root() const
{
    return *m_root;
}
