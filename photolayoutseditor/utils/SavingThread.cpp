#include "SavingThread.h"
#include "Canvas.h"

#include <QFile>

using namespace KIPIPhotoLayoutsEditor;

SavingThread::SavingThread(Canvas * canvas, const KUrl & url, QObject * parent) :
    QThread(parent),
    canvas(canvas),
    url(url)
{}

void SavingThread::run()
{
    QFile file(url.path());
    if (file.open(QFile::WriteOnly | QFile::Text))
    {
        file.write(canvas->toSvg().toString().toAscii());
        file.close();
    }
    if (!file.errorString().isEmpty())
        emit savingError(file.errorString());
}
