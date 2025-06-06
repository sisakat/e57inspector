#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "E57TreeNode.h"
#include "E57Utils.h"
#include "Image2d.h"
#include "PanoramaImageThread.h"
#include "SceneView.h"
#include "about.h"
#include "siimageviewer.h"
#include "version.h"
#include "welcome.h"

#include <QBuffer>
#include <QFileDialog>
#include <QFontDatabase>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QImageReader>
#include <QLabel>
#include <QMessageBox>
#include <QMimeData>
#include <QTextEdit>
#include <QThread>

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
    connect(ui->actionHelp, &QAction::triggered, this,
            &MainWindow::actionHelp_triggered);
    connect(ui->actionCamera_Top, &QAction::triggered, this,
            &MainWindow::actionCamera_Top_triggered);
    connect(ui->actionCamera_Bottom, &QAction::triggered, this,
            &MainWindow::actionCamera_Bottom_triggered);
    connect(ui->actionCamera_Left, &QAction::triggered, this,
            &MainWindow::actionCamera_Left_triggered);
    connect(ui->actionCamera_Right, &QAction::triggered, this,
            &MainWindow::actionCamera_Right_triggered);
    connect(ui->actionCamera_Front, &QAction::triggered, this,
            &MainWindow::actionCamera_Front_triggered);
    connect(ui->actionCamera_Back, &QAction::triggered, this,
            &MainWindow::actionCamera_Back_triggered);
    connect(ui->actionShow_XML_dump, &QAction::triggered, this,
            &MainWindow::actionShow_XML_dump_triggered);
    connect(ui->twMain, &E57Tree::nodeSelected, this,
            &MainWindow::twMain_nodeSelected);
    connect(ui->twMain, &E57Tree::onAction, this, &MainWindow::twMain_onAction);
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this,
            &MainWindow::tabWidget_tabClosesRequested);
    connect(ui->twMain, &E57Tree::itemDoubleClicked, this,
            &MainWindow::twMain_itemDoubleClicked);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this,
            &MainWindow::tabWidget_currentChanged);
    connect(ui->twViewProperties, &ScenePropertyEditor::itemChanged, this,
            &MainWindow::twViewProperties_itemChanged);

    ui->actionOpen->setIcon(QIcon(":/icons/Open.png"));
    ui->actionCamera_Top->setIcon(QIcon(":/icons/SideViewTop.png"));
    ui->actionCamera_Bottom->setIcon(QIcon(":/icons/SideViewBottom.png"));
    ui->actionCamera_Left->setIcon(QIcon(":/icons/SideViewLeft.png"));
    ui->actionCamera_Right->setIcon(QIcon(":/icons/SideViewRight.png"));
    ui->actionCamera_Front->setIcon(QIcon(":/icons/SideViewFront.png"));
    ui->actionCamera_Back->setIcon(QIcon(":/icons/SideViewBack.png"));
    ui->actionShow_XML_dump->setIcon(QIcon(":/icons/XmlDump.png"));
    ui->actionAbout->setIcon(QIcon(":/icons/About.png"));
    ui->actionClose->setIcon(QIcon(":/icons/Close.png"));

    ui->twScene->setScenePropertyEditor(ui->twViewProperties);
    ui->tabWidget->addTab(new Welcome(ui->tabWidget), tr("Welcome"));

    ui->tabWidgetMain->setHidden(true);
    ui->tabWidgetScene->setHidden(true);

    std::string statusBarText = std::string(INFO_PRODUCTNAME_STRING) + " " +
                                std::string(INFO_PRODUCTVERSION_STRING);
    ui->statusbar->showMessage(QString::fromStdString(statusBarText));

    // workaround for closing and reopening window
    auto* sceneView = new SceneView();
    int index = ui->tabWidget->addTab(sceneView, "");
    ui->tabWidget->removeTab(index);
    sceneView->deleteLater();
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

void MainWindow::actionHelp_triggered()
{
    QFile helpFile(":/text/help/help.html");
    if (helpFile.open(QIODevice::ReadOnly))
    {
        auto* helpDocument = new QTextDocument(this);
        helpDocument->setHtml(helpFile.readAll());
        helpDocument->setDocumentMargin(10);

        auto* textEdit = new QTextEdit(ui->tabWidget);
        textEdit->setDocument(helpDocument);
        textEdit->setReadOnly(true);

        ui->tabWidget->addTab(textEdit, tr("Help"));
        ui->tabWidget->setCurrentWidget(textEdit);
    }
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
            camera->setPickpointNavigation(true);
        }
        sceneView->update();
    }
}

void MainWindow::actionCamera_Bottom_triggered()
{
    auto sceneView = findSceneView();
    if (sceneView)
    {
        auto camera = sceneView->scene().findNode<Camera>();
        if (camera)
        {
            camera->bottomView();
            camera->setPickpointNavigation(true);
        }
        sceneView->update();
    }
}

void MainWindow::actionCamera_Left_triggered()
{
    auto sceneView = findSceneView();
    if (sceneView)
    {
        auto camera = sceneView->scene().findNode<Camera>();
        if (camera)
        {
            camera->leftView();
            camera->setPickpointNavigation(true);
        }
        sceneView->update();
    }
}

void MainWindow::actionCamera_Right_triggered()
{
    auto sceneView = findSceneView();
    if (sceneView)
    {
        auto camera = sceneView->scene().findNode<Camera>();
        if (camera)
        {
            camera->rightView();
            camera->setPickpointNavigation(true);
        }
        sceneView->update();
    }
}

void MainWindow::actionCamera_Front_triggered()
{
    auto sceneView = findSceneView();
    if (sceneView)
    {
        auto camera = sceneView->scene().findNode<Camera>();
        if (camera)
        {
            camera->frontView();
            camera->setPickpointNavigation(true);
        }
        sceneView->update();
    }
}

void MainWindow::actionCamera_Back_triggered()
{
    auto sceneView = findSceneView();
    if (sceneView)
    {
        auto camera = sceneView->scene().findNode<Camera>();
        if (camera)
        {
            camera->backView();
            camera->setPickpointNavigation(true);
        }
        sceneView->update();
    }
}

void MainWindow::actionShow_XML_dump_triggered()
{
    showXMLDump();
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
    m_filename = filename;
    m_reader = std::make_unique<E57Reader>(filename);
    ui->twMain->init(m_reader->root());
    ui->twViewProperties->init(nullptr);
    ui->tabWidget->clear();

    QScrollArea* scrollArea = new QScrollArea(ui->tabWidget);
    scrollArea->setVerticalScrollBarPolicy(
        Qt::ScrollBarPolicy::ScrollBarAsNeeded);
    scrollArea->setHorizontalScrollBarPolicy(
        Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    auto* layout = new QVBoxLayout();
    scrollArea->setLayout(layout);
    QLabel* test = new QLabel(scrollArea);
    layout->addWidget(test);
    test->setWordWrap(true);
    test->setTextFormat(Qt::TextFormat::MarkdownText);

    std::stringstream ss;
    ss << "# " << std::filesystem::path(filename).stem().string() << "\n";
    ss << "**Scans:** " << m_reader->root()->data3D().size() << "\n\n";
    ss << "**Images:** " << m_reader->root()->images2D().size() << "\n";
    test->setText(QString::fromStdString(ss.str()));

    test->setAlignment(Qt::Alignment(Qt::AlignmentFlag::AlignTop));
    test->setMargin(10);
    ui->tabWidget->addTab(scrollArea, tr("Summary"));

    ui->tabWidgetMain->setHidden(false);
    ui->tabWidgetScene->setHidden(false);
    ui->tabWidgetScene->clear();
    ui->tabWidgetScene->addTab(ui->tabPageProperties, tr("Properties"));
    ui->tabWidgetScene->addTab(ui->tabPageScene, tr("View"));
}

void MainWindow::twMain_nodeSelected(TNode* node)
{
    if (dynamic_cast<TE57Node*>(node) != nullptr)
    {
        ui->twProperties->init(dynamic_cast<TE57Node*>(node));
    }
}

void MainWindow::onPanoramaImageThreadFinished(const std::string& title,
                                               QImage image)
{
    if (title.empty())
    {
        QMessageBox::critical(
            this, "Error",
            "Could not create scan data panorama. This may be due to "
            "missing fields in the Data3D item.");
        return;
    }

    auto* imageViewer = new SiImageViewer(ui->tabWidget);
    int tabIndex =
        ui->tabWidget->addTab(imageViewer, QString::fromStdString(title));
    ui->tabWidget->setCurrentIndex(tabIndex);
    imageViewer->setImage(image);
}

void MainWindow::twMain_onAction(const TNode* node, NodeAction action)
{
    if (dynamic_cast<const TNodeE57*>(node) != nullptr)
    {
        const auto* e57node = dynamic_cast<const TNodeE57*>(node);
        if (action == NodeAction::opXmlDump)
        {
            showXMLDump();
        }
    }

    if (dynamic_cast<const TNodeData3D*>(node) != nullptr)
    {
        const auto* data3DNode = dynamic_cast<const TNodeData3D*>(node);

        if (action == NodeAction::opScanPanorama)
        {
            QThread* thread = new QThread();
            PanoramaImageThread* worker = new PanoramaImageThread(
                m_filename, data3DNode->node()->getString("guid"),
                data3DNode->node()->name() + std::string(" Scan Panorama"));
            worker->moveToThread(thread);

            connect(worker, SIGNAL(error(QString)), this,
                    SLOT(errorString(QString)));
            connect(thread, &QThread::started, worker,
                    &PanoramaImageThread::process);
            connect(worker, &PanoramaImageThread::panoramaImageResult, this,
                    &MainWindow::onPanoramaImageThreadFinished);
            connect(worker, &PanoramaImageThread::finished, thread,
                    &QThread::quit);
            connect(worker, &PanoramaImageThread::finished, worker,
                    &PanoramaImageThread::deleteLater);
            connect(thread, &QThread::finished, thread, &QThread::deleteLater);
            thread->start();
        }
        else if (action == NodeAction::opView3d)
        {
            auto e57Data3D =
                std::dynamic_pointer_cast<E57Data3D>(data3DNode->node());
            if (e57Data3D->data().contains("points"))
            {
                sceneView_itemDropped(nullptr, ui->twMain);
            }
        }
    }

    if (dynamic_cast<const TNodeImage2D*>(node) != nullptr)
    {
        const auto* dataImage2D = dynamic_cast<const TNodeImage2D*>(node);
        if (action == NodeAction::opView2d)
        {
            auto e57Image2D =
                std::dynamic_pointer_cast<E57Image2D>(dataImage2D->node());
            openImage(*e57Image2D, e57Image2D->name());
        }
        else if (action == NodeAction::opView3d)
        {
            sceneView_itemDropped(nullptr, ui->twMain);
        }
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

    ui->twViewProperties->clear();
}

void MainWindow::tabWidget_currentChanged(int index)
{
    auto* widget = ui->tabWidget->currentWidget();
    if (dynamic_cast<SceneView*>(widget))
    {
        auto* renderer = dynamic_cast<SceneView*>(widget);
        ui->twScene->init(renderer->scene());
        ui->twViewProperties->clear();
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
            sceneView_itemDropped(nullptr, ui->twMain);
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
    bool topView = true;
    bool sceneViewCreated = false;
    if (!sender)
    {
        sender = createSceneView();
        sceneViewCreated = true;
    }

    E57Utils utils(*m_reader);
    auto* treeWidget = dynamic_cast<QTreeWidget*>(source);
    if (!treeWidget)
        return;

    sender->makeCurrent();
    for (auto* item : treeWidget->selectedItems())
    {
        if (auto* nodeImage2D = dynamic_cast<TNodeImage2D*>(item))
        {
            Image2d::Ptr image2d = std::make_shared<Image2d>();
            image2d->setName(nodeImage2D->node()->name());

            auto associatedData3D =
                dynamic_cast<TNode*>(item)->findParent<TNodeData3D>();

            auto e57NodeImage2D =
                std::dynamic_pointer_cast<E57Image2D>(nodeImage2D->node());

            if (sceneViewCreated)
            {
                ui->tabWidget->setTabText(
                    ui->tabWidget->currentIndex(),
                    QString::fromStdString(e57NodeImage2D->name()));
            }

            if (sender->scene().nodes().size() < 2)
            {
                if (associatedData3D)
                {
                    auto e57Data3D = std::dynamic_pointer_cast<E57Data3D>(
                        associatedData3D->node());
                    sender->scene().setPose(
                        InverseMatrix(E57Utils::getPose(*e57Data3D)));
                }
                else
                {
                    sender->scene().setPose(
                        InverseMatrix(E57Utils::getPose(*e57NodeImage2D)));
                }
            }

            auto image = utils.getImage(*e57NodeImage2D);
            if (!image)
                return;
            image2d->setImage(*image);
            auto imageMask = utils.getImageMask(*e57NodeImage2D);
            if (imageMask)
            {
                image2d->setImageMask(*imageMask);
            }
            auto imageParameters = utils.getImageParameters(*e57NodeImage2D);
            if (!imageParameters)
                return;
            image2d->setImageWidth(imageParameters->width);
            image2d->setImageHeight(imageParameters->height);
            image2d->setPixelWidth(imageParameters->pixelWidth);
            image2d->setPixelHeight(imageParameters->pixelHeight);
            image2d->setFocalLength(imageParameters->focalLength);
            image2d->setBackfaceCulling(image2d->isFullPanorama());
            image2d->setPose(E57Utils::getPose(*e57NodeImage2D));
            if (imageParameters->isSpherical)
            {
                image2d->setFocalLength(1.0);
                image2d->setIsSpherical(true);
            }
            sender->scene().addNode(image2d);

            auto camera = sender->scene().findNode<Camera>();
            if (camera)
            {
                if (sender->scene().nodes().size() < 3)
                {
                    if (image2d->isSpherical())
                    {
                        image2d->cameraToImageView();
                        topView = false;
                    }
                }
                else
                {
                    camera->setPickpointNavigation(true);
                }
            }
        }
        else if (auto* nodeData3D = dynamic_cast<TNodeData3D*>(item))
        {
            auto e57NodeData3D =
                std::dynamic_pointer_cast<E57Data3D>(nodeData3D->node());
            auto dataInfo =
                m_reader->dataInfo(e57NodeData3D->data().at("points"));

            bool isFirstObject = sender->scene().nodes().size() < 2;
            if (isFirstObject)
            {
                sender->scene().setPose(
                    InverseMatrix(E57Utils::getPose(*e57NodeData3D)));
            }

            if (sceneViewCreated)
            {
                ui->tabWidget->setTabText(
                    ui->tabWidget->currentIndex(),
                    QString::fromStdString(e57NodeData3D->name()));
            }

            auto pointCloud =
                std::make_shared<PointCloud>(nullptr, e57NodeData3D);
            auto data = E57Utils(*m_reader).getData3D(*e57NodeData3D);

            if (data)
            {
                if (!data->rgba.empty())
                {
                    pointCloud->setViewType(PointCloudViewType::COLOR);
                }
                else if (!data->intensity.empty())
                {
                    pointCloud->setViewType(PointCloudViewType::INTENSITY);
                }
                else
                {
                    pointCloud->setViewType(PointCloudViewType::SINGLECOLOR);
                    pointCloud->setSingleColor(Qt::white);
                }

                pointCloud->setPose(E57Utils::getPose(*e57NodeData3D));
                pointCloud->setPointCloudData(*data);
                sender->scene().addNode(pointCloud);
            }
        }
    }

    sender->update();
    ui->twScene->init(sender->scene());
    ui->twViewProperties->clear();

    if (sceneViewCreated)
    {
        sender->scene().render();
        auto camera = sender->scene().findNode<Camera>();
        if (camera && topView)
        {
            camera->topView();
            sender->update();
        }
    }
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

void MainWindow::createEditor(const std::string& title,
                              const std::string& content)
{
    auto* imageViewer = new SiImageViewer(ui->tabWidget);
    auto* editor = new QTextEdit(ui->tabWidget);
    int tabIndex = ui->tabWidget->addTab(editor, QString::fromStdString(title));
    ui->tabWidget->setCurrentIndex(tabIndex);
    editor->setText(QString::fromStdString(content));
    editor->setHorizontalScrollBarPolicy(
        Qt::ScrollBarPolicy::ScrollBarAsNeeded);
    editor->setWordWrapMode(QTextOption::WrapMode::NoWrap);

    const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    editor->setFont(fixedFont);
}

void MainWindow::showXMLDump()
{
    if (m_reader)
    {
        createEditor("XML Dump", m_reader->dumpXML(2));
    }
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

    if (event->mimeData()->hasFormat(
            "application/x-qabstractitemmodeldatalist"))
    {
        event->accept();
    }
}

void MainWindow::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasUrls() && event->mimeData()->urls().size() == 1 &&
        event->mimeData()->urls().first().toLocalFile().endsWith(".e57"))
    {
        const auto url = event->mimeData()->urls().first();
        auto filename = url.toLocalFile();
        loadE57(filename.toStdString());
    }

    if (event->mimeData()->hasFormat(
            "application/x-qabstractitemmodeldatalist"))
    {
        if (event->source())
        {
            sceneView_itemDropped(nullptr, event->source());
        }
    }
}

void MainWindow::closeEvent(QCloseEvent* event) {}

SceneView* MainWindow::createSceneView(const std::string& name)
{
    auto sceneView = new SceneView(ui->tabWidget);
    connect(sceneView, &SceneView::itemDropped, this,
            &MainWindow::sceneView_itemDropped);
    int tabIndex =
        ui->tabWidget->addTab(sceneView, QString::fromStdString(name));
    ui->tabWidget->setCurrentIndex(tabIndex);
    return sceneView;
}
