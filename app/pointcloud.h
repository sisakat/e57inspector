#ifndef POINTCLOUD_H
#define POINTCLOUD_H

#include "e57inspector/E57Node.h"
#include "scene.h"
#include "shader.h"
#include <QColor>

#include <array>
#include <vector>

enum class PointCloudViewType
{
    COLOR = 0,
    INTENSITY = 1,
    SINGLECOLOR = 2
};

struct PointCloudData
{
    std::vector<std::array<float, 3>> xyz;
    std::vector<std::array<float, 3>> normal;
    std::vector<float> intensity;
    std::vector<std::array<float, 4>> rgba;
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

    void setPointCloudData(const PointCloudData& pointCloudData);

    [[nodiscard]] const E57Data3D& data3D() const { return *m_data3D; }

private:
    E57Data3DPtr m_data3D;
    int m_pointSize{1};
    PointCloudViewType m_viewType{PointCloudViewType::COLOR};
    QColor m_singleColor;
    bool m_visible{true};
    Shader::Ptr m_shader;
    Shader::Ptr m_lineShader;

    OpenGLArrayBuffer::Ptr m_bufferXYZ;
    OpenGLArrayBuffer::Ptr m_bufferNormal;
    OpenGLArrayBuffer::Ptr m_bufferIntensity;
    OpenGLArrayBuffer::Ptr m_bufferRGBA;

    int64_t m_vao{-1};
    uint64_t m_pointCount{0};
};

#endif // POINTCLOUD_H
