#ifndef SAVINGTHREAD_H
#define SAVINGTHREAD_H

#include <QThread>

#include <kurl.h>

namespace KIPIPhotoLayoutsEditor
{
    class Canvas;
    class SavingThread : public QThread
    {
        Q_OBJECT

        Canvas * canvas;
        KUrl url;

    public:

        SavingThread(Canvas * canvas, const KUrl & url, QObject * parent = 0);

    protected:

        virtual void run();

    signals:

        void savingError(QString);
    };
}

#endif // SAVINGTHREAD_H
