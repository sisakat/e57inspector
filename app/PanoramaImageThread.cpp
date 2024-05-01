#include "PanoramaImageThread.h"

#include "panorama.h"
#include <QImage>

void PanoramaImageThread::process()
{
    try
    {
        Panorama panorama(filename);
        auto panoramaImage = panorama.createPanorama(guid);
        QImage result = QImage(panoramaImage.data.data(), panoramaImage.width,
                               panoramaImage.height, QImage::Format_RGBA8888)
                            .copy();
        emit panoramaImageResult(title, result);
    }
    catch (...)
    {
        emit panoramaImageResult("", QImage());
    }

    emit finished();
}
