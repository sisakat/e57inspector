#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "E57TreeNode.h"

#include <QBuffer>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QGraphicsView>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionOpen, &QAction::triggered, this,
            &MainWindow::actionOpen_triggered);
    connect(ui->twMain, &E57Tree::nodeSelected, this,
            &MainWindow::twMain_nodeSelected);
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

    if (dynamic_cast<TNodePinholeRepresentation*>(node) != nullptr)
    {
        // load image
        auto pinholeRepr =
            std::dynamic_pointer_cast<E57PinholeRepresentation>(node->node());
        if (pinholeRepr->blobs().contains("jpegImage"))
        {
            auto jpegImageData =
                m_reader->blobData(pinholeRepr->blobs().at("jpegImage"));

            QByteArray data = QByteArray::fromRawData(
                reinterpret_cast<const char*>(jpegImageData.data()),
                jpegImageData.size());
            //            QBuffer buffer(data);
            //            QImageReader reader(&buffer);
            //            QImage img = reader.read();
            QImage img;
            img.loadFromData(
                reinterpret_cast<const uchar*>(jpegImageData.data()),
                (int)jpegImageData.size(), "jpeg");

            auto* scene = new QGraphicsScene(ui->tabWidget);
            scene->addPixmap(QPixmap::fromImage(img));
            scene->setSceneRect(img.rect());
            auto* view = new QGraphicsView(ui->tabWidget);
            view->setScene(scene);
            ui->tabWidget->addTab(view, QString("Test"));
        }
    }
}
