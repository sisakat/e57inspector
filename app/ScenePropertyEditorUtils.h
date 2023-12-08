#ifndef E57INSPECTOR_SCENEPROPERTYEDITORUTILS_H
#define E57INSPECTOR_SCENEPROPERTYEDITORUTILS_H

#include <QColor>
#include <QTreeWidgetItem>

#include <optional>
#include <string>

#include <CBoolProperty.h>
#include <CColorProperty.h>
#include <CDoubleProperty.h>
#include <CIntegerProperty.h>
#include <CListProperty.h>

inline std::optional<int> getIntegerValue(QTreeWidgetItem* item,
                                          const std::string& id)
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

inline std::optional<double> getDoubleValue(QTreeWidgetItem* item,
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

inline std::optional<int> getListIndex(QTreeWidgetItem* item,
                                       const std::string& id)
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

inline std::optional<QColor> getColorValue(QTreeWidgetItem* item,
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

inline std::optional<bool> getBooleanValue(QTreeWidgetItem* item,
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

#endif // E57INSPECTOR_SCENEPROPERTYEDITORUTILS_H
