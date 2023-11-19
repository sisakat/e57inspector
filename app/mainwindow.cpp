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
    //    connect(ui->tabWidget->tabBar(), &QTabBar::tabCloseRequested,
    //            ui->tabWidget->tabBar(), &QTabBar::removeTab);
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
        auto pinholeRepr = e57Image2D->pinholeRepresentation();
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

            auto* view = new QGraphicsView(ui->tabWidget);
            auto* scene = new QGraphicsScene(view);
            scene->addPixmap(QPixmap::fromImage(img));
            scene->setSceneRect(img.rect());

            view->setScene(scene);
            int tabIndex =
                ui->tabWidget->addTab(view, QString(image2D->text(0)));
            ui->tabWidget->setCurrentIndex(tabIndex);
        }
    }
}
