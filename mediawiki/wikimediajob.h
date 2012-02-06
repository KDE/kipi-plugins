/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-02-11
 * Description : a kipi plugin to export images to WikiMedia web service
 *
 * Copyright (C) 2011 by Alexandre Mendes <alex dot mendes1988 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef WIKIMEDIAJOB_H
#define WIKIMEDIAJOB_H

// Qt includes

#include <QString>
#include <QList>
#include <QMap>

// KDE includes

#include <KUrl>
#include <KJob>

// Local includes

#include "imageslist.h"

namespace mediawiki
{
    class MediaWiki;
}

namespace KIPI
{
    class Interface;
}

namespace KIPIWikiMediaPlugin
{

class WikiMediaJob : public KJob
{
    Q_OBJECT

public:

    WikiMediaJob(KIPI::Interface* interface ,mediawiki::MediaWiki* mediawiki, QObject* parent=0);
    QString buildWikiText(const QMap<QString, QString>& info);

    void setImageMap(const QList<QMap<QString, QString> >& imageDesc);
    void start();

Q_SIGNALS:

    void uploadProgress(int percent);
    void endUpload();

public Q_SLOTS:

    void begin();
    void uploadHandle(KJob* j = 0);
    void slotUploadProgress(KJob* job, unsigned long percent);

private:

    KUrl::List                    m_urls;
    KIPI::Interface*              m_interface;
    mediawiki::MediaWiki*         m_mediawiki;
    QList<QMap<QString,QString> > m_imageDesc;
    QString                       m_error;
    QString                       m_currentFile;
};

} // namespace KIPIWikiMediaPlugin

#endif // WIKIMEDIAJOB_H
