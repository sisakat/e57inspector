#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "E57TreeNode.h"

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
}

void MainWindow::twMain_nodeSelected(TNode* node)
{
    ui->twProperties->init(node);
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
    auto* node = dynamic_cast<TNode*>(item);
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

    auto* view = new QGraphicsView(ui->tabWidget);
    auto* scene = new QGraphicsScene(view);
    scene->addPixmap(QPixmap::fromImage(img));
    scene->setSceneRect(img.rect());

    view->setScene(scene);
    int tabIndex = ui->tabWidget->addTab(view, QString::fromStdString(tabName));
    ui->tabWidget->setCurrentIndex(tabIndex);
}
