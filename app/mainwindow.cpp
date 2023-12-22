#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "E57TreeNode.h"
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
        // load image
        auto e57Image2D = std::dynamic_pointer_cast<E57Image2D>(node->node());

        E57NodePtr imageRepresentation = nullptr;
        if (e57Image2D->pinholeRepresentation())
        {
            imageRepresentation = e57Image2D->pinholeRepresentation();
            openPinholeRepresentation3d(e57Image2D,
                                        e57Image2D->pinholeRepresentation());
        }
        else if (e57Image2D->sphericalRepresentation())
        {
            imageRepresentation = e57Image2D->sphericalRepresentation();
        }
        else if (e57Image2D->cylindricalRepresentation())
        {
            imageRepresentation = e57Image2D->cylindricalRepresentation();
        }

        if (imageRepresentation)
        {
            if (imageRepresentation->blobs().contains("jpegImage"))
            {
                openImageBlob(imageRepresentation, "jpegImage",
                              image2D->text(0).toStdString());
            }
            else if (imageRepresentation->blobs().contains("pngImage"))
            {
                openImageBlob(imageRepresentation, "pngImage",
                              image2D->text(0).toStdString());
            }
        }
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

void MainWindow::openImageBlob(const E57NodePtr& node,
                               const std::string& blobName,
                               const std::string& tabName)
{
    if (!node->blobs().contains(blobName))
        return;
    auto imageData = m_reader->blobData(node->blobs().at(blobName));
    QByteArray data = QByteArray::fromRawData(
        reinterpret_cast<const char*>(imageData.data()), imageData.size());

    // Disable image allocation limit (else 128MB)
    QImageReader::setAllocationLimit(0);

    QImage img;
    img.loadFromData(reinterpret_cast<const uchar*>(imageData.data()),
                     (int)imageData.size(), "jpeg");
    auto* imageViewer = new SiImageViewer(ui->tabWidget);
    int tabIndex =
        ui->tabWidget->addTab(imageViewer, QString::fromStdString(tabName));
    ui->tabWidget->setCurrentIndex(tabIndex);
    imageViewer->setImage(img);
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
        int tabIndex =
            ui->tabWidget->addTab(sceneView, QString::fromStdString(tabName));
        ui->tabWidget->setCurrentIndex(tabIndex);
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

    const auto& pose = e57Data3D->pose();
    glm::quat quaternion(static_cast<float>(pose.rotation.w),
                         static_cast<float>(pose.rotation.x),
                         static_cast<float>(pose.rotation.y),
                         static_cast<float>(pose.rotation.z));
    Matrix4d sop(quaternion);
    sop[3] = Vector4d(static_cast<float>(pose.translation[0]),
                      static_cast<float>(pose.translation[1]),
                      static_cast<float>(pose.translation[2]), 1.0f);
    pointCloud->setSOP(sop);

    for (auto& node : sceneView->scene().nodes())
    {
        if (std::dynamic_pointer_cast<PointCloud>(node))
        {
            auto firstPointCloud = std::dynamic_pointer_cast<PointCloud>(node);
            pointCloud->setPose(InverseMatrix(firstPointCloud->sop()) *
                                pointCloud->sop());
            break;
        }
    }

    pointCloud->doneInserting();
    sceneView->scene().addNode(pointCloud);

    ui->twScene->init(sceneView->scene());
}

void MainWindow::openPinholeRepresentation3d(
    const E57Image2DPtr& image2D,
    const E57PinholeRepresentationPtr& pinholeRepresentation)
{
    const auto tabName = image2D->name();

    auto* sceneView = findSceneView();
    if (!sceneView)
    {
        sceneView = new SceneView(ui->tabWidget);
        int tabIndex =
            ui->tabWidget->addTab(sceneView, QString::fromStdString(tabName));
        ui->tabWidget->setCurrentIndex(tabIndex);
    }
    else
    {
        ui->tabWidget->setCurrentWidget(sceneView);
        ui->tabWidget->setTabText(ui->tabWidget->currentIndex(),
                                  QString("Combined View"));
    }

    sceneView->makeCurrent();
    Image2d::Ptr image2d = std::make_shared<Image2d>();

    const auto& pose = image2D->pose();
    glm::quat quaternion(static_cast<float>(pose.rotation.w),
                         static_cast<float>(pose.rotation.x),
                         static_cast<float>(pose.rotation.y),
                         static_cast<float>(pose.rotation.z));
    Matrix4d sop(quaternion);
    sop[3] = Vector4d(static_cast<float>(pose.translation[0]),
                      static_cast<float>(pose.translation[1]),
                      static_cast<float>(pose.translation[2]), 1.0f);

    for (auto& node : sceneView->scene().nodes())
    {
        if (std::dynamic_pointer_cast<PointCloud>(node))
        {
            auto firstPointCloud = std::dynamic_pointer_cast<PointCloud>(node);
            image2d->setPose(InverseMatrix(firstPointCloud->sop()) * sop);
            break;
        }
    }

    image2d->setImage2D(image2D);
    image2d->setPinholeRepresentation(pinholeRepresentation);

    if (pinholeRepresentation->blobs().contains("jpegImage"))
    {
        auto imageData = m_reader->blobData(pinholeRepresentation->blobs().at("jpegImage"));
        QByteArray data = QByteArray::fromRawData(
            reinterpret_cast<const char*>(imageData.data()), imageData.size());

        // Disable image allocation limit (else 128MB)
        QImageReader::setAllocationLimit(0);

        QImage img;
        img.loadFromData(reinterpret_cast<const uchar*>(imageData.data()),
                         (int)imageData.size(), "jpeg");
        image2d->setImage(img);
    }

    sceneView->scene().addNode(image2d);
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
