/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-06-08
 * Description : a kipi plugin to automatically detect
 *               and remove red eyes from images
 *
 * Copyright 2008 by Andi Clemens <andi dot clemens at gmx dot net>
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

#include "workerthread.h"
#include "workerthread.moc"

// Qt includes.

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QFileInfo>

// Local includes.

#include "eyelocator.h"
#include "removalsettings.h"
#include "storagesettingsbox.h"
#include "wteventdata.h"

namespace KIPIRemoveRedEyesPlugin
{
WorkerThread::WorkerThread(QObject* parent, RemovalSettings* settings, int type, KUrl::List urls)
{
    m_parent    = parent;
    m_settings  = settings;
    m_cancel    = false;
    m_type      = type;
    m_urls      = urls;
}

WorkerThread::~ WorkerThread()
{
    // wait for the thread to finish
    wait();

    delete m_settings;
}

void WorkerThread::run()
{
    int total = m_urls.count();

    if (total <= 0)
        return;

    int i = 1;

    for (KUrl::List::iterator it = m_urls.begin(); it != m_urls.end(); ++it, ++i)
    {
        KUrl& url = (KUrl&)(*it);
        if (!url.isLocalFile())
            break;

        // we need to convert the QString to const char* for openCV to work
        QByteArray src = QFile::encodeName(url.path());
        QByteArray cls = QFile::encodeName(m_settings->classifierFile);

        // find and remove red eyes
        EyeLocator loc(src.data(),
                       cls.data(),
                       m_settings->scaleFactor,
                       m_settings->neighborGroups,
                       m_settings->minRoundness,
                       m_settings->minBlobsize);

        // save image to specified location
        if ((m_type == Correction) && (loc.redEyes() > 0))
        {
            QFileInfo info(url.path());
            KUrl saveLocation = KUrl(info.path());

            switch (m_settings->storageMode)
            {
                case StorageSettingsBox::Subfolder:
                {
                    saveLocation.addPath(m_settings->subfolderName);

                    // check if subfolder exists
                    if (!QDir(saveLocation.path()).exists())
                        QDir(info.path()).mkdir(m_settings->subfolderName);

                    saveLocation.addPath(info.fileName());
                    break;
                }
                case StorageSettingsBox::Prefix:
                {
                    QString file = info.baseName();
                    file.append(m_settings->prefixName);
                    file.append(".");
                    file.append(info.suffix());
                    saveLocation.addPath(file);
                    break;
                }
                case StorageSettingsBox::Overwrite:
                {
                    saveLocation.addPath(info.fileName());
                    break;
                }
            }

            QByteArray dest = QFile::encodeName(saveLocation.path());
            loc.saveImage(dest.data());
        }

        int eyes = loc.redEyes();
        emit calculationFinished(new WTEventData(url, i, eyes));
    }
}
} // namespace KIPIRemoveRedEyesPlugin
