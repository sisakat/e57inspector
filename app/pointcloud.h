#ifndef POINTCLOUD_H
#define POINTCLOUD_H

#include "e57inspector/E57Node.h"
#include "octree.h"
#include "scene.h"
#include "shader.h"
#include <QColor>

enum class PointCloudViewType
{
    COLOR = 0,
    INTENSITY = 1,
    SINGLECOLOR = 2
};

class PointCloud : public SceneNode
{
public:
    PointCloud(SceneNode* parent, E57Data3DPtr data3D);
    ~PointCloud() override;

    void render() override;
    void render2D(QPainter& painter) override;
    void configureShader() override;

    [[nodiscard]] int pointSize() const { return m_pointSize; }
    void setPointSize(int value) { m_pointSize = value; }

    [[nodiscard]] PointCloudViewType viewType() const { return m_viewType; }
    void setViewType(PointCloudViewType viewType) { m_viewType = viewType; }

    [[nodiscard]] QColor singleColor() const { return m_singleColor; }
    void setSingleColor(QColor singleColor) { m_singleColor = singleColor; }

    [[nodiscard]] bool visible() const { return m_visible; }
    void setVisible(bool visible) { m_visible = visible; }

    [[nodiscard]] Matrix4d sop() const { return m_sop; }
    void setSOP(const Matrix4d& sop) { m_sop = sop; }

    void insertPoints(const std::vector<PointData>& data);
    void doneInserting();

    [[nodiscard]] const E57Data3D& data3D() const { return *m_data3D; }

private:
    E57Data3DPtr m_data3D;
    Octree m_octree;
    std::vector<OctreeNode*> m_octreeNodes;
    int m_pointSize{1};
    PointCloudViewType m_viewType{PointCloudViewType::COLOR};
    QColor m_singleColor;
    Matrix4d m_sop;
    bool m_visible{true};
    Shader m_shader;
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
