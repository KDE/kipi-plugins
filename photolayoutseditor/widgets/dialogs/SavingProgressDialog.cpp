#include "SavingProgressDialog.h"
#include "SavingThread.h"
#include "Canvas.h"

#include <QThread>

using namespace KIPIPhotoLayoutsEditor;

SavingProgressDialog::SavingProgressDialog(Canvas * canvas, const KUrl & url, QString * errorString) :
    KDialog(canvas),
    canvas(canvas),
    url(url),
    errorString(errorString)
{
    this->setModal(true);
    this->setButtons( Cancel );
}

int SavingProgressDialog::exec()
{
    SavingThread thread(canvas, url, this);
    if (errorString)
        connect(&thread, SIGNAL(savingError(QString)), this, SLOT(savingErrorSlot(QString)));
    connect(&thread, SIGNAL(finished()), this, SLOT(accept()));
    thread.start();
    int result = KDialog::exec();
    if (result == KDialog::Cancel ||
            result == KDialog::Close)
    {
        if (thread.isRunning())
            thread.terminate();
        thread.wait();
    }
    return result;
}

void SavingProgressDialog::savingErrorSlot(const QString & error)
{
    if (errorString)
        *errorString = error;
}
