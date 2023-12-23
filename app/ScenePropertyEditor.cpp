#include "ScenePropertyEditor.h"

#include "CBaseProperty.h"
#include "CBoolProperty.h"
#include "CColorProperty.h"
#include "CDoubleProperty.h"
#include "CIntegerProperty.h"
#include "CListProperty.h"
#include "CPropertyHeader.h"

#include "Image2d.h"
#include "ScenePropertyEditorUtils.h"
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

    // clang-format off
    if      (dynamic_cast<Camera*    >(sceneNode)) initFromCamera();
    else if (dynamic_cast<PointCloud*>(sceneNode)) initFromPointcloud();
    else if (dynamic_cast<Image2d*   >(sceneNode)) initFromImage2d();
    // clang-format on

    adjustToContents();
}

void ScenePropertyEditor::onItemChanged(QTreeWidgetItem* item, int column)
{
    auto* property = dynamic_cast<CBaseProperty*>(item);
    if (!m_sceneNode || !property)
        return;

    // clang-format off
    if      (dynamic_cast<Camera*    >(m_sceneNode)) changeFromCamera(item);
    else if (dynamic_cast<PointCloud*>(m_sceneNode)) changeFromPointcloud(item);
    else if (dynamic_cast<Image2d*   >(m_sceneNode)) changeFromImage2d(item);
    // clang-format on
}

void ScenePropertyEditor::initFromCamera()
{
    auto* camera = dynamic_cast<Camera*>(m_sceneNode);

    auto* fieldOfView =
        new CDoubleProperty("fieldOfView", "Field of View",
                            camera->fieldOfView(), 90.0f, 1.0f, 179.0f);
    add(fieldOfView);

    auto* constrained = new CBoolProperty("constrained", "Constrained",
                                          camera->constrained(), true);
    add(constrained);
}

void ScenePropertyEditor::initFromPointcloud()
{
    auto* pointcloud = dynamic_cast<PointCloud*>(m_sceneNode);
    auto* pointProperties =
        new CPropertyHeader("PointProperties", "Point Settings");
    add(pointProperties);

    auto* pointSize = new CIntegerProperty(
        pointProperties, "pointSize", "Point Size", pointcloud->pointSize());
    pointSize->setRange(1, 100);
    add(pointSize);

    auto* viewProperties =
        new CPropertyHeader("ViewProperties", "View Settings");
    add(viewProperties);

    auto* visible = new CBoolProperty(viewProperties, "visible", "Visible",
                                      pointcloud->visible(), true);
    add(visible);

    CListData viewTypes;
    viewTypes.append(CListDataItem(QString("Color"), QIcon(), QVariant(0)));
    viewTypes.append(CListDataItem(QString("Intensity"), QIcon(), QVariant(1)));
    viewTypes.append(
        CListDataItem(QString("Single Color"), QIcon(), QVariant(2)));
    auto* viewType =
        new CListProperty(viewProperties, "viewType", "View Type", viewTypes,
                          static_cast<int>(pointcloud->viewType()), 0);
    add(viewType);

    auto* singleColor = new CColorProperty(viewProperties, "singleColor",
                                           "Color", pointcloud->singleColor());
    add(singleColor);
}

void ScenePropertyEditor::initFromImage2d()
{
    auto* image2d = dynamic_cast<Image2d*>(m_sceneNode);
    auto* coneLength = new CDoubleProperty(
        "coneLength", "Cone Length", image2d->coneLength(), 1.0, 0.01, 1000.0);
    add(coneLength);

    auto* visible =
        new CBoolProperty("visible", "Visible", image2d->isVisible(), true);
    add(visible);

    auto* showCoordinateSystemAxes =
        new CBoolProperty("showCoordinateSystemAxes", "Show Axes",
                          image2d->isShowCoordinateSystemAxes(), true);
    add(showCoordinateSystemAxes);

    auto* segments = new CIntegerProperty("segments", "Segments",
                                          image2d->getSegments(), 20, 4, 1000);
    add(segments);
}

void ScenePropertyEditor::changeFromCamera(QTreeWidgetItem* item)
{
    auto* camera = dynamic_cast<Camera*>(m_sceneNode);
    auto fieldOfView = getDoubleValue(item, "fieldOfView");
    if (fieldOfView)
    {
        camera->setFieldOfView(*fieldOfView);
    }
    auto constrained = getBooleanValue(item, "constrained");
    if (constrained)
    {
        camera->setConstrained(*constrained);
    }
}

void ScenePropertyEditor::changeFromPointcloud(QTreeWidgetItem* item)
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

void ScenePropertyEditor::changeFromImage2d(QTreeWidgetItem* item)
{
    auto* image2d = dynamic_cast<Image2d*>(m_sceneNode);
    auto coneLength = getDoubleValue(item, "coneLength");
    if (coneLength)
    {
        image2d->setConeLength(static_cast<float>(*coneLength));
    }

    auto visible = getBooleanValue(item, "visible");
    if (visible)
    {
        image2d->setVisible(*visible);
    }

    auto showCoordinateSystemAxes =
        getBooleanValue(item, "showCoordinateSystemAxes");
    if (showCoordinateSystemAxes)
    {
        image2d->setShowCoordinateSystemAxes(*showCoordinateSystemAxes);
    }

    auto segments = getIntegerValue(item, "segments");
    if (segments)
    {
        image2d->setSegments(*segments);
    }
}
