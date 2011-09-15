#ifndef CANVASSAVINGTHREAD_H
#define CANVASSAVINGTHREAD_H

#include <QThread>

#include <kurl.h>

#include "ProgressObserver.h"

namespace KIPIPhotoLayoutsEditor
{
    class Canvas;
    class CanvasSavingThread : public QThread, public ProgressObserver
    {
            Q_OBJECT

        public:

            explicit CanvasSavingThread(QObject * parent = 0);
            void save(Canvas * canvas, const KUrl & url);
            virtual void progresChanged(double progress);
            virtual void progresName(const QString & name);

        signals:

            void saved();

        protected:

            virtual void run();

        private slots:

            void bytesWritten(qint64);

        private:

            void sendProgressUpdate(double v);
            void sendActionUpdate(const QString & str);

            Canvas * m_canvas;
            KUrl m_url;
    };
}

#endif // CANVASSAVINGTHREAD_H
