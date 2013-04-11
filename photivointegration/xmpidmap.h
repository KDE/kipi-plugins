/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-07-28
 * Description : Integration of the Photivo RAW-Processor.
 *
 * Copyright (C) 2012 by Dominic Lyons <domlyons at googlemail dot com>
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

#ifndef XMPIDMAP_H
#define XMPIDMAP_H

// Qt includes

#include <qstring.h>
#include <qmutex.h>
#include <qsettings.h>
#include <qdir.h>

//using namespace KIPIPlugins;

namespace KIPIPhotivoIntegrationPlugin
{

/*! ***************************************************************************
 * Thread-safe singelton wrapper around QSettings 
 */
class XmpIDMap
{

public:

    /*! Get singleton intance */
    static XmpIDMap& getMap()
    {
        static XmpIDMap map;

        return map;
    }

    /*! Set value by key. If this key already exists the value gets replaced,
     *  otherwise the key-value-pair gets added. */
    void setValue (const QString& key, const QString& value)
    {
        QMutexLocker locker(&mutex);

        settings.setValue(key, QVariant (value));
        settings.sync();
    }

    /*! Get value by key */
    QString value (const QString& key) const
    {
        QMutexLocker locker (&mutex);

        return settings.value(key).toString();
    }

private:

    QSettings      settings;
    QDateTime      lastModified;
    mutable QMutex mutex;

    /*! Initialize settings file path (<code>%PhotivoUserDirectory%/xmpmmid.ini</code>)
     *  and format (INI-Format, UTF-8) */
    XmpIDMap()
        : settings(settingsFilePath(), QSettings::IniFormat)
    {
        settings.setIniCodec("UTF-8");
    }

    /*! Helper to make initialization of <code>settings</code> in ctor less ugly */
    static QString settingsFilePath()
    {
        const QString filename = "xmpmmid.ini";
        const QString ptPath   = "/.photivo/";

        return QDir::homePath() + ptPath + filename;
    }
};

} // namespace KIPIPhotivoIntegrationPlugin

#endif // XMPIDMAP_H

