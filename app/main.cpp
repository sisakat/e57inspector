#include "mainwindow.h"

#include <string>

#include <QApplication>
#include <QTimer>

int main(int argc, char* argv[])
{
    QSurfaceFormat format;
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    // Load file given on CLI.
    if (argc > 1)
    {
        std::string filename = argv[1];
        QTimer::singleShot(0, &w, [&w, filename]() { w.loadE57(filename); });
    }

    return a.exec();
}
