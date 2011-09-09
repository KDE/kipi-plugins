#include "CanvasLoadingThread.h"

using namespace KIPIPhotoLayoutsEditor;

class CanvasLoadingThread::CanvasLoadingThreadPrivate
{
};

CanvasLoadingThread::CanvasLoadingThread(QObject *parent) :
    QThread(parent),
    d(new CanvasLoadingThreadPrivate)
{
}

void CanvasLoadingThread::run()
{
}
