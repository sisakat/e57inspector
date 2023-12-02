#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "E57TreeNode.h"
#include "siimageviewer.h"
#include "sipointcloudrenderer.h"

#include <QBuffer>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QImageReader>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionOpen, &QAction::triggered, this,
            &MainWindow::actionOpen_triggered);
    connect(ui->twMain, &E57Tree::nodeSelected, this,
            &MainWindow::twMain_nodeSelected);
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this,
            &MainWindow::tabWidget_tabClosesRequested);
    connect(ui->twMain, &E57Tree::itemDoubleClicked, this,
            &MainWindow::twMain_itemDoubleClicked);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::actionOpen_triggered() { openFile(); }

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

void MainWindow::twMain_itemDoubleClicked(QTreeWidgetItem* item, int column)
{
    if (dynamic_cast<TE57Node*>(item) == nullptr) return;
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

    auto dataInfo = m_reader->dataInfo(node->data().at(dataName));

    auto* pointCloudViewer = new SiPointCloudRenderer(ui->tabWidget);
    int tabIndex = ui->tabWidget->addTab(pointCloudViewer,
                                         QString::fromStdString(tabName));
    ui->tabWidget->setCurrentIndex(tabIndex);

    std::vector<std::array<float, 3>> xyz(10000);
    auto dataReader = m_reader->dataReader(node->data().at(dataName));
    dataReader.bindBuffer("cartesianX", (float*)&xyz[0][0], xyz.size(),
                          3 * sizeof(float));
    dataReader.bindBuffer("cartesianY", (float*)&xyz[0][1], xyz.size(),
                          3 * sizeof(float));
    dataReader.bindBuffer("cartesianZ", (float*)&xyz[0][2], xyz.size(),
                          3 * sizeof(float));

    std::vector<std::array<float, 3>> rgb(10000);
    bool hasColor = false;
    if (std::any_of(dataInfo.begin(), dataInfo.end(),
                    [](const auto& info)
                    { return info.identifier == "colorRed"; }))
    {
        hasColor = true;
        dataReader.bindBuffer("colorRed", (float*)&rgb[0][0], rgb.size(),
                              3 * sizeof(float));
        dataReader.bindBuffer("colorGreen", (float*)&rgb[0][1], rgb.size(),
                              3 * sizeof(float));
        dataReader.bindBuffer("colorBlue", (float*)&rgb[0][2], rgb.size(),
                              3 * sizeof(float));
    }

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
            pointData.push_back(pd);
        }
        pointCloudViewer->insert(pointData);
        totalCount += count;
    }

    pointCloudViewer->doneInserting();
}
