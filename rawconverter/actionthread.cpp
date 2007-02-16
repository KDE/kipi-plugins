/* ============================================================
 * Authors: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *          Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2003-12-03
 * Description : a class to manage plugin actions using threads
 *
 * Copyright 2003-2005 by Renchi Raju
 * Copyright 2006-2007 by Gilles Caulier
 *
 * NOTE: Do not use kdDebug() in this implementation because 
 *       it will be multithreaded. Use qDebug() instead. 
 *       See B.K.O #133026 for details.
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

// C Ansi includes.

extern "C"
{
#include <unistd.h>
}

// Qt includes.

#include <qapplication.h>
#include <qdir.h>
#include <qdeepcopy.h>

// KDE includes.

#include <klocale.h>
#include <kstandarddirs.h>

// LibKDcraw includes.

#include <libkdcraw/dcrawinfocontainer.h>

// Local includes.

#include "actionthread.h"

namespace KIPIRawConverterPlugin
{

ActionThread::ActionThread(QObject *parent)
            : QThread(), m_parent(parent)
{
}

ActionThread::~ActionThread()
{
    // cancel the thread
    cancel();
    // wait for the thread to finish
    wait();
}

void ActionThread::identifyRawFile(const KURL& url)
{
    KURL::List oneFile;
    oneFile.append(url);
    identifyRawFiles(oneFile);
}

void ActionThread::processRawFile(const KURL& url)
{
    KURL::List oneFile;
    oneFile.append(url);
    processRawFiles(oneFile);
}

void ActionThread::processHalfRawFile(const KURL& url)
{
    KURL::List oneFile;
    oneFile.append(url);
    processHalfRawFiles(oneFile);
}

void ActionThread::identifyRawFiles(const KURL::List& urlList)
{
    for (KURL::List::const_iterator it = urlList.begin();
         it != urlList.end(); ++it ) 
    {
        Task *t     = new Task;
        t->filePath = QDeepCopy<QString>((*it).path()); //deep copy
        t->action   = IDENTIFY;
        m_taskQueue.enqueue(t);
    }
}

void ActionThread::setRawDecodingSettings(KDcrawIface::RawDecodingSettings rawDecodingSettings, 
                                          SaveSettingsWidget::OutputFormat outputFormat)
{
    m_rawDecodingSettings = rawDecodingSettings;
    m_outputFormat        = outputFormat;
}

void ActionThread::processRawFiles(const KURL::List& urlList)
{
    for (KURL::List::const_iterator it = urlList.begin();
         it != urlList.end(); ++it ) 
    {
        Task *t             = new Task;
        t->filePath         = QDeepCopy<QString>((*it).path()); //deep copy
        t->outputFormat     = m_outputFormat;
        t->decodingSettings = m_rawDecodingSettings;
        t->action           = PROCESS;
        m_taskQueue.enqueue(t);
    }
}

void ActionThread::processHalfRawFiles(const KURL::List& urlList)
{
    for (KURL::List::const_iterator it = urlList.begin();
         it != urlList.end(); ++it ) 
    {
        Task *t             = new Task;
        t->filePath         = QDeepCopy<QString>((*it).path()); //deep copy
        t->outputFormat     = m_outputFormat;
        t->decodingSettings = m_rawDecodingSettings;
        t->action           = PREVIEW;
        m_taskQueue.enqueue(t);
    }
}

void ActionThread::cancel()
{
    m_taskQueue.flush();
    m_dcrawIface.cancel();
}

void ActionThread::run()
{
    while (!m_taskQueue.isEmpty()) 
    {
        Task *t = m_taskQueue.dequeue();
        if (!t) continue;

        QString errString;

        EventData *d = new EventData;

        switch (t->action) 
        {
            case IDENTIFY: 
            {
                // Get embedded RAW file thumbnail.
                QImage image;
                m_dcrawIface.loadDcrawPreview(image, t->filePath);

                // Identify Camera model.    
                KDcrawIface::DcrawInfoContainer info;
                m_dcrawIface.rawFileIdentify(info, t->filePath);

                QString identify = i18n("Cannot identify RAW file");
                if (info.isDecodable)
                    identify = info.make + QString(" ") + info.model;

                EventData *r = new EventData;
                r->action    = IDENTIFY;
                r->filePath  = t->filePath;
                r->image     = image;
                r->message   = identify;
                r->success   = true;
                QApplication::postEvent(m_parent, new QCustomEvent(QEvent::User, r));
                break;
            }

            case PREVIEW: 
            {
                d->action    = PREVIEW;
                d->filePath  = t->filePath;
                d->starting  = true;
                QApplication::postEvent(m_parent, new QCustomEvent(QEvent::User, d));

                QString destPath;
                bool result  = m_dcrawIface.decodeHalfRAWImage(t->filePath, destPath, 
                                                               t->outputFormat, t->decodingSettings);

                EventData *r = new EventData;
                r->action    = PREVIEW;
                r->filePath  = t->filePath;
                r->destPath  = destPath;
                r->success   = result;
                QApplication::postEvent(m_parent, new QCustomEvent(QEvent::User, r));
                break;
            }

            case PROCESS: 
            {
                d->action    = PROCESS;
                d->filePath  = t->filePath;
                d->starting  = true;
                QApplication::postEvent(m_parent, new QCustomEvent(QEvent::User, d));

                QString destPath;
                bool result  = m_dcrawIface.decodeRAWImage(t->filePath, destPath, 
                                                           t->outputFormat, t->decodingSettings);

                EventData *r = new EventData;
                r->action    = PROCESS;
                r->filePath  = t->filePath;
                r->destPath  = destPath;
                r->success   = result;
                QApplication::postEvent(m_parent, new QCustomEvent(QEvent::User, r));
                break;
            }

            default: 
            {
                qWarning("KIPIRawConverterPlugin:ActionThread: Unknown action specified");
                delete d;
            }
        }

        delete t;
    }
}

}  // NameSpace KIPIRawConverterPlugin
