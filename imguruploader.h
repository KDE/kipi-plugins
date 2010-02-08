/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-02-04
 * Description : a tool to export or import image to imgur.com
 *
 * Copyright (C) 2010 by Marius Orcisk <marius at habarnam dot ro>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */
#ifndef IMGURUPLOADER_H
#define IMGURUPLOADER_H

// api key from imgur
#define _IMGUR_API_KEY "2da1cc4923f33dc72885aa32adede5c3";
#define _IMGUR_UPLOAD_URL "http://imgur.com/api/upload";

#include "plugin_imgurexport.h"

// Qt
#include <QObject>

// KDE
#include <kurl.h>
#include <kio/jobclasses.h>

namespace KIO
{
    class Job;
}
namespace KIPIImgurExportPlugin
{
class ImgurUploader : public QObject
{
Q_OBJECT
enum State
{
    IE_ADDPHOTO = 0,
    IE_RESPONSE_RECEIVED,
    IE_OK,
    IE_FAILED
};

public:
    ImgurUploader(QObject *parent = 0);

Q_SIGNALS:
    void signalError( const QString& msg );
    void signalBusy( bool val );
    void signalAddPhotoDone(int, const QString&);

private:
    QString apiKey;
    QString postUrl;
    bool addPhoto(const QString& photoPath);

    State      m_state;

};
} // namespace KIPIImgurExportPlugin
#endif // IMGURUPLOADER_H
