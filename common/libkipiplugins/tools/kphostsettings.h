/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-02-12
 * Description : help wrapper around libkipi Interface to manage easily
 *               KIPI host application settings.
 *
 * Copyright (C) 2012-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef KPHOSTSETTINGS_H
#define KPHOSTSETTINGS_H

// Qt includes

#include <QString>

// Local includes

#include "kipiplugins_export.h"
#include "kpmetasettings.h"

namespace KIPIPlugins
{

class KIPIPLUGINS_EXPORT KPHostSettings
{

public:

    /** Default Contructor and destructor.
     */
    KPHostSettings();
    ~KPHostSettings();

    /** Return all file extensions (image, sound, video) managed by host application, separated by
     *  blank spaces, (ex: "JPG PNG TIF NEF AVI MP3").
     */
    QString fileExtensions() const;
    bool    hasFileExtensions() const;

    /** Return images file extensions managed by host application, not incuding RAW formats, separated by
     *  blank spaces, (ex: "JPG PNG TIF").
     */
    QString imageExtensions() const;
    bool    hasImageExtensions() const;

    /** Return RAW file extensions managed by host application, separated by
     *  blank spaces, (ex: "NEF CR2 ARW PEF").
     */
    QString rawExtensions() const;
    bool    hasRawExtensions() const;

    /** Return video file extensions managed by host application, separated by
     *  blank spaces, (ex: "AVI MOV MPG").
     */
    QString videoExtensions() const;
    bool    hasVideoExtensions() const;

    /** Return audio file extensions managed by host application, separated by
     *  blank spaces, (ex: "MP3 WAV OGG").
     */
    QString audioExtensions() const;
    bool    hasAudioExtensions() const;

    /** Return metadata settings container from KIPI host application.
     */
    KPMetaSettings metadataSettings() const;

private:

    class Private;
    Private* const d;
};

} // namespace KIPIPlugins

#endif  // KPHOSTSETTINGS_H
