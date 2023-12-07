#include "ScenePropertyEditor.h"

#include "CBaseProperty.h"
#include "CDoubleProperty.h"
#include "CIntegerProperty.h"
#include "CListProperty.h"
#include "CPropertyHeader.h"
#include "camera.h"
#include "pointcloud.h"

ScenePropertyEditor::ScenePropertyEditor(QWidget* parent)
    : CPropertyEditor(parent)
{
    connect(this, &QTreeWidget::itemChanged, this,
            &ScenePropertyEditor::onItemChanged);
    setHeaderHidden(true);
}

void ScenePropertyEditor::init(SceneNode* sceneNode)
{
    setHeaderHidden(false);
    CPropertyEditor::clear();
    CPropertyEditor::init();
    m_sceneNode = sceneNode;
    if (!m_sceneNode)
        return;

    if (dynamic_cast<Camera*>(sceneNode))
    {
        auto* camera = dynamic_cast<Camera*>(sceneNode);

        auto* viewportHeader = new CPropertyHeader("Viewport", "Viewport");
        add(viewportHeader);

        auto* viewportWidth = new CIntegerProperty(
            viewportHeader, "viewportWidth", "Width", camera->viewportWidth());
        viewportWidth->setRange(0, 10000);
        add(viewportWidth);
    }
    else if (dynamic_cast<PointCloud*>(sceneNode))
    {
        auto* pointcloud = dynamic_cast<PointCloud*>(sceneNode);
        auto* pointProperties =
            new CPropertyHeader("PointProperties", "Point Settings");
        add(pointProperties);

        auto* pointSize =
            new CIntegerProperty(pointProperties, "pointSize", "Point Size",
                                 pointcloud->pointSize());
        pointSize->setRange(1, 100);
        add(pointSize);
    }

    adjustToContents();
}

std::optional<int> getIntegerValue(QTreeWidgetItem* item, const std::string& id)
{
    if (auto* property = dynamic_cast<CIntegerProperty*>(item))
    {
        if (property->getId().toStdString() == id)
        {
            return property->getValue();
        }
    }
    return std::nullopt;
}

std::optional<double> getDoubleValue(QTreeWidgetItem* item,
                                     const std::string& id)
{
    if (auto* property = dynamic_cast<CDoubleProperty*>(item))
    {
        if (property->getId().toStdString() == id)
        {
            return property->getValue();
        }
    }
    return std::nullopt;
}

std::optional<int> getListIndex(QTreeWidgetItem* item, const std::string& id)
{
    if (auto* property = dynamic_cast<CListProperty*>(item))
    {
        if (property->getId().toStdString() == id)
        {
            return property->getIndex();
        }
    }
    return std::nullopt;
}

void ScenePropertyEditor::onItemChanged(QTreeWidgetItem* item, int column)
{
    auto* property = dynamic_cast<CBaseProperty*>(item);
    if (!m_sceneNode || !property)
        return;

    if (dynamic_cast<Camera*>(m_sceneNode))
    {
    }
    else if (dynamic_cast<PointCloud*>(m_sceneNode))
    {
        auto* pointcloud = dynamic_cast<PointCloud*>(m_sceneNode);
        auto pointSize = getIntegerValue(item, "pointSize");
        if (pointSize)
        {
            pointcloud->setPointSize(*pointSize);
        }
    }
}
