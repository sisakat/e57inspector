#ifndef POINTCLOUD_H
#define POINTCLOUD_H

#include "e57inspector/E57Node.h"
#include "octree.h"
#include "scene.h"
#include <QMatrix4x4>

class PointCloud : public SceneNode
{
public:
    PointCloud(SceneNode* parent, E57Data3DPtr data3D);
    ~PointCloud() override;

    void render() override;

    [[nodiscard]] int pointSize() const { return m_pointSize; }
    void setPointSize(int value) { m_pointSize = value; }

    [[nodiscard]] QMatrix4x4 sop() const { return m_sop; }
    void setSOP(const QMatrix4x4& sop) { m_sop = sop; }

    void insertPoints(const std::vector<PointData>& data);
    void doneInserting();

    [[nodiscard]] const E57Data3D& data3D() const { return *m_data3D; }

private:
    E57Data3DPtr m_data3D;
    Octree m_octree;
    std::vector<OctreeNode*> m_octreeNodes;
    int m_pointSize{1};
    QMatrix4x4 m_sop;
};

class PointCloudOctreeNode : public SceneNode
{
public:
    explicit PointCloudOctreeNode(SceneNode* parent, OctreeNode* node);
    ~PointCloudOctreeNode() override;

    void render() override;

private:
    GLuint m_vao{};
    OctreeNode* m_node;
};

#endif // POINTCLOUD_H
