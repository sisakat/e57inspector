#ifndef E57INSPECTOR_PANORAMAIMAGETHREAD_H
#define E57INSPECTOR_PANORAMAIMAGETHREAD_H

#include <QObject>
#include <string>

class PanoramaImageThread : public QObject
{
    Q_OBJECT
public:
    std::string filename;
    std::string guid;
    std::string title;

    PanoramaImageThread(std::string filename_, std::string guid_,
                        std::string title_)
    {
        filename = std::move(filename_);
        guid = std::move(guid_);
        title = std::move(title_);
    }

public slots:
    void process();

signals:
    void panoramaImageResult(const std::string& title, const QImage& image);
    void finished();
};

#endif // E57INSPECTOR_PANORAMAIMAGETHREAD_H
