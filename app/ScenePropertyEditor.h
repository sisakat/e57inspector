#ifndef E57INSPECTOR_SCENEPROPERTYEDITOR_H
#define E57INSPECTOR_SCENEPROPERTYEDITOR_H

#include "CPropertyEditor.h"
#include "scene.h"

class ScenePropertyEditor : public CPropertyEditor
{
public:
    explicit ScenePropertyEditor(QWidget* parent);

    void init(SceneNode* sceneNode);

protected:
    void onItemChanged(QTreeWidgetItem* item, int column);

private:
    SceneNode* m_sceneNode;
};

#endif // E57INSPECTOR_SCENEPROPERTYEDITOR_H
