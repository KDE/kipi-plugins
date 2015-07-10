/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a kipi plugin to export images to Google-Drive web service
 *
 * Copyright (C) 2013 by Pankaj Kumar <me at panks dot me>
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

#include <gdtalker.moc>

// C++ includes

#include <ctime>

// Qt includes

#include <QByteArray>
#include <QtAlgorithms>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QList>
#include <QVariant>
#include <QVariantList>
#include <QVariantMap>
#include <QPair>
#include <QFileInfo>

// KDE includes

#include <kcodecs.h>
#include <kdebug.h>
#include <kio/job.h>
#include <kio/jobuidelegate.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <ktoolinvocation.h>
#include <kstandarddirs.h>

// LibKDcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

// LibQJson

#include <qjson/parser.h>

// local includes

#include "kpversion.h"
#include "kpmetadata.h"
#include "gswindow.h"
#include "gsitem.h"
#include "mpform_gdrive.h"

namespace KIPIGoogleServicesPlugin
{
  
static bool gdriveLessThan(GSFolder& p1, GSFolder& p2)
{
    return (p1.title.toLower() < p2.title.toLower());
}  

GDTalker::GDTalker(QWidget* const parent)
    :Authorize(parent,QString("https://www.googleapis.com/auth/drive")), m_state(GD_LOGOUT)
{
    m_rootid          = "root";
    m_rootfoldername  = "GoogleDrive Root";
}

GDTalker::~GDTalker()
{

}

/** Gets username
 */
void GDTalker::getUserName()
{
    KUrl url("https://www.googleapis.com/drive/v2/about");
    url.addQueryItem("scope", m_scope);
    url.addQueryItem("access_token", m_access_token);
    QString auth = "Authorization: " + m_bearer_access_token.toAscii();

    KIO::TransferJob* job = KIO::get(url,KIO::NoReload,KIO::HideProgressInfo);
    job->addMetaData("content-type","Content-Type: application/json");
    job->addMetaData("customHTTPHeader",auth.toAscii());

    connect(job,SIGNAL(data(KIO::Job*,QByteArray)),
            this,SLOT(data(KIO::Job*,QByteArray)));

    connect(job,SIGNAL(result(KJob*)),
            this,SLOT(slotResult(KJob*)));

    m_state = GD_USERNAME;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy(true);
}

/** Gets list of folder of user in json format
 */
void GDTalker::listFolders()
{
    KUrl url("https://www.googleapis.com/drive/v2/files?q=mimeType = 'application/vnd.google-apps.folder'");
    QString auth = "Authorization: " + m_bearer_access_token.toAscii();
    kDebug() << auth;
    KIO::TransferJob* const job = KIO::get(url,KIO::NoReload,KIO::HideProgressInfo);
    job->addMetaData("content-type","Content-Type: application/json");
    job->addMetaData("customHTTPHeader",auth.toAscii());

    connect(job,SIGNAL(data(KIO::Job*,QByteArray)),
            this,SLOT(data(KIO::Job*,QByteArray)));

    connect(job,SIGNAL(result(KJob*)),
            this,SLOT(slotResult(KJob*)));

    m_state = GD_LISTFOLDERS;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy(true);
}

/** Creates folder inside any folder(of which id is passed)
 */
void GDTalker::createFolder(const QString& title,const QString& id)
{
    if(m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    KUrl url("https://www.googleapis.com/drive/v2/files");
    QByteArray data;
    data += "{\"title\":\"";
    data += title.toAscii();
    data += "\",\r\n";
    data += "\"parents\":";
    data += "[{";
    data += "\"id\":\"";
    data += id.toAscii();
    data += "\"}],\r\n";
    data += "\"mimeType\":";
    data += "\"application/vnd.google-apps.folder\"";
    data += "}\r\n";

    kDebug() << "data:" << data;
    QString auth = "Authorization: " + m_bearer_access_token.toAscii();
    KIO::TransferJob* const job = KIO::http_post(url,data,KIO::HideProgressInfo);
    job->addMetaData("content-type","Content-Type: application/json");
    job->addMetaData("customHTTPHeader",auth.toAscii());

    connect(job,SIGNAL(data(KIO::Job*,QByteArray)),
            this,SLOT(data(KIO::Job*,QByteArray)));

    connect(job,SIGNAL(result(KJob*)),
            this,SLOT(slotResult(KJob*)));

    m_state = GD_CREATEFOLDER;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy(true);
}

bool GDTalker::addPhoto(const QString& imgPath,const GSPhoto& info,const QString& id,bool rescale,int maxDim,int imageQuality)
{
    if(m_job)
    {
        m_job->kill();
        m_job = 0;
    }
    emit signalBusy(true);
    MPForm_GDrive form;
    form.addPair(KUrl(imgPath).fileName(),info.description,imgPath,id);
    QString path = imgPath;
    QImage image;

    if(KPMetadata::isRawFile(imgPath))
    {
        KDcrawIface::KDcraw::loadRawPreview(image,imgPath);
    }
    else
    {
        image.load(imgPath);
    }

    if(image.isNull())
    {
        return false;
    }

    path = KStandardDirs::locateLocal("tmp",QFileInfo(imgPath).baseName().trimmed()+".jpg");

    if(rescale && (image.width() > maxDim || image.height() > maxDim)){
        image = image.scaled(maxDim,maxDim,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    }

    image.save(path,"JPEG",imageQuality);

    KPMetadata meta;

    if(meta.load(imgPath))
    {
        meta.setImageDimensions(image.size());
        meta.setImageProgramId("Kipi-plugins",kipiplugins_version);
        meta.save(path);
    }

    if(!form.addFile(path))
    {
        emit signalBusy(false);
        return false;
    }

    form.finish();

    QString auth = "Authorization: " + m_bearer_access_token.toAscii();
    KUrl url("https://www.googleapis.com/upload/drive/v2/files?uploadType=multipart");
    KIO::TransferJob* job = KIO::http_post(url,form.formData(),KIO::HideProgressInfo);
    job->addMetaData("content-type",form.contentType());
    job->addMetaData("content-length","Content-Length:"+form.getFileSize());
    job->addMetaData("customHTTPHeader",auth.toAscii());
    job->addMetaData("host","Host:www.googleapis.com");

    connect(job,SIGNAL(data(KIO::Job*,QByteArray)),
            this,SLOT(data(KIO::Job*,QByteArray)));

    connect(job,SIGNAL(result(KJob*)),
            this,SLOT(slotResult(KJob*)));

    kDebug() << "In add photo";
    m_state = GD_ADDPHOTO;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy(true);
    return true;
}

void GDTalker::slotResult(KJob* kjob)
{
    m_job = 0;
    KIO::Job* const job = static_cast<KIO::Job*>(kjob);

    if(job->error())
    {
        emit signalBusy(false);
        job->ui()->setWindow(m_parent);
        job->ui()->showErrorMessage();
        return;
    }

    switch(m_state)
    {
        case(GD_LOGOUT):
            break;
        case (GD_LISTFOLDERS):
            kDebug() << "In GD_LISTFOLDERS";
            parseResponseListFolders(m_buffer);
            break;
        case (GD_CREATEFOLDER):
            kDebug() << "In GD_CREATEFOLDER";
            parseResponseCreateFolder(m_buffer);
            break;
        case (GD_ADDPHOTO):
            kDebug() << "In GD_ADDPHOTO";// << m_buffer;
            parseResponseAddPhoto(m_buffer);
            break;
        case (GD_USERNAME):
            kDebug() << "In GD_USERNAME";// << m_buffer;
            parseResponseUserName(m_buffer);
            break;
        default:
            break;
    }
}

void GDTalker::parseResponseUserName(const QByteArray& data)
{
    QJson::Parser parser;

    bool ok;
    // json is a QString containing the data to convert
    QVariant result = parser.parse(data, &ok);

    if(!ok)
    {
        emit signalBusy(false);
        return;
    }

    kDebug() << "in parseResponseUserName";
    QVariantMap rlist = result.toMap();
    kDebug() << "size " << rlist.size();
    QList<QString> keys = rlist.uniqueKeys();

    QString temp;

    for(int i=0;i<rlist.size();i++)
    {
        if(keys[i] == "name")
        {
            kDebug() << "username:" << rlist[keys[i]].value<QString>();
            temp = rlist[keys[i]].value<QString>();
            //break;
        }
        if(keys[i] == "user")
        {
            kDebug() << "USername:" << rlist[keys[i]];
            //temp = rlist[keys[i]].value<QString>();
            break;
        }
    }

    emit signalBusy(false);
    emit signalSetUserName(temp);
}

void GDTalker::parseResponseListFolders(const QByteArray& data)
{
    kDebug()<<"DATA is"<<data;
    QJson::Parser parser;
    bool ok;
    QVariant result = parser.parse(data,&ok);

    if(!ok)
    {
        emit signalBusy(false);
        emit signalListAlbumsDone(0,i18n("Failed to list folders"),QList<GSFolder>());
        return;
    }

    QVariantMap rMap = result.toMap();
    QList<QPair<QString,QString> > list;
    list.append(qMakePair(m_rootid,m_rootfoldername));
    
    QList<GSFolder> albumList;
    GSFolder fps;
    fps.id = m_rootid;
    fps.title = m_rootfoldername;
    albumList.append(fps);

    foreach(QVariant val,rMap)
    {
        QVariantList abcd = val.toList();

        foreach(QVariant abc,abcd)
        {
            QVariantMap qwer    = abc.toMap();
            QList<QString> keys = qwer.uniqueKeys();
            QString temp;

            for(int i=0;i<qwer.size();i++)
            {
                if(keys[i] == "id")
                {
                    temp = qwer[keys[i]].value<QString>();
                    fps.id = qwer[keys[i]].value<QString>();
                }
                else if(keys[i] == "title")
                {
                    fps.title = qwer[keys[i]].value<QString>();
                    albumList.append(fps);
                    list.append(qMakePair(temp,qwer[keys[i]].value<QString>()));
                }
            }
        }
    }

    qSort(albumList.begin(), albumList.end(), gdriveLessThan);
    emit signalBusy(false);
    emit signalListAlbumsDone(1,QString(),albumList);
}

void GDTalker::parseResponseCreateFolder(const QByteArray& data)
{
    QJson::Parser parser;

    bool ok;
    bool success        = false;
    QVariant result     = parser.parse(data, &ok);
    QVariantMap rMap    = result.toMap();
    QList<QString> keys = rMap.uniqueKeys();

    kDebug() << "in parse folder" << rMap.size();

    for(int i=0;i<rMap.size();i++)
    {
        if(keys[i]  == "alternateLink")
        {
            success = true;
            break;
        }
    }

    emit signalBusy(false);

    if(!success)
    {
        emit signalCreateFolderDone(0,i18n("Failed to create folder"));
    }
    else
    {
        emit signalCreateFolderDone(1,QString());
    }
}

void GDTalker::parseResponseAddPhoto(const QByteArray& data)
{
    QJson::Parser parser;

    bool ok;
    bool success = false;
    QVariant result = parser.parse(data, &ok);
    QVariantMap rMap = result.toMap();
    QList<QString> keys = rMap.uniqueKeys();
    QString photoId("");
    kDebug() << "in parse folder" << rMap.size();

    for(int i=0;i<rMap.size();i++)
    {
        if(keys[i]  == "alternateLink")
        {
            success = true;
        }
        if(keys[i] == "id")
            photoId = rMap[keys[i]].value<QString>();
    }

    emit signalBusy(false);

    if(!success)
    {
        emit signalAddPhotoDone(0,i18n("Failed to upload photo"),"-1");
    }
    else
    {
        emit signalAddPhotoDone(1,QString(),photoId);
    }
}

void GDTalker::cancel()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    emit signalBusy(false);
}

} // namespace KIPIGoogleServicesPlugin
