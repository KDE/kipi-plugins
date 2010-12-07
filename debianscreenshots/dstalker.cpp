/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-11-29
 * Description : a kipi plugin to export images to Debian Screenshots
 *
 * Copyright (C) 2010 by Pau Garcia i Quiles <pgquiles at elpauer dot org>
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

#include "dstalker.h"
#include "dstalker.moc"

// C++ includes

#include <ctime>

// Qt includes

#include <QByteArray>
#include <QDomDocument>
#include <QDomElement>
#include <QtAlgorithms>

// KDE includes

#include <KCodecs>
#include <kdebug.h>
#include <KIO/Job>
#include <KIO/JobUiDelegate>
#include <KApplication>
#include <KMessageBox>
#include <KToolInvocation>

// Local includes

#include "pluginsversion.h"
#include "mpform.h"
#include "debshots.h"

namespace KIPIDebianScreenshotsPlugin
{

DsTalker::DsTalker(QWidget* parent) : QObject(parent), m_parent(parent), m_job(0)
{
    m_parent = parent;

    m_userAgent  = QString("KIPI-Plugin-DebianScreenshots/%1 (pgquiles@elpauer.org)").arg(kipiplugins_version);
    m_uploadUrl = KIPIDebianScreenshotsPlugin::debshotsUrl + "/uploadfile";
}

DsTalker::~DsTalker()
{
    if (m_job)
    {
        m_job->kill();
    }
}

bool DsTalker::addScreenshot(const QString& imgPath, const QString& packageName,
                             const QString& packageVersion, const QString& description)
{
    kDebug() << "Adding screenshot " << imgPath << " to package "
             << packageName << " " << packageVersion<< " using description '" << description << "'";

    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }
    emit signalBusy(true);

    MPForm  form;
    form.addPair("packagename", packageName);
    form.addPair("version", packageVersion);
    form.addPair("description", description);
    form.addFile(imgPath, imgPath, "file");
    form.finish();

    kDebug() << "FORM: " << endl << form.formData();

    KIO::TransferJob* job = KIO::http_post(m_uploadUrl, form.formData(), KIO::HideProgressInfo);
    job->addMetaData("UserAgent", m_userAgent);
    job->addMetaData("content-type", form.contentType());

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(data(KIO::Job*, const QByteArray&)));

    connect(job, SIGNAL(result(KJob *)),
            this, SLOT(slotResult(KJob *)));

    m_job   = job;
    m_buffer.resize(0);
    return true;
}

 void DsTalker::data(KIO::Job*, const QByteArray& data)
 {
     kDebug() << Q_FUNC_INFO;

     if (data.isEmpty())
     {
         return;
     }

     int oldSize = m_buffer.size();
     m_buffer.resize(m_buffer.size() + data.size());
     memcpy(m_buffer.data()+oldSize, data.data(), data.size());
 }
 void DsTalker::slotResult(KJob* kjob)
 {
     m_job         = 0;
     KIO::Job* job = static_cast<KIO::Job*>(kjob);

     if (job->error())
     {
         emit signalBusy(false);
         emit signalAddScreenshotDone(job->error(), job->errorText());
     }

     emit signalBusy(false);
     emit signalAddScreenshotDone(0, QString());

//     else
//     {
//         kDebug() << "Uploaded successfully screenshot " << job->queryMetaData("Screenshot")
//                  << " to Debian Screenshots for package " << job->queryMetaData("Package")
//                  << " " << job->queryMetaData("Version")
//                  << " with description " << job->queryMetaData("Description");
//     }

 }

} // namespace KIPIDebianScreenshotsPlugin
