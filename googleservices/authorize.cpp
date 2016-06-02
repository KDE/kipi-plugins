/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2015-06-21
 * Description : a kipi plugin to export images to Google-Drive web service
 *
 * Copyright (C) 2015 by Shourya Singh Gupta <shouryasgupta at gmail dot com>
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

#include <authorize.h>

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
#include <QDebug>
#include <QApplication>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QDesktopServices>
#include <QUrlQuery>

// KDE includes

#include <klocalizedstring.h>
#include <kio/jobuidelegate.h>
#include <kjobwidgets.h>
#include <kconfiggroup.h>

// local includes

#include "mpform_gdrive.h"
#include "kipiplugins_debug.h"

namespace KIPIGoogleServicesPlugin
{

Authorize::Authorize(QWidget* const parent, const QString & scope)
{
    m_parent          = parent;
    m_scope           = scope;
    m_redirect_uri    = QString::fromLatin1("urn:ietf:wg:oauth:2.0:oob");
    m_response_type   = QString::fromLatin1("code");
    m_client_id       = QString::fromLatin1("735222197981-mrcgtaqf05914buqjkts7mk79blsquas.apps.googleusercontent.com");
    m_token_uri       = QString::fromLatin1("https://accounts.google.com/o/oauth2/token");
    m_client_secret   = QString::fromLatin1("4MJOS0u1-_AUEKJ0ObA-j22U");
    m_code            = QString::fromLatin1("0");
    m_job             = 0;
    continuePos       = 0;
    m_Authstate       = GD_ACCESSTOKEN;
    window            = 0;
}

Authorize::~Authorize()
{
    if (m_job)
        m_job->kill();
}

bool Authorize::authenticated()
{
    if(m_access_token.isEmpty())
    {
        return false;
    }

    return true;
}

/** Starts authentication by opening the browser
 */
void Authorize::doOAuth()
{
    QUrl url(QString::fromLatin1("https://accounts.google.com/o/oauth2/auth"));
    QUrlQuery urlQuery;
    urlQuery.addQueryItem(QString::fromLatin1("scope"), m_scope);
    urlQuery.addQueryItem(QString::fromLatin1("redirect_uri"), m_redirect_uri);
    urlQuery.addQueryItem(QString::fromLatin1("response_type"), m_response_type);
    urlQuery.addQueryItem(QString::fromLatin1("client_id"), m_client_id);
    urlQuery.addQueryItem(QString::fromLatin1("access_type"), QString::fromLatin1("offline"));
    url.setQuery(urlQuery);
    qCDebug(KIPIPLUGINS_LOG) << "OAuth URL: " << url;
    QDesktopServices::openUrl(url);

    emit signalBusy(false);

    window = new QDialog(QApplication::activeWindow(),0);
    window->setModal(true);
    window->setWindowTitle(i18n("Google Drive Authorization"));
    
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    
    window->connect(buttonBox, SIGNAL(accepted()), 
                    this, SLOT(slotAccept()));
    
    window->connect(buttonBox, SIGNAL(rejected()), 
                    this, SLOT(slotReject()));
    
    QLineEdit* const textbox      = new QLineEdit();
    QPlainTextEdit* const infobox = new QPlainTextEdit(i18n("Please follow the instructions in the browser. "
                                                            "After logging in and authorizing the application, "
                                                            "copy the code from the browser, paste it in the "
                                                            "textbox below, and click OK."));
    QVBoxLayout *layout = new QVBoxLayout;
    window->setLayout(layout);
    infobox->setReadOnly(true);
    layout->addWidget(infobox);
    layout->addWidget(textbox);
    layout->addWidget(buttonBox);
    
    window->exec();
    
    if(window->result() == QDialog::Accepted && !(textbox->text().isEmpty()))
    {
        qCDebug(KIPIPLUGINS_LOG) << "1";
        m_code = textbox->text();
    }

    if(textbox->text().isEmpty())
    {
        qCDebug(KIPIPLUGINS_LOG) << "3";
        emit signalTextBoxEmpty();
    }

    if(m_code != QString::fromLatin1("0"))
    {
        getAccessToken();
    }

}

void Authorize::slotAccept()
{
    window->close();
    window->setResult(QDialog::Accepted); 
}

void Authorize::slotReject()
{
    window->close();   
    window->setResult(QDialog::Rejected);
}

/** Gets access token from googledrive after authentication by user
 */
void Authorize::getAccessToken()
{
    QUrl url(QString::fromLatin1("https://accounts.google.com/o/oauth2/token?"));
    QUrlQuery urlQuery;
    urlQuery.addQueryItem(QString::fromLatin1("scope"), m_scope);
    urlQuery.addQueryItem(QString::fromLatin1("response_type"), m_response_type);
    urlQuery.addQueryItem(QString::fromLatin1("token_uri"), m_token_uri);
    url.setQuery(urlQuery);
    QByteArray postData;
    postData = "code=";
    postData += m_code.toLatin1();
    postData += "&client_id=";
    postData += m_client_id.toLatin1();
    postData += "&client_secret=";
    postData += m_client_secret.toLatin1();
    postData += "&redirect_uri=";
    postData += m_redirect_uri.toLatin1();
    postData += "&grant_type=authorization_code";

    KIO::TransferJob* const job = KIO::http_post(url,postData,KIO::HideProgressInfo);
    job->addMetaData(QString::fromLatin1("content-type"),
                     QString::fromLatin1("Content-Type: application/x-www-form-urlencoded"));

    connect(job,SIGNAL(data(KIO::Job*,QByteArray)),
            this,SLOT(data(KIO::Job*,QByteArray)));

    connect(job,SIGNAL(result(KJob*)),
            this,SLOT(slotAuthResult(KJob*)));

    m_Authstate = GD_ACCESSTOKEN;
    m_job       = job;
    m_buffer.resize(0);
    emit signalBusy(true);
}

/** Gets access token from refresh token for handling login of user across digikam sessions
 */
void Authorize::getAccessTokenFromRefreshToken(const QString& msg)
{
    QUrl url(QString::fromLatin1("https://accounts.google.com/o/oauth2/token"));

    QByteArray postData;
    postData = "&client_id=";
    postData += m_client_id.toLatin1();
    postData += "&client_secret=";
    postData += m_client_secret.toLatin1();
    postData += "&refresh_token=";
    postData += msg.toLatin1();
    postData += "&grant_type=refresh_token";

    KIO::TransferJob* const job = KIO::http_post(url,postData,KIO::HideProgressInfo);
    job->addMetaData(QString::fromLatin1("content-type"),
                     QString::fromLatin1("Content-Type: application/x-www-form-urlencoded"));

    connect(job,SIGNAL(data(KIO::Job*,QByteArray)),
            this,SLOT(data(KIO::Job*,QByteArray)));

    connect(job,SIGNAL(result(KJob*)),
            this,SLOT(slotAuthResult(KJob*)));

    m_Authstate = GD_REFRESHTOKEN;
    m_job       = job;
    m_buffer.resize(0);
    emit signalBusy(true);
}

void Authorize::data(KIO::Job*,const QByteArray& data)
{
    if(data.isEmpty())
    {
        return;
    }

    int oldsize = m_buffer.size();
    m_buffer.resize(m_buffer.size() + data.size());
    memcpy(m_buffer.data()+oldsize,data.data(),data.size());
}

void Authorize::slotAuthResult(KJob* kjob)
{
    m_job = 0;
    KIO::Job* const job = static_cast<KIO::Job*>(kjob);

    if(job->error())
    {
        if(m_Authstate == GD_ACCESSTOKEN)
        {
            emit signalBusy(false);
            emit signalAccessTokenFailed(job->error(),job->errorText());
        }
        else
        {
            emit signalBusy(false);
            KIO::JobUiDelegate* const job_ui = static_cast<KIO::JobUiDelegate*>(job->ui());
            KJobWidgets::setWindow(job, m_parent);
            job_ui->showErrorMessage();
        }

        return;
    }

    switch(m_Authstate)
    {
        case (GD_ACCESSTOKEN):
            qCDebug(KIPIPLUGINS_LOG) << "In GD_ACCESSTOKEN";// << m_buffer;
            parseResponseAccessToken(m_buffer);
            break;
        case (GD_REFRESHTOKEN):
            qCDebug(KIPIPLUGINS_LOG) << "In GD_REFRESHTOKEN" << m_buffer;
            parseResponseRefreshToken(m_buffer);
            break;
        default:
            break;
    }
}

void Authorize::parseResponseAccessToken(const QByteArray& data)
{
    m_access_token  = getValue(QString::fromUtf8(data), QString::fromLatin1("access_token"));
    m_refresh_token = getValue(QString::fromUtf8(data), QString::fromLatin1("refresh_token"));

    if (getValue(QString::fromUtf8(data), QString::fromLatin1("error")) == QString::fromLatin1("invalid_request") ||
        getValue(QString::fromUtf8(data), QString::fromLatin1("error")) == QString::fromLatin1("invalid_grant"))
    {
        doOAuth();
        return;
    }

    m_bearer_access_token = QString::fromLatin1("Bearer ") + m_access_token;
    qCDebug(KIPIPLUGINS_LOG) << "In parse GD_ACCESSTOKEN" << m_bearer_access_token << "  " << data;
    //emit signalAccessTokenObtained();
    emit signalRefreshTokenObtained(m_refresh_token);
}

void Authorize::parseResponseRefreshToken(const QByteArray& data)
{
    m_access_token = getValue(QString::fromUtf8(data), QString::fromLatin1("access_token"));

    if (getValue(QString::fromUtf8(data), QString::fromLatin1("error")) == QString::fromLatin1("invalid_request") ||
        getValue(QString::fromUtf8(data), QString::fromLatin1("error")) == QString::fromLatin1("invalid_grant"))
    {
        doOAuth();
        return;
    }

    m_bearer_access_token = QString::fromLatin1("Bearer ") + m_access_token;
    qCDebug(KIPIPLUGINS_LOG) << "In parse GD_ACCESSTOKEN" << m_bearer_access_token << "  " << data;
    emit signalAccessTokenObtained();
}

QString Authorize::getValue(const QString& jsonStr, const QString& key)
{
    QString token(getToken(jsonStr, key, QString::fromLatin1(",")));

    token.remove(QRegExp(QString::fromLatin1("[\"}]")));

    QStringList tokenValues(token.split(QString::fromLatin1(": ")));
    QString value;

    if (tokenValues.count() == 2) 
        value = tokenValues[1].trimmed();

    return value;
}

QStringList Authorize::getParams(const QString& jsonStr, const QStringList& pathValues, const QString& key)
{
    if(pathValues.count() == 0)
        return QStringList();

    QString token(getToken(jsonStr, pathValues[0], QString::fromLatin1("]")));

    for(int i = 1; i < pathValues.count(); ++i)
    {
        token = getToken(token, pathValues[i], QString::fromLatin1("]"));
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

QString Authorize::getToken(const QString& object, const QString& key, const QString& endDivider)
{
    QString searchToken(QString::fromLatin1("\"") + key + QString::fromLatin1("\""));

    int beginPos(object.indexOf(searchToken));

    if (beginPos == -1)
        return QString();

    int endPos;

    if (endDivider == QString::fromLatin1(","))
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

int Authorize::getTokenEnd(const QString& object, int beginPos)
{
    int beginDividerPos(object.indexOf(QString::fromLatin1("["), beginPos ));
    int endDividerPos(object.indexOf(QString::fromLatin1("]"), beginPos + 1));

    while((beginDividerPos < endDividerPos) && beginDividerPos != -1)
    {
        beginDividerPos = object.indexOf(QString::fromLatin1("["), endDividerPos);
        endDividerPos = object.indexOf(QString::fromLatin1("]"), endDividerPos + 1);
    }

    return endDividerPos + 1;
}

} // namespace KIPIGoogleServicesPlugin
