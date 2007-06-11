/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-12-03
 * Description : a class to manage plugin actions using threads
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef ACTIONTHREAD_H
#define ACTIONTHREAD_H

// Qt includes.

#include <qthread.h>
#include <qstring.h>

// KDE includes.

#include <kurl.h>

// LibKDcraw includes.

#include <libkdcraw/rawdecodingsettings.h>

// Local includes.

#include "rawdecodingiface.h"
#include "savesettingswidget.h"
#include "actions.h"
#include "mtqueue.h"

class QObject;

namespace KIPIRawConverterPlugin
{

class ActionThread : public QThread
{

public:

    ActionThread(QObject *parent);
    ~ActionThread();

    void setRawDecodingSettings(KDcrawIface::RawDecodingSettings rawDecodingSettings, 
                                SaveSettingsWidget::OutputFormat outputFormat);

    void identifyRawFile(const KURL& url, bool full=false);
    void identifyRawFiles(const KURL::List& urlList, bool full=false);

    void processHalfRawFile(const KURL& url);
    void processHalfRawFiles(const KURL::List& urlList);

    void processRawFile(const KURL& url);
    void processRawFiles(const KURL::List& urlList);

    void cancel();

protected:

    void run();

private:

    struct Task_ 
    {
        QString                          filePath;
        Action                           action;
        SaveSettingsWidget::OutputFormat outputFormat;
        KDcrawIface::RawDecodingSettings decodingSettings;
    };

    typedef struct Task_ Task;

    QObject                          *m_parent;

    SaveSettingsWidget::OutputFormat  m_outputFormat;

    KDcrawIface::RawDecodingSettings  m_rawDecodingSettings;

    RawDecodingIface                  m_dcrawIface;

    MTQueue<Task>                     m_taskQueue;
};

}  // NameSpace KIPIRawConverterPlugin

#endif /* ACTIONTHREAD_H */
