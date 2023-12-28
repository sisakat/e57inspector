#ifndef E57INSPECTOR_MAINWINDOW_H
#define E57INSPECTOR_MAINWINDOW_H

#include <QMainWindow>
#include <QProgressBar>

#include <e57inspector/E57Reader.h>

#include "E57TreeNode.h"
#include "SceneView.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private slots:
    void actionOpen_triggered();
    void actionClose_triggered();
    void actionAbout_triggered();
    void actionCamera_Top_triggered();
    void actionCamera_Bottom_triggered();
    void actionCamera_Left_triggered();
    void actionCamera_Right_triggered();
    void actionCamera_Front_triggered();
    void actionCamera_Back_triggered();
    void twMain_nodeSelected(TNode* node);
    void tabWidget_tabClosesRequested(int index);
    void tabWidget_currentChanged(int index);
    void twMain_itemDoubleClicked(QTreeWidgetItem* item, int column);
    void twViewProperties_itemChanged(QTreeWidgetItem* item, int column);
    void sceneView_itemDropped(SceneView* sender, QObject* source);

private:
    Ui::MainWindow* ui;
    std::unique_ptr<E57Reader> m_reader;

    void openFile();
    void loadE57(const std::string& filename);
    void openImage(const E57Image2D& node, const std::string& tabName);
    SceneView* createSceneView(const std::string& name = "New View");

    SceneView* findSceneView();
};

#endif // E57INSPECTOR_MAINWINDOW_H
