/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
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

#include "SavingThread.moc"
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
