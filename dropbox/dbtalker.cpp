/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a kipi plugin to import/export images to Dropbox web service
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

#include <dbtalker.moc>

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
#include <QDateTime>
#include <QWidget>

// KDE includes

#include <kcodecs.h>
#include <kdebug.h>
#include <kio/job.h>
#include <kio/jobuidelegate.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <ktoolinvocation.h>
#include <kstandarddirs.h>
#include <kdialog.h>

// LibKDcraw includes

#include <libkdcraw/version.h>
#include <libkdcraw/kdcraw.h>

// LibQJSon

#include <qjson/parser.h>

// Local includes

#include "kpversion.h"
#include "kpmetadata.h"
#include "dbwindow.h"
#include "dbitem.h"
#include "mpform.h"

namespace KIPIDropboxPlugin
{

DBTalker::DBTalker(QWidget* const parent)
{
    m_parent                 = parent;
    m_oauth_consumer_key     = "kn7kajkaqf6retw";
    m_oauth_signature_method = "PLAINTEXT";
    m_oauth_version          = "1.0";
    m_oauth_signature        = "t9w4c6j837ubstf&";
    nonce                    = generateNonce(8);
    timestamp                = QDateTime::currentMSecsSinceEpoch()/1000;
    m_root                   = "dropbox";
    m_job                    = 0;
    m_state                  = DB_REQ_TOKEN;
    auth                     = false;
}

DBTalker::~DBTalker()
{
}

/** generate a random number
 */
QString DBTalker::generateNonce(qint32 length)
{
    QString clng = "";

    for(int i=0; i<length; ++i)
    {
        clng += QString::number(int( qrand() / (RAND_MAX + 1.0) * (16 + 1 - 0) + 0 ), 16).toUpper();
    }

    return clng;
}

/** dropbox first has to obtain request token before asking user for authorization
 */
void DBTalker::obtain_req_token()
{
    KUrl url("https://api.dropbox.com/1/oauth/request_token");
    url.addQueryItem("oauth_consumer_key",m_oauth_consumer_key);
    url.addQueryItem("oauth_nonce", nonce);
    url.addQueryItem("oauth_signature",m_oauth_signature);
    url.addQueryItem("oauth_signature_method",m_oauth_signature_method);
    url.addQueryItem("oauth_timestamp", QString::number(timestamp));
    url.addQueryItem("oauth_version",m_oauth_version);

    KIO::TransferJob* const job = KIO::http_post(url,"",KIO::HideProgressInfo);
    job->addMetaData("content-type","Content-Type : application/x-www-form-urlencoded");

    connect(job,SIGNAL(data(KIO::Job*,QByteArray)),
            this,SLOT(data(KIO::Job*,QByteArray)));

    connect(job,SIGNAL(result(KJob*)),
            this,SLOT(slotResult(KJob*)));

    auth    = false;
    m_state = DB_REQ_TOKEN;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy(true);
}

bool DBTalker::authenticated()
{
    if(auth)
    {
        return true;
    }

    return false;
}

/** Maintain login across digikam sessions
 */
void DBTalker::continueWithAccessToken(const QString& msg1, const QString& msg2, const QString& msg3)
{
    m_oauthToken             = msg1;
    m_oauthTokenSecret       = msg2;
    m_access_oauth_signature = msg3;
    emit signalAccessTokenObtained(m_oauthToken,m_oauthTokenSecret,m_access_oauth_signature);
}

/** Ask for authorization and login by opening browser
 */
void DBTalker::doOAuth()
{
    KUrl url("https://api.dropbox.com/1/oauth/authorize");
    kDebug() << "in doOAuth()" << m_oauthToken;
    url.addQueryItem("oauth_token",m_oauthToken);

    kDebug() << "OAuth URL: " << url;
    KToolInvocation::invokeBrowser(url.url());

    emit signalBusy(false);

    KDialog* const dialog = new KDialog(kapp->activeWindow(),0);
    dialog->setModal(true);
    dialog->setWindowTitle(i18n("Authorize Dropbox"));
    dialog->setButtons(KDialog::Ok | KDialog::Cancel);
    //dialog->setButtonText(0x00000004,"Authorize");

    QWidget* const mainWidget = new QWidget(dialog,0);

    QPlainTextEdit* const infobox = new QPlainTextEdit( i18n(
          "Please follow the instructions in the browser. "
           "After logging in and authorizing the application, press OK."
                                                           ));

    infobox->setReadOnly(true);
    QVBoxLayout* const layout = new QVBoxLayout;
    layout->addWidget(infobox);
    mainWidget->setLayout(layout);

    dialog->setMainWidget(mainWidget);


    if(dialog->exec() == QDialog::Accepted)
    {
        getAccessToken();
    }
    else
    {
        return;
    }
}

/** Get access token from dropbox
 */
void DBTalker::getAccessToken()
{
    KUrl url("https://api.dropbox.com/1/oauth/access_token");
    url.addQueryItem("oauth_consumer_key",m_oauth_consumer_key);
    url.addQueryItem("oauth_nonce", nonce);
    url.addQueryItem("oauth_signature",m_access_oauth_signature);
    url.addQueryItem("oauth_signature_method",m_oauth_signature_method);
    url.addQueryItem("oauth_timestamp", QString::number(timestamp));
    url.addQueryItem("oauth_version",m_oauth_version);
    url.addQueryItem("oauth_token",m_oauthToken);

    KIO::TransferJob* const job = KIO::http_post(url,"",KIO::HideProgressInfo);
    job->addMetaData("content-type","Content-Type : application/x-www-form-urlencoded");

    connect(job,SIGNAL(data(KIO::Job*,QByteArray)),
            this,SLOT(data(KIO::Job*,QByteArray)));

    connect(job,SIGNAL(result(KJob*)),
            this,SLOT(slotResult(KJob*)));

    m_state = DB_ACCESSTOKEN;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy(true);
}

/** Creates folder at specified path
 */
void DBTalker::createFolder(const QString& path)
{
    //path also has name of new folder so send path parameter accordingly
    kDebug() << "in cre fol " << path;

    KUrl url("https://api.dropbox.com/1/fileops/create_folder");
    url.addQueryItem("root",m_root);
    url.addQueryItem("path",path);

    url.addQueryItem("oauth_consumer_key",m_oauth_consumer_key);
    url.addQueryItem("oauth_nonce", nonce);
    url.addQueryItem("oauth_signature",m_access_oauth_signature);
    url.addQueryItem("oauth_signature_method",m_oauth_signature_method);
    url.addQueryItem("oauth_timestamp", QString::number(timestamp));
    url.addQueryItem("oauth_version",m_oauth_version);
    url.addQueryItem("oauth_token",m_oauthToken);

    KIO::TransferJob* const job = KIO::http_post(url,"",KIO::HideProgressInfo);
    job->addMetaData("content-type","Content-Type : application/x-www-form-urlencoded");

    connect(job,SIGNAL(data(KIO::Job*,QByteArray)),
            this,SLOT(data(KIO::Job*,QByteArray)));

    connect(job,SIGNAL(result(KJob*)),
            this,SLOT(slotResult(KJob*)));

    m_state = DB_CREATEFOLDER;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy(true);
}

/** Get username of dropbox user
 */
void DBTalker::getUserName()
{
    QUrl url("https://api.dropbox.com/1/account/info");
    url.addQueryItem("oauth_consumer_key",m_oauth_consumer_key);
    url.addQueryItem("oauth_nonce", nonce);
    url.addQueryItem("oauth_signature",m_access_oauth_signature);
    url.addQueryItem("oauth_signature_method",m_oauth_signature_method);
    url.addQueryItem("oauth_timestamp", QString::number(timestamp));
    url.addQueryItem("oauth_version",m_oauth_version);
    url.addQueryItem("oauth_token",m_oauthToken);

    KIO::TransferJob* const job = KIO::http_post(url,"",KIO::HideProgressInfo);
    job->addMetaData("content-type","Content-Type : application/x-www-form-urlencoded");

    connect(job,SIGNAL(data(KIO::Job*,QByteArray)),
            this,SLOT(data(KIO::Job*,QByteArray)));

    connect(job,SIGNAL(result(KJob*)),
            this,SLOT(slotResult(KJob*)));

    m_state = DB_USERNAME;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy(true);
}

/** Get list of folders by parsing json sent by dropbox
 */
void DBTalker::listFolders(const QString& path)
{
    QString make_url = QString("https://api.dropbox.com/1/metadata/dropbox/") + path;
    KUrl url(make_url);
    url.addQueryItem("oauth_consumer_key",m_oauth_consumer_key);
    url.addQueryItem("oauth_nonce", nonce);
    url.addQueryItem("oauth_signature",m_access_oauth_signature);
    url.addQueryItem("oauth_signature_method",m_oauth_signature_method);
    url.addQueryItem("oauth_timestamp", QString::number(timestamp));
    url.addQueryItem("oauth_version",m_oauth_version);
    url.addQueryItem("oauth_token",m_oauthToken);

    KIO::TransferJob* const job = KIO::get(url,KIO::NoReload,KIO::HideProgressInfo);
    job->addMetaData("content-type","Content-Type : application/x-www-form-urlencoded");

    connect(job,SIGNAL(data(KIO::Job*,QByteArray)),
            this,SLOT(data(KIO::Job*,QByteArray)));

    connect(job,SIGNAL(result(KJob*)),
            this,SLOT(slotResult(KJob*)));

    m_state = DB_LISTFOLDERS;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy(true);
}

bool DBTalker::addPhoto(const QString& imgPath, const QString& uploadFolder, bool rescale, int maxDim, int imageQuality)
{
    if(m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    emit signalBusy(true);
    MPForm form;
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

    if(rescale && (image.width() > maxDim || image.height() > maxDim))
    {
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

    QString uploadPath = uploadFolder + KUrl(imgPath).fileName();
    QString m_url = QString("https://api-content.dropbox.com/1/files_put/dropbox/") + "/" +uploadPath;
    KUrl url(m_url);
    url.addQueryItem("oauth_consumer_key",m_oauth_consumer_key);
    url.addQueryItem("oauth_nonce", nonce);
    url.addQueryItem("oauth_signature",m_access_oauth_signature);
    url.addQueryItem("oauth_signature_method",m_oauth_signature_method);
    url.addQueryItem("oauth_timestamp", QString::number(timestamp));
    url.addQueryItem("oauth_version",m_oauth_version);
    url.addQueryItem("oauth_token",m_oauthToken);
    url.addQueryItem("overwrite","false");

    KIO::TransferJob* const job = KIO::http_post(url,form.formData(),KIO::HideProgressInfo);
    job->addMetaData("content-type","Content-Type : application/x-www-form-urlencoded");

    connect(job,SIGNAL(data(KIO::Job*,QByteArray)),
            this,SLOT(data(KIO::Job*,QByteArray)));

    connect(job,SIGNAL(result(KJob*)),
            this,SLOT(slotResult(KJob*)));

    m_state = DB_ADDPHOTO;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy(true);
    return true;
}

void DBTalker::cancel()
{
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    emit signalBusy(false);
}

void DBTalker::data(KIO::Job*,const QByteArray& data)
{
    if(data.isEmpty())
    {
        return;
    }

    int oldsize = m_buffer.size();
    m_buffer.resize(m_buffer.size() + data.size());
    memcpy(m_buffer.data()+oldsize,data.data(),data.size());
}

void DBTalker::slotResult(KJob* kjob)
{
    m_job = 0;
    KIO::Job* const job = static_cast<KIO::Job*>(kjob);

    if(job->error())
    {
        if(m_state ==  DB_REQ_TOKEN)
        {
            emit signalBusy(false);
            emit signalRequestTokenFailed(job->error(),job->errorText());
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
        case (DB_REQ_TOKEN):
            kDebug() << "In DB_REQ_TOKEN";
            parseResponseRequestToken(m_buffer);
            break;
        case (DB_ACCESSTOKEN):
            kDebug() << "In DB_ACCESSTOKEN" << m_buffer;
            parseResponseAccessToken(m_buffer);
            break;
        case (DB_LISTFOLDERS):
            kDebug() << "In DB_LISTFOLDERS";
            parseResponseListFolders(m_buffer);
            break;
        case (DB_CREATEFOLDER):
            kDebug() << "In DB_CREATEFOLDER";
            parseResponseCreateFolder(m_buffer);
            break;
        case (DB_ADDPHOTO):
            kDebug() << "In DB_ADDPHOTO";// << m_buffer;
            parseResponseAddPhoto(m_buffer);
            break;
        case (DB_USERNAME):
            kDebug() << "In DB_USERNAME";// << m_buffer;
            parseResponseUserName(m_buffer);
            break;
        default:
            break;
    }
}

void DBTalker::parseResponseAddPhoto(const QByteArray& data)
{
    bool success        = false;
    QJson::Parser parser;
    bool ok;
    QVariant result     = parser.parse(data, &ok);
    QVariantMap rmap    = result.toMap();
    QList<QString> keys = rmap.uniqueKeys();

    for(int i=0;i<rmap.size();i++)
    {
        if(keys[i] == "bytes")
        {
            success = true;
            break;
        }
    }

    emit signalBusy(false);

    if(!success)
    {
        emit signalAddPhotoFailed(i18n("Failed to upload photo"));
    }
    else
    {
        emit signalAddPhotoSucceeded();
    }
}

void DBTalker::parseResponseRequestToken(const QByteArray& data)
{
    QString temp(data);
    QStringList split           = temp.split("&");
    QStringList tokenSecretList = split.at(0).split("=");
    m_oauthTokenSecret          = tokenSecretList.at(1);
    QStringList tokenList       = split.at(1).split("=");
    m_oauthToken                = tokenList.at(1);
    m_access_oauth_signature    = m_oauth_signature + m_oauthTokenSecret;
    doOAuth();
}

void DBTalker::parseResponseAccessToken(const QByteArray& data)
{
    QString temp(data);

    if(temp.contains("error"))
    {
        //doOAuth();
        emit signalBusy(false);
        emit signalAccessTokenFailed();
        return;
    }

    QStringList split           = temp.split("&");
    QStringList tokenSecretList = split.at(0).split("=");
    m_oauthTokenSecret          = tokenSecretList.at(1);
    QStringList tokenList       = split.at(1).split("=");
    m_oauthToken                = tokenList.at(1);
    m_access_oauth_signature    = m_oauth_signature + m_oauthTokenSecret;

    emit signalBusy(false);
    emit signalAccessTokenObtained(m_oauthToken,m_oauthTokenSecret,m_access_oauth_signature);
}

void DBTalker::parseResponseUserName(const QByteArray& data)
{
    QJson::Parser parser;
    bool ok;
    QVariant result     = parser.parse(data,&ok);
    QVariantMap rmap    = result.toMap();
    QList<QString> keys = rmap.uniqueKeys();
    QString temp;

    for(int i=0;i<rmap.size();i++)
    {
        if(keys[i] == "display_name")
        {
            temp = rmap[keys[i]].value<QString>();
        }
    }

    emit signalBusy(false);
    emit signalSetUserName(temp);
}

void DBTalker::parseResponseListFolders(const QByteArray& data)
{
    //added root in list at constructor and call getfolderslist after calling list folders in dbwindow
    QJson::Parser parser;
    bool ok;
    QVariant result = parser.parse(data,&ok);

    if(!ok)
    {
        emit signalBusy(false);
        emit signalListAlbumsFailed(i18n("Failed to list folders"));
        return;
    }

    QList<QPair<QString, QString> > list;
    list.clear();
    list.append(qMakePair(QString("/"),QString("root")));

    QVariantMap rmap = result.toMap();
    QList<QString> a = rmap.uniqueKeys();

    for(int i=0;i<rmap.size();i++)
    {
        if(a[i] == "contents")
        {
            QVariantList qwe = rmap[a[i]].toList();

            foreach(QVariant abc, qwe)
            {
                QVariantMap qwer = abc.toMap();
                QList<QString> b = qwer.uniqueKeys();
                QString path("");
                QString isDir("");
                int temp = 0;

                for(int i=0;i<qwer.size();i++)
                {
                    if(b[i] == "is_dir")
                        isDir = qwer[b[i]].value<QString>();
                    if(b[i] == "path")
                    {
                        path = qwer[b[i]].value<QString>();
                        temp = i;
                    }
                }
                if(QString::compare(isDir, QString("true"), Qt::CaseInsensitive) == 0)
                {
                    kDebug() << temp << " " << b[temp] << " : " << qwer[b[temp]] << " " << qwer[b[temp]].value<QString>() << endl;
                    QString name = qwer[b[temp]].value<QString>().section('/',-2);
                    kDebug() << "str " << name;
                    list.append(qMakePair(qwer[b[temp]].value<QString>(),name));
                    queue.enqueue(qwer[b[temp]].value<QString>());
                }
            }
        }
    }

    auth = true;
    emit signalBusy(false);
    emit signalListAlbumsDone(list);
}

void DBTalker::parseResponseCreateFolder(const QByteArray& data)
{
    bool success        = true;
    QJson::Parser parser;
    bool ok;
    QVariant result     = parser.parse(data,&ok);
    QVariantMap rmap    = result.toMap();
    QList<QString> keys = rmap.uniqueKeys();
    QString temp;

    for(int i=0;i<rmap.size();i++)
    {
        if(keys[i] == "error")
        {
            temp    = rmap[keys[i]].value<QString>();
            success = false;
            signalCreateFolderFailed(rmap[keys[i]].value<QString>());
        }
    }

    emit signalBusy(false);

    if(success)
    {
        emit signalCreateFolderSucceeded();
    }
}

} // namespace KIPIDropboxPlugin
