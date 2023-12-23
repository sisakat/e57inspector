#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "E57TreeNode.h"
#include "E57Utils.h"
#include "Image2d.h"
#include "SceneView.h"
#include "about.h"
#include "siimageviewer.h"
#include "welcome.h"

#include <QBuffer>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QImageReader>
#include <QMimeData>

static const int BUFFER_SIZE = 10000;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setAcceptDrops(true);

    connect(ui->actionOpen, &QAction::triggered, this,
            &MainWindow::actionOpen_triggered);
    connect(ui->actionClose, &QAction::triggered, this,
            &MainWindow::actionClose_triggered);
    connect(ui->actionAbout, &QAction::triggered, this,
            &MainWindow::actionAbout_triggered);
    connect(ui->actionCamera_Top, &QAction::triggered, this,
            &MainWindow::actionCamera_Top_triggered);
    connect(ui->twMain, &E57Tree::nodeSelected, this,
            &MainWindow::twMain_nodeSelected);
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this,
            &MainWindow::tabWidget_tabClosesRequested);
    connect(ui->twMain, &E57Tree::itemDoubleClicked, this,
            &MainWindow::twMain_itemDoubleClicked);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this,
            &MainWindow::tabWidget_currentChanged);
    connect(ui->twViewProperties, &ScenePropertyEditor::itemChanged, this,
            &MainWindow::twViewProperties_itemChanged);

    ui->twScene->setScenePropertyEditor(ui->twViewProperties);
    ui->tabWidget->addTab(new Welcome(ui->tabWidget), tr("Welcome"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::actionOpen_triggered()
{
    openFile();
}

void MainWindow::actionClose_triggered()
{
    close();
}

void MainWindow::actionAbout_triggered()
{
    auto* about = new About(this);
    about->setModal(true);
    about->show();
}

void MainWindow::actionCamera_Top_triggered()
{
    auto sceneView = findSceneView();
    if (sceneView)
    {
        auto camera = sceneView->scene().findNode<Camera>();
        if (camera)
        {
            camera->topView();
        }
        sceneView->update();
    }
}

void MainWindow::openFile()
{
    QFileDialog dialog(this);
    dialog.setNameFilter(tr("E57 (*.e57)"));
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setFileMode(QFileDialog::ExistingFile);

    if (dialog.exec())
    {
        QString filename = dialog.selectedFiles().front();
        loadE57(filename.toStdString());
    }
}

void MainWindow::loadE57(const std::string& filename)
{
    m_reader = std::make_unique<E57Reader>(filename);
    ui->twMain->init(m_reader->root());
    ui->twViewProperties->init(nullptr);
    ui->tabWidget->clear();
}

void MainWindow::twMain_nodeSelected(TNode* node)
{
    if (dynamic_cast<TE57Node*>(node) != nullptr)
    {
        ui->twProperties->init(dynamic_cast<TE57Node*>(node));
    }
}

void MainWindow::tabWidget_tabClosesRequested(int index)
{
    if (index == -1)
        return;
    QWidget* tabItem = ui->tabWidget->widget(index);
    ui->tabWidget->removeTab(index);
    delete tabItem;
    tabItem = nullptr;
}

void MainWindow::tabWidget_currentChanged(int index)
{
    auto* widget = ui->tabWidget->currentWidget();
    if (dynamic_cast<SceneView*>(widget))
    {
        auto* renderer = dynamic_cast<SceneView*>(widget);
        ui->twScene->init(renderer->scene());
    }
    else
    {
        ui->twScene->clear();
    }
}

void MainWindow::twMain_itemDoubleClicked(QTreeWidgetItem* item, int column)
{
    if (dynamic_cast<TE57Node*>(item) == nullptr)
        return;
    auto* node = dynamic_cast<TE57Node*>(item);
    if (dynamic_cast<TNodeImage2D*>(node) != nullptr)
    {
        auto* image2D = dynamic_cast<TNodeImage2D*>(node);
        auto e57Image2D = std::dynamic_pointer_cast<E57Image2D>(node->node());
        openImage(*e57Image2D, e57Image2D->name());
    }
    else if (dynamic_cast<TNodeData3D*>(node) != nullptr)
    {
        auto* data3D = dynamic_cast<TNodeData3D*>(node);
        auto e57Data3D = std::dynamic_pointer_cast<E57Data3D>(node->node());
        if (e57Data3D->data().contains("points"))
        {
            openPointCloud(e57Data3D, "points", data3D->text(0).toStdString());
        }
    }
}

void MainWindow::twViewProperties_itemChanged(QTreeWidgetItem* item, int column)
{
    auto* widget = ui->tabWidget->currentWidget();
    if (dynamic_cast<SceneView*>(widget))
    {
        dynamic_cast<SceneView*>(widget)->update();
    }
}

void MainWindow::sceneView_itemDropped(SceneView* sender, QObject* source)
{
    auto* treeWidget = dynamic_cast<QTreeWidget*>(source);
    if (!treeWidget)
        return;
    for (auto* item : treeWidget->selectedItems())
    {
        if (auto* nodeImage2D = dynamic_cast<TNodeImage2D*>(item))
        {
            Image2d::Ptr image2d = std::make_shared<Image2d>();
            image2d->setName(nodeImage2D->node()->name());

            auto e57NodeImage2D =
                std::dynamic_pointer_cast<E57Image2D>(nodeImage2D->node());

            E57Utils utils(*m_reader);
            auto image = utils.getImage(*e57NodeImage2D);
            if (!image)
                return;
            image2d->setImage(*image);
            auto imageParameters = utils.getImageParameters(*e57NodeImage2D);
            if (!imageParameters)
                return;
            image2d->setImageWidth(imageParameters->width);
            image2d->setImageHeight(imageParameters->height);
            image2d->setPixelWidth(imageParameters->pixelWidth);
            image2d->setPixelHeight(imageParameters->pixelHeight);
            image2d->setFocalLength(imageParameters->focalLength);
            image2d->setPose(E57Utils::getPose(*e57NodeImage2D));
            if (imageParameters->isSpherical)
            {
                image2d->setFocalLength(1.0);
                image2d->setIsSpherical(true);
            }
            sender->scene().addNode(image2d);
        }
    }

    sender->update();
    ui->twScene->init(sender->scene());
}

void MainWindow::openImage(const E57Image2D& node, const std::string& tabName)
{
    auto image = E57Utils(*m_reader).getImage(node);
    if (!image)
        return;

    auto* imageViewer = new SiImageViewer(ui->tabWidget);
    int tabIndex =
        ui->tabWidget->addTab(imageViewer, QString::fromStdString(tabName));
    ui->tabWidget->setCurrentIndex(tabIndex);
    imageViewer->setImage(*image);
}

void MainWindow::openPointCloud(const E57NodePtr& node,
                                const std::string& dataName,
                                const std::string& tabName)
{
    if (!node->data().contains(dataName))
        return;

    auto e57Data3D = std::dynamic_pointer_cast<E57Data3D>(node);
    auto dataInfo = m_reader->dataInfo(node->data().at(dataName));

    auto* sceneView = findSceneView();
    if (!sceneView)
    {
        sceneView = new SceneView(ui->tabWidget);
        connect(sceneView, &SceneView::itemDropped, this,
                &MainWindow::sceneView_itemDropped);
        int tabIndex =
            ui->tabWidget->addTab(sceneView, QString::fromStdString(tabName));
        ui->tabWidget->setCurrentIndex(tabIndex);
        sceneView->scene().setPose(
            InverseMatrix(E57Utils::getPose(*e57Data3D)));
    }
    else
    {
        ui->tabWidget->setCurrentWidget(sceneView);
        ui->tabWidget->setTabText(ui->tabWidget->currentIndex(),
                                  QString("Combined View"));
    }

    sceneView->makeCurrent();

    std::vector<std::array<float, 3>> xyz(BUFFER_SIZE);
    auto dataReader = m_reader->dataReader(node->data().at(dataName));
    dataReader.bindBuffer("cartesianX", (float*)&xyz[0][0], xyz.size(),
                          3 * sizeof(float));
    dataReader.bindBuffer("cartesianY", (float*)&xyz[0][1], xyz.size(),
                          3 * sizeof(float));
    dataReader.bindBuffer("cartesianZ", (float*)&xyz[0][2], xyz.size(),
                          3 * sizeof(float));

    bool hasColor = false;
    bool hasIntensity = false;

    std::vector<std::array<float, 3>> rgb(0);
    if (std::any_of(dataInfo.begin(), dataInfo.end(),
                    [](const auto& info)
                    { return info.identifier == "colorRed"; }))
    {
        rgb.resize(BUFFER_SIZE);
        hasColor = true;
        dataReader.bindBuffer("colorRed", (float*)&rgb[0][0], rgb.size(),
                              3 * sizeof(float));
        dataReader.bindBuffer("colorGreen", (float*)&rgb[0][1], rgb.size(),
                              3 * sizeof(float));
        dataReader.bindBuffer("colorBlue", (float*)&rgb[0][2], rgb.size(),
                              3 * sizeof(float));
    }

    std::vector<float> intensity(0);
    if (std::any_of(dataInfo.begin(), dataInfo.end(),
                    [](const auto& info)
                    { return info.identifier == "intensity"; }))
    {
        intensity.resize(BUFFER_SIZE);
        hasIntensity = true;
        dataReader.bindBuffer("intensity", (float*)&intensity[0],
                              intensity.size());
    }

    auto pointCloud = std::make_shared<PointCloud>(nullptr, e57Data3D);

    uint64_t count;
    uint64_t totalCount = 0;
    while ((count = dataReader.read()) > 0)
    {
        std::vector<PointData> pointData;
        for (size_t i = 0; i < count; ++i)
        {
            PointData pd{};
            pd.xyz = xyz[i];
            if (hasColor)
            {
                pd.rgb = rgb[i];
                pd.rgb[0] /= 255.0;
                pd.rgb[1] /= 255.0;
                pd.rgb[2] /= 255.0;
            }
            else
            {
                pd.rgb[0] = 1.0;
                pd.rgb[1] = 1.0;
                pd.rgb[2] = 1.0;
            }

            if (hasIntensity)
            {
                pd.intensity = intensity[i];
            }
            else
            {
                pd.intensity = 1.0f;
            }

            pointData.push_back(pd);
        }
        pointCloud->insertPoints(pointData);
        totalCount += count;
    }

    pointCloud->setPose(E57Utils::getPose(*e57Data3D));
    pointCloud->doneInserting();
    sceneView->scene().addNode(pointCloud);

    ui->twScene->init(sceneView->scene());
}

SceneView* MainWindow::findSceneView()
{
    for (int i = 0; i < ui->tabWidget->count(); ++i)
    {
        auto* widget = ui->tabWidget->widget(i);
        if (dynamic_cast<SceneView*>(widget))
        {
            return dynamic_cast<SceneView*>(widget);
        }
    }

    return nullptr;
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls() && event->mimeData()->urls().size() == 1 &&
        event->mimeData()->urls().first().toLocalFile().endsWith(".e57"))
    {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent* event)
{
    const auto url = event->mimeData()->urls().first();
    auto filename = url.toLocalFile();
    loadE57(filename.toStdString());
}
