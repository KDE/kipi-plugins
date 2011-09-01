#ifndef SAVINGPROGRESSDIALOG_H
#define SAVINGPROGRESSDIALOG_H

#include <kdialog.h>

namespace KIPIPhotoLayoutsEditor
{
    class Canvas;

    class SavingProgressDialog : public KDialog
    {
            Q_OBJECT

            Canvas * canvas;
            KUrl url;
            QString * errorString;

        public:

            explicit SavingProgressDialog(Canvas * canvas, const KUrl & url, QString * errorString = 0);
            virtual int exec();

        protected slots:

            void savingErrorSlot(const QString & error);
    };
}

#endif // SAVINGPROGRESSDIALOG_H
