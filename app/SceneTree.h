#ifndef E57INSPECTOR_SCENETREE_H
#define E57INSPECTOR_SCENETREE_H

#include <QTreeWidget>

#include "ScenePropertyEditor.h"
#include "SceneTreeNodeFactory.h"
#include "scene.h"

class SceneTree : public QTreeWidget
{
    Q_OBJECT
public:
    explicit SceneTree(QWidget* parent = nullptr);

    void init(Scene& scene);

    void setScenePropertyEditor(ScenePropertyEditor* scenePropertyEditor);

protected:
    void onItemClicked(QTreeWidgetItem* item, int column);

private:
    ScenePropertyEditor* m_scenePropertyEditor{nullptr};
};

#endif // E57INSPECTOR_SCENETREE_H
