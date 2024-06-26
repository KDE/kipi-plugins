/* ============================================================
 *
 * This file is a part of KDE project
 *
 *
 * Date        : 2011-02-11
 * Description : a kipi plugin to export images to WikiMedia web service
 *
 * Copyright (C) 2011      by Alexandre Mendes <alex dot mendes1988 at gmail dot com>
 * Copyright (C) 2011-2018 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef WM_TALKER_H
#define WM_TALKER_H

// Qt includes

#include <QString>
#include <QList>
#include <QMap>
#include <QUrl>

// KDE includes

#include <kjob.h>

namespace mediawiki
{
    class MediaWiki;
}

namespace KIPI
{
    class Interface;
}

using namespace KIPI;
using namespace mediawiki;

namespace KIPIWikiMediaPlugin
{

class WMTalker : public KJob
{
    Q_OBJECT

public:

    WMTalker(Interface* const interface, MediaWiki* const mediawiki, QObject* const parent=nullptr);
    ~WMTalker();

    QString buildWikiText(const QMap<QString, QString>& info) const;

    void setImageMap(const QMap <QString,QMap <QString,QString> >& imageDesc);
    void start() override;

Q_SIGNALS:

    void uploadProgress(int percent);
    void endUpload();

public Q_SLOTS:

    void begin();
    void uploadHandle(KJob* j = nullptr);
    void slotUploadProgress(KJob* job, unsigned long percent);

private:

    class Private;
    Private* const d;
};

} // namespace KIPIWikiMediaPlugin

#endif // WM_TALKER_H
