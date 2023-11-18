#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "E57TreeNode.h"

#include <QFileDialog>

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

void MainWindow::twMain_nodeSelected(TNode* node) { qDebug() << node; }
