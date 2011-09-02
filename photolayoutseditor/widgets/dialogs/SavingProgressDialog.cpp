/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2011-09-01
 * Description : a plugin to create photo layouts by fusion of several images.
 * Acknowledge : based on the expoblending plugin
 *
 * Copyright (C) 2011 by Łukasz Spas <lukasz dot spas at gmail dot com>
 * Copyright (C) 2009-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

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
