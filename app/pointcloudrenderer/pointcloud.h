#ifndef POINTCLOUD_H
#define POINTCLOUD_H

#include "scene.h"
#include "octree.h"

class PointCloud : public SceneNode
{
public:
    PointCloud(Octree& octree);
    ~PointCloud() override;

    void render() override;

private:
    Octree& m_octree;
    std::vector<OctreeNode*> m_octreeNodes;
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
