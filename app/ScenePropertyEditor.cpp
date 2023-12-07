#include "ScenePropertyEditor.h"

#include "CBaseProperty.h"
#include "CBoolProperty.h"
#include "CColorProperty.h"
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

        auto* fieldOfView =
            new CDoubleProperty("fieldOfView", "Field of View",
                                camera->fieldOfView(), 90.0f, 1.0f, 179.0f);
        add(fieldOfView);
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

        auto* viewProperties =
            new CPropertyHeader("ViewProperties", "View Settings");
        add(viewProperties);

        auto* visible = new CBoolProperty(viewProperties, "visible", "Visible",
                                          pointcloud->visible(), true);

        CListData viewTypes;
        viewTypes.append(CListDataItem(QString("Color"), QIcon(), QVariant(0)));
        viewTypes.append(
            CListDataItem(QString("Intensity"), QIcon(), QVariant(1)));
        viewTypes.append(
            CListDataItem(QString("Single Color"), QIcon(), QVariant(2)));
        auto* viewType = new CListProperty(
            viewProperties, "viewType", "View Type", viewTypes,
            static_cast<int>(pointcloud->viewType()), 0);
        add(viewType);

        auto* singleColor = new CColorProperty(
            viewProperties, "singleColor", "Color", pointcloud->singleColor());
        add(singleColor);
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

std::optional<QColor> getColorValue(QTreeWidgetItem* item,
                                    const std::string& id)
{
    if (auto* property = dynamic_cast<CColorProperty*>(item))
    {
        if (property->getId().toStdString() == id)
        {
            return property->getColor();
        }
    }
    return std::nullopt;
}

std::optional<bool> getBooleanValue(QTreeWidgetItem* item,
                                    const std::string& id)
{
    if (auto* property = dynamic_cast<CBoolProperty*>(item))
    {
        if (property->getId().toStdString() == id)
        {
            return property->getValue();
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
        auto* camera = dynamic_cast<Camera*>(m_sceneNode);
        auto fieldOfView = getDoubleValue(item, "fieldOfView");
        if (fieldOfView)
        {
            camera->setFieldOfView(*fieldOfView);
        }
    }
    else if (dynamic_cast<PointCloud*>(m_sceneNode))
    {
        auto* pointcloud = dynamic_cast<PointCloud*>(m_sceneNode);
        auto pointSize = getIntegerValue(item, "pointSize");
        if (pointSize)
        {
            pointcloud->setPointSize(*pointSize);
        }

        auto visible = getBooleanValue(item, "visible");
        if (visible)
        {
            pointcloud->setVisible(*visible);
        }

        auto viewType = getListIndex(item, "viewType");
        if (viewType)
        {
            pointcloud->setViewType(static_cast<PointCloudViewType>(*viewType));
        }

        auto singleColor = getColorValue(item, "singleColor");
        if (singleColor)
        {
            pointcloud->setSingleColor(*singleColor);
        }
    }
}
