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
#include <qjson/parser.h>
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
#include "gdwindow.h"
#include "gditem.h"
#include "mpform.h"

namespace KIPIGoogleDrivePlugin
{

GDTalker::GDTalker(QWidget* const parent)
{
    m_parent          = parent;
    m_scope           = "https://www.googleapis.com/auth/drive";
    m_redirect_uri    = "urn:ietf:wg:oauth:2.0:oob";
    m_response_type   = "code";
    m_client_id       = "735222197981-mrcgtaqf05914buqjkts7mk79blsquas.apps.googleusercontent.com";
    m_token_uri       = "https://accounts.google.com/o/oauth2/token";
    m_client_secret   = "4MJOS0u1-_AUEKJ0ObA-j22U";
    m_code            = "0";
    m_rootid          = "root";
    m_rootfoldername  = "GoogleDrive Root";
    m_job             = 0;
    continuePos       = 0;
    m_state           = GD_ACCESSTOKEN;
}

GDTalker::~GDTalker()
{
    if (m_job)
        m_job->kill();
}

bool GDTalker::authenticated()
{
    if(m_access_token.isEmpty())
    {
        return false;
    }

    return true;
}

/** Starts authentication by opening the browser
 */
void GDTalker::doOAuth()
{
    KUrl url("https://accounts.google.com/o/oauth2/auth");
    url.addQueryItem("scope",m_scope);
    url.addQueryItem("redirect_uri",m_redirect_uri);
    url.addQueryItem("response_type",m_response_type);
    url.addQueryItem("client_id",m_client_id);
    url.addQueryItem("access_type","offline");
    kDebug() << "OAuth URL: " << url;
    KToolInvocation::invokeBrowser(url.url());

    emit signalBusy(false);

    KDialog* const window         = new KDialog(kapp->activeWindow(),0);
    window->setModal(true);
    window->setWindowTitle(i18n("Google Drive Authorization"));
    window->setButtons(KDialog::Ok | KDialog::Cancel);
    QWidget* const main           = new QWidget(window,0);
    QLineEdit* const textbox      = new QLineEdit();
    QPlainTextEdit* const infobox = new QPlainTextEdit(i18n("Please follow instructions in the browser."
                                                            "After logging in and authorizing app copy code from browser and paste below in textbox and press OK"));
    infobox->setReadOnly(true);
    QVBoxLayout* const layout = new QVBoxLayout;
    layout->addWidget(infobox);
    layout->addWidget(textbox);
    main->setLayout(layout);
    window->setMainWidget(main);

    if(window->exec() == QDialog::Accepted && !(textbox->text().isEmpty()))
    {
        kDebug() << "1";
        m_code = textbox->text();
    }

    if(textbox->text().isEmpty())
    {
        kDebug() << "3";
        emit signalTextBoxEmpty();
    }

    if(m_code != "0")
    {
        getAccessToken();
    }
}

/** Gets access token from googledrive after authentication by user
 */
void GDTalker::getAccessToken()
{
    KUrl url("https://accounts.google.com/o/oauth2/token?");
    url.addQueryItem("scope",m_scope.toAscii());
    url.addQueryItem("response_type",m_response_type.toAscii());
    url.addQueryItem("token_uri",m_token_uri.toAscii());
    QByteArray postData;
    postData = "code=";
    postData += m_code.toAscii();
    postData += "&client_id=";
    postData += m_client_id.toAscii();
    postData += "&client_secret=";
    postData += m_client_secret.toAscii();
    postData += "&redirect_uri=";
    postData += m_redirect_uri.toAscii();
    postData += "&grant_type=authorization_code";

    KIO::TransferJob* const job = KIO::http_post(url,postData,KIO::HideProgressInfo);
    job->addMetaData("content-type","Content-Type: application/x-www-form-urlencoded");

    connect(job,SIGNAL(data(KIO::Job*,QByteArray)),
            this,SLOT(data(KIO::Job*,QByteArray)));

    connect(job,SIGNAL(result(KJob*)),
            this,SLOT(slotResult(KJob*)));

    m_state = GD_ACCESSTOKEN;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy(true);
}

/** Gets access token from refresh token for handling login of user across digikam sessions
 */
void GDTalker::getAccessTokenFromRefreshToken(const QString& msg)
{
    KUrl url("https://accounts.google.com/o/oauth2/token");

    QByteArray postData;
    postData = "&client_id=";
    postData += m_client_id.toAscii();
    postData += "&client_secret=";
    postData += m_client_secret.toAscii();
    postData += "&refresh_token=";
    postData += msg.toAscii();
    postData += "&grant_type=refresh_token";

    KIO::TransferJob* const job = KIO::http_post(url,postData,KIO::HideProgressInfo);
    job->addMetaData("content-type","Content-Type: application/x-www-form-urlencoded");

    connect(job,SIGNAL(data(KIO::Job*,QByteArray)),
            this,SLOT(data(KIO::Job*,QByteArray)));

    connect(job,SIGNAL(result(KJob*)),
            this,SLOT(slotResult(KJob*)));

    m_state = GD_REFRESHTOKEN;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy(true);
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

bool GDTalker::addPhoto(const QString& imgPath,const GDPhoto& info,const QString& id,bool rescale,int maxDim,int imageQuality)
{
    if(m_job)
    {
        m_job->kill();
        m_job = 0;
    }
    emit signalBusy(true);
    MPForm form;
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

void GDTalker::data(KIO::Job*,const QByteArray& data)
{
    if(data.isEmpty())
    {
        return;
    }

    int oldsize = m_buffer.size();
    m_buffer.resize(m_buffer.size() + data.size());
    memcpy(m_buffer.data()+oldsize,data.data(),data.size());
}

void GDTalker::slotResult(KJob* kjob)
{
    m_job = 0;
    KIO::Job* const job = static_cast<KIO::Job*>(kjob);

    if(job->error())
    {
        if(m_state == GD_ACCESSTOKEN)
        {
            emit signalBusy(false);
            emit signalAccessTokenFailed(job->error(),job->errorText());
        }
        else
        {
            emit signalBusy(false);
            job->ui()->setWindow(m_parent);
            job->ui()->showErrorMessage();
        }

        return;
    }

    switch(m_state)
    {
        case (GD_ACCESSTOKEN):
            kDebug() << "In GD_ACCESSTOKEN";// << m_buffer;
            parseResponseAccessToken(m_buffer);
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
        case (GD_REFRESHTOKEN):
            kDebug() << "In GD_REFRESHTOKEN" << m_buffer;
            parseResponseRefreshToken(m_buffer);
            break;
        default:
            break;
    }
}

void GDTalker::parseResponseAccessToken(const QByteArray& data)
{
    m_access_token  = getValue(data,"access_token");
    m_refresh_token = getValue(data,"refresh_token");

    if(getValue(data,"error") == "invalid_request" || getValue(data,"error") == "invalid_grant")
    {
        doOAuth();
        return;
    }

    m_bearer_access_token = "Bearer " + m_access_token;
    kDebug() << "In parse GD_ACCESSTOKEN" << m_bearer_access_token << "  " << data;
    //emit signalAccessTokenObtained();
    emit signalRefreshTokenObtained(m_refresh_token);
}

void GDTalker::parseResponseRefreshToken(const QByteArray& data)
{
    m_access_token = getValue(data,"access_token");

    if(getValue(data,"error") == "invalid_request" || getValue(data,"error") == "invalid_grant")
    {
        doOAuth();
        return;
    }

    m_bearer_access_token = "Bearer " + m_access_token;
    kDebug() << "In parse GD_ACCESSTOKEN" << m_bearer_access_token << "  " << data;
    emit signalAccessTokenObtained();
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
    qDebug() << "size " << rlist.size();
    QList<QString> keys = rlist.uniqueKeys();

    QString temp;

    for(int i=0;i<rlist.size();i++)
    {
        if(keys[i] == "name")
        {
            kDebug() << "username:" << rlist[keys[i]].value<QString>();
            temp = rlist[keys[i]].value<QString>();
            break;
        }
    }

    emit signalBusy(false);
    emit signalSetUserName(temp);
}

void GDTalker::parseResponseListFolders(const QByteArray& data)
{
    QJson::Parser parser;
    bool ok;
    QVariant result = parser.parse(data,&ok);

    if(!ok)
    {
        emit signalBusy(false);
        emit signalListAlbumsFailed(i18n("Failed to list Folders"));
        return;
    }

    QVariantMap rMap = result.toMap();
    QList<QPair<QString,QString> > list;
    list.append(qMakePair(m_rootid,m_rootfoldername));

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
                }
                else if(keys[i] == "title")
                {
                    list.append(qMakePair(temp,qwer[keys[i]].value<QString>()));
                }
            }
        }
    }

    emit signalBusy(false);
    emit signalListAlbumsDone(list);
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
        emit signalCreateFolderFailed(i18n("Failed to Create Folder"));
    }
    else
    {
        emit signalCreateFolderSucceeded();
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
        emit signalAddPhotoFailed(i18n("Failed to Upload Photo"));
    }
    else
    {
        emit signalAddPhotoSucceeded();
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

QString GDTalker::getValue(const QString& jsonStr, const QString& key)
{
    QString token(getToken(jsonStr, key, QString(",")));

    token.remove(QRegExp("[\"}]"));

    QStringList tokenValues(token.split(": "));
    QString value;

    if (tokenValues.count() == 2) 
        value = tokenValues[1].trimmed();

    return value;
}

QStringList GDTalker::getParams(const QString& jsonStr, const QStringList& pathValues, const QString& key)
{
    if(pathValues.count() == 0)
        return QStringList();

    QString token(getToken(jsonStr, pathValues[0], QString("]")));

    for(int i = 1; i < pathValues.count(); ++i)
    {
        token = getToken(token, pathValues[i], QString("]"));
    }

    QStringList tokens;
    QString nextToken;

    continuePos = 0;

    while(!(nextToken = getValue(token, key)).isEmpty())
    {
        token = token.mid(continuePos);
        tokens << nextToken;
    }

    return tokens;
}

QString GDTalker::getToken(const QString& object, const QString& key, const QString& endDivider)
{
    QString searchToken(QString("\"") + key + QString("\""));

    int beginPos(object.indexOf(searchToken));

    if (beginPos == -1)
        return QString();

    int endPos;

    if (endDivider == QString(","))
        endPos = object.indexOf(endDivider, beginPos);
    else
        endPos = getTokenEnd(object, beginPos);

    int strLength = endPos - beginPos;
    QString token(object.mid(beginPos, strLength));

    if(endPos != -1)
        continuePos = endPos;
    else
        continuePos = beginPos + token.length();

    return token;
}

int GDTalker::getTokenEnd(const QString& object, int beginPos)
{
    int beginDividerPos(object.indexOf(QString("["), beginPos ));
    int endDividerPos(object.indexOf(QString("]"), beginPos + 1));

    while((beginDividerPos < endDividerPos) && beginDividerPos != -1)
    {
        beginDividerPos = object.indexOf(QString("["), endDividerPos);
        endDividerPos = object.indexOf(QString("]"), endDividerPos + 1);
    }

    return endDividerPos + 1;
}

} // namespace KIPIGoogleDrivePlugin
