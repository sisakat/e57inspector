#ifndef POINTCLOUD_H
#define POINTCLOUD_H

#include "octree.h"
#include "scene.h"

class PointCloud : public SceneNode
{
public:
    PointCloud(Octree& octree);
    ~PointCloud() override;

    void render() override;

    [[nodiscard]] int pointSize() const { return m_pointSize; }
    void setPointSize(int value) { m_pointSize = value; }

private:
    Octree& m_octree;
    std::vector<OctreeNode*> m_octreeNodes;
    int m_pointSize{1};
};

class PointCloudOctreeNode : public SceneNode
{
public:
    PointCloudOctreeNode(OctreeNode* node);
    ~PointCloudOctreeNode() override;

    void render() override;

private:
    GLuint m_vao;
    OctreeNode* m_node;
};

#endif // POINTCLOUD_H
