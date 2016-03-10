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

#include <dbtalker.h>

// C++ includes

#include <ctime>

// Qt includes

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
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
#include <QApplication>
#include <QDesktopServices>
#include <QPushButton>
#include <QUrlQuery>

// KDE includes

#include <kio/job.h>
#include <kio/jobuidelegate.h>
#include <kjobwidgets.h>

// Libkipi includes

#include <KIPI/PluginLoader>

// Local includes

#include "kipiplugins_debug.h"
#include "kpversion.h"
#include "kputil.h"
#include "dbwindow.h"
#include "dbitem.h"
#include "mpform.h"

namespace KIPIDropboxPlugin
{

DBTalker::DBTalker(QWidget* const parent)
{
    m_parent                 = parent;
    m_oauth_consumer_key     = QString::fromLatin1("kn7kajkaqf6retw");
    m_oauth_signature_method = QString::fromLatin1("PLAINTEXT");
    m_oauth_version          = QString::fromLatin1("1.0");
    m_oauth_signature        = QString::fromLatin1("t9w4c6j837ubstf&");
    m_nonce                  = generateNonce(8);
    m_timestamp              = QDateTime::currentMSecsSinceEpoch()/1000;
    m_root                   = QString::fromLatin1("dropbox");
    m_job                    = 0;
    m_state                  = DB_REQ_TOKEN;
    m_auth                   = false;
    m_dialog                 = 0;
    m_meta                   = 0;
    m_iface                  = 0;

    PluginLoader* const pl = PluginLoader::instance();

    if (pl)
    {
        m_iface = pl->interface();

        if (m_iface)
            m_meta = m_iface->createMetadataProcessor();
    }
}

DBTalker::~DBTalker()
{
}

/** generate a random number
 */
QString DBTalker::generateNonce(qint32 length)
{
    QString clng = QString::fromLatin1("");

    for(int i = 0; i < length; ++i)
    {
        clng += QString::number(int( qrand() / (RAND_MAX + 1.0) * (16 + 1 - 0) + 0 ), 16).toUpper();
    }

    return clng;
}

/** dropbox first has to obtain request token before asking user for authorization
 */
void DBTalker::obtain_req_token()
{
    QUrl url(QString::fromLatin1("https://api.dropbox.com/1/oauth/request_token"));
    QUrlQuery q(url);
    q.addQueryItem(QString::fromLatin1("oauth_consumer_key"), m_oauth_consumer_key);
    q.addQueryItem(QString::fromLatin1("oauth_nonce"), m_nonce);
    q.addQueryItem(QString::fromLatin1("oauth_signature"), m_oauth_signature);
    q.addQueryItem(QString::fromLatin1("oauth_signature_method"), m_oauth_signature_method);
    q.addQueryItem(QString::fromLatin1("oauth_timestamp"), QString::number(m_timestamp));
    q.addQueryItem(QString::fromLatin1("oauth_version"), m_oauth_version);
    url.setQuery(q);

    KIO::TransferJob* const job = KIO::http_post(url,"",KIO::HideProgressInfo);
    job->addMetaData(QString::fromLatin1("content-type"),
                     QString::fromLatin1("Content-Type : application/x-www-form-urlencoded"));

    connect(job,SIGNAL(data(KIO::Job*,QByteArray)),
            this,SLOT(data(KIO::Job*,QByteArray)));

    connect(job,SIGNAL(result(KJob*)),
            this,SLOT(slotResult(KJob*)));

    m_auth  = false;
    m_state = DB_REQ_TOKEN;
    m_job   = job;
    m_buffer.resize(0);
    emit signalBusy(true);
}

bool DBTalker::authenticated()
{
    if (m_auth)
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
    QUrl url(QString::fromLatin1("https://api.dropbox.com/1/oauth/authorize"));
    qCDebug(KIPIPLUGINS_LOG) << "in doOAuth()" << m_oauthToken;
    QUrlQuery q(url);
    q.addQueryItem(QString::fromLatin1("oauth_token"), m_oauthToken);
    url.setQuery(q);

    qCDebug(KIPIPLUGINS_LOG) << "OAuth URL: " << url;
    QDesktopServices::openUrl(url);

    emit signalBusy(false);

    m_dialog = new QDialog(QApplication::activeWindow(), 0);
    m_dialog->setModal(true);
    m_dialog->setWindowTitle(i18n("Authorize Dropbox"));
    QDialogButtonBox* const buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, m_dialog);
    buttons->button(QDialogButtonBox::Ok)->setDefault(true);

    m_dialog->connect(buttons, SIGNAL(accepted()), 
                      this, SLOT(slotAccept()));

    m_dialog->connect(buttons, SIGNAL(rejected()), 
                      this, SLOT(slotReject()));    

    QPlainTextEdit* const infobox = new QPlainTextEdit(i18n("Please follow the instructions in the browser. "
                                                            "After logging in and authorizing the application, press OK."));
    infobox->setReadOnly(true);

    QVBoxLayout* const vbx = new QVBoxLayout(m_dialog);
    vbx->addWidget(infobox);
    vbx->addWidget(buttons);
    m_dialog->setLayout(vbx);

    m_dialog->exec();

    if (m_dialog->result() == QDialog::Accepted)
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
    QUrl url(QString::fromLatin1("https://api.dropbox.com/1/oauth/access_token"));
    QUrlQuery q(url);
    q.addQueryItem(QString::fromLatin1("oauth_consumer_key"), m_oauth_consumer_key);
    q.addQueryItem(QString::fromLatin1("oauth_nonce"), m_nonce);
    q.addQueryItem(QString::fromLatin1("oauth_signature"), m_access_oauth_signature);
    q.addQueryItem(QString::fromLatin1("oauth_signature_method"), m_oauth_signature_method);
    q.addQueryItem(QString::fromLatin1("oauth_timestamp"), QString::number(m_timestamp));
    q.addQueryItem(QString::fromLatin1("oauth_version"), m_oauth_version);
    q.addQueryItem(QString::fromLatin1("oauth_token"), m_oauthToken);
    url.setQuery(q);

    KIO::TransferJob* const job = KIO::http_post(url, "", KIO::HideProgressInfo);
    job->addMetaData(QString::fromLatin1("content-type"),
                     QString::fromLatin1("Content-Type : application/x-www-form-urlencoded"));

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
    qCDebug(KIPIPLUGINS_LOG) << "in cre fol " << path;

    QUrl url(QString::fromLatin1("https://api.dropbox.com/1/fileops/create_folder"));
    QUrlQuery q(url);
    q.addQueryItem(QString::fromLatin1("root"),m_root);
    q.addQueryItem(QString::fromLatin1("path"),path);
    q.addQueryItem(QString::fromLatin1("oauth_consumer_key"), m_oauth_consumer_key);
    q.addQueryItem(QString::fromLatin1("oauth_nonce"), m_nonce);
    q.addQueryItem(QString::fromLatin1("oauth_signature"), m_access_oauth_signature);
    q.addQueryItem(QString::fromLatin1("oauth_signature_method"), m_oauth_signature_method);
    q.addQueryItem(QString::fromLatin1("oauth_timestamp"), QString::number(m_timestamp));
    q.addQueryItem(QString::fromLatin1("oauth_version"), m_oauth_version);
    q.addQueryItem(QString::fromLatin1("oauth_token"), m_oauthToken);
    url.setQuery(q);

    KIO::TransferJob* const job = KIO::http_post(url,"",KIO::HideProgressInfo);
    job->addMetaData(QString::fromLatin1("content-type"),
                     QString::fromLatin1("Content-Type : application/x-www-form-urlencoded"));

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
    QUrl url(QString::fromLatin1("https://api.dropbox.com/1/account/info"));
    QUrlQuery q(url);
    q.addQueryItem(QString::fromLatin1("oauth_consumer_key"), m_oauth_consumer_key);
    q.addQueryItem(QString::fromLatin1("oauth_nonce"), m_nonce);
    q.addQueryItem(QString::fromLatin1("oauth_signature"), m_access_oauth_signature);
    q.addQueryItem(QString::fromLatin1("oauth_signature_method"), m_oauth_signature_method);
    q.addQueryItem(QString::fromLatin1("oauth_timestamp"), QString::number(m_timestamp));
    q.addQueryItem(QString::fromLatin1("oauth_version"), m_oauth_version);
    q.addQueryItem(QString::fromLatin1("oauth_token"), m_oauthToken);
    url.setQuery(q);

    KIO::TransferJob* const job = KIO::http_post(url,"",KIO::HideProgressInfo);
    job->addMetaData(QString::fromLatin1("content-type"),
                     QString::fromLatin1("Content-Type : application/x-www-form-urlencoded"));

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
    QString make_url = QString::fromLatin1("https://api.dropbox.com/1/metadata/dropbox/") + path;
    QUrl url(make_url);
    QUrlQuery q(url);
    q.addQueryItem(QString::fromLatin1("oauth_consumer_key"), m_oauth_consumer_key);
    q.addQueryItem(QString::fromLatin1("oauth_nonce"), m_nonce);
    q.addQueryItem(QString::fromLatin1("oauth_signature"), m_access_oauth_signature);
    q.addQueryItem(QString::fromLatin1("oauth_signature_method"), m_oauth_signature_method);
    q.addQueryItem(QString::fromLatin1("oauth_timestamp"), QString::number(m_timestamp));
    q.addQueryItem(QString::fromLatin1("oauth_version"), m_oauth_version);
    q.addQueryItem(QString::fromLatin1("oauth_token"), m_oauthToken);
    url.setQuery(q);

    KIO::TransferJob* const job = KIO::get(url,KIO::NoReload,KIO::HideProgressInfo);
    job->addMetaData(QString::fromLatin1("content-type"),
                     QString::fromLatin1("Content-Type : application/x-www-form-urlencoded"));

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
    if (m_job)
    {
        m_job->kill();
        m_job = 0;
    }

    emit signalBusy(true);
    MPForm form;
    QImage image;

    if (m_iface)
    {
        image = m_iface->preview(QUrl::fromLocalFile(imgPath));
    }

    if (image.isNull())
    {
        return false;
    }

    QDir tempDir = makeTemporaryDir("kipi-dropbox");
    QString path = tempDir.filePath(QFileInfo(imgPath).baseName().trimmed() + QString::fromLatin1(".jpg"));

    if (rescale && (image.width() > maxDim || image.height() > maxDim))
    {
        image = image.scaled(maxDim,maxDim,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    }

    image.save(path,"JPEG",imageQuality);

    if (m_meta->load(QUrl::fromLocalFile(imgPath)))
    {
        m_meta->setImageDimensions(image.size());
        m_meta->setImageProgramId(QString::fromLatin1("Kipi-plugins"), kipipluginsVersion());
        m_meta->save(QUrl::fromLocalFile(path));
    }

    if (!form.addFile(path))
    {
        emit signalBusy(false);
        return false;
    }

    QString uploadPath = uploadFolder + QUrl(imgPath).fileName();
    QString m_url      = QString::fromLatin1("https://api-content.dropbox.com/1/files_put/dropbox/") + QString::fromLatin1("/") + uploadPath;
    QUrl url(m_url);
    QUrlQuery q(url);
    q.addQueryItem(QString::fromLatin1("oauth_consumer_key"), m_oauth_consumer_key);
    q.addQueryItem(QString::fromLatin1("oauth_nonce"), m_nonce);
    q.addQueryItem(QString::fromLatin1("oauth_signature"), m_access_oauth_signature);
    q.addQueryItem(QString::fromLatin1("oauth_signature_method"), m_oauth_signature_method);
    q.addQueryItem(QString::fromLatin1("oauth_timestamp"), QString::number(m_timestamp));
    q.addQueryItem(QString::fromLatin1("oauth_version"), m_oauth_version);
    q.addQueryItem(QString::fromLatin1("oauth_token"), m_oauthToken);
    q.addQueryItem(QString::fromLatin1("overwrite"), QString::fromLatin1("false"));
    url.setQuery(q);

    KIO::TransferJob* const job = KIO::http_post(url,form.formData(),KIO::HideProgressInfo);
    job->addMetaData(QString::fromLatin1("content-type"),
                     QString::fromLatin1("Content-Type : application/x-www-form-urlencoded"));

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
    m_job               = 0;
    KIO::Job* const job = static_cast<KIO::Job*>(kjob);

    if (job->error())
    {
        if (m_state ==  DB_REQ_TOKEN)
        {
            emit signalBusy(false);
            emit signalRequestTokenFailed(job->error(),job->errorText());
        }
        else
        {
            emit signalBusy(false);
            KJobWidgets::setWindow(job, m_parent);
            job->ui()->showErrorMessage();
        }
        return;
    }

    switch(m_state)
    {
        case (DB_REQ_TOKEN):
            qCDebug(KIPIPLUGINS_LOG) << "In DB_REQ_TOKEN";
            parseResponseRequestToken(m_buffer);
            break;
        case (DB_ACCESSTOKEN):
            qCDebug(KIPIPLUGINS_LOG) << "In DB_ACCESSTOKEN" << m_buffer;
            parseResponseAccessToken(m_buffer);
            break;
        case (DB_LISTFOLDERS):
            qCDebug(KIPIPLUGINS_LOG) << "In DB_LISTFOLDERS";
            parseResponseListFolders(m_buffer);
            break;
        case (DB_CREATEFOLDER):
            qCDebug(KIPIPLUGINS_LOG) << "In DB_CREATEFOLDER";
            parseResponseCreateFolder(m_buffer);
            break;
        case (DB_ADDPHOTO):
            qCDebug(KIPIPLUGINS_LOG) << "In DB_ADDPHOTO";// << m_buffer;
            parseResponseAddPhoto(m_buffer);
            break;
        case (DB_USERNAME):
            qCDebug(KIPIPLUGINS_LOG) << "In DB_USERNAME";// << m_buffer;
            parseResponseUserName(m_buffer);
            break;
        default:
            break;
    }
}

void DBTalker::parseResponseAddPhoto(const QByteArray& data)
{
    QJsonDocument doc      = QJsonDocument::fromJson(data);
    QJsonObject jsonObject = doc.object();
    bool success           = jsonObject.contains(QString::fromLatin1("bytes"));
    emit signalBusy(false);

    if (!success)
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
    QString temp                = QString::fromUtf8(data);
    QStringList split           = temp.split(QString::fromLatin1("&"));
    QStringList tokenSecretList = split.at(0).split(QString::fromLatin1("="));
    m_oauthTokenSecret          = tokenSecretList.at(1);
    QStringList tokenList       = split.at(1).split(QString::fromLatin1("="));
    m_oauthToken                = tokenList.at(1);
    m_access_oauth_signature    = m_oauth_signature + m_oauthTokenSecret;
    doOAuth();
}

void DBTalker::parseResponseAccessToken(const QByteArray& data)
{
    QString temp = QString::fromUtf8(data);

    if (temp.contains(QString::fromLatin1("error")))
    {
        //doOAuth();
        emit signalBusy(false);
        emit signalAccessTokenFailed();
        return;
    }

    QStringList split           = temp.split(QString::fromLatin1("&"));
    QStringList tokenSecretList = split.at(0).split(QString::fromLatin1("="));
    m_oauthTokenSecret          = tokenSecretList.at(1);
    QStringList tokenList       = split.at(1).split(QString::fromLatin1("="));
    m_oauthToken                = tokenList.at(1);
    m_access_oauth_signature    = m_oauth_signature + m_oauthTokenSecret;

    emit signalBusy(false);
    emit signalAccessTokenObtained(m_oauthToken,m_oauthTokenSecret,m_access_oauth_signature);
}

void DBTalker::parseResponseUserName(const QByteArray& data)
{
    QJsonDocument doc      = QJsonDocument::fromJson(data);
    QJsonObject jsonObject = doc.object();
    QString temp           = jsonObject[QString::fromLatin1("display_name")].toString();

    emit signalBusy(false);
    emit signalSetUserName(temp);
}

void DBTalker::parseResponseListFolders(const QByteArray& data)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);

    if (err.error != QJsonParseError::NoError)
    {
        emit signalBusy(false);
        emit signalListAlbumsFailed(i18n("Failed to list folders"));
        return;
    }

    QJsonObject jsonObject = doc.object();
    QJsonArray jsonArray   = jsonObject[QString::fromLatin1("contents")].toArray();

    QList<QPair<QString, QString> > list;
    list.clear();
    list.append(qMakePair(QString::fromLatin1("/"), QString::fromLatin1("root")));

    foreach (const QJsonValue& value, jsonArray) 
    {
        QString path(QString::fromLatin1(""));
        bool isDir;

        QJsonObject obj = value.toObject();
        path            = obj[QString::fromLatin1("path")].toString();
        isDir           = obj[QString::fromLatin1("is_dir")].toBool();
        qCDebug(KIPIPLUGINS_LOG) << "Path is "<<path<<" Is Dir "<<isDir;

        if(isDir)
        {
            qCDebug(KIPIPLUGINS_LOG) << "Path is "<<path<<" Is Dir "<<isDir;
            QString name = path.section(QLatin1Char('/'), -2);
            qCDebug(KIPIPLUGINS_LOG) << "str " << name;
            list.append(qMakePair(path,name));
            m_queue.enqueue(path);
        }
    }

    m_auth = true;
    emit signalBusy(false);
    emit signalListAlbumsDone(list);
}

void DBTalker::parseResponseCreateFolder(const QByteArray& data)
{
    QJsonDocument doc      = QJsonDocument::fromJson(data);
    QJsonObject jsonObject = doc.object();
    bool fail              = jsonObject.contains(QString::fromLatin1("error"));
    QString temp;

    emit signalBusy(false);

    if(fail)
    {
        emit signalCreateFolderFailed(jsonObject[QString::fromLatin1("error")].toString());
    }
    else
    {
        emit signalCreateFolderSucceeded();   
    }
}

void DBTalker::slotAccept()
{
    m_dialog->close();
    m_dialog->setResult(QDialog::Accepted); 
}

void DBTalker::slotReject()
{
    m_dialog->close();   
    m_dialog->setResult(QDialog::Rejected);
}

} // namespace KIPIDropboxPlugin
