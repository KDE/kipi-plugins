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
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrlQuery>

// KDE includes

#include <klocalizedstring.h>
#include <kconfiggroup.h>

// Local includes

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
    m_reply           = 0;
    m_continuePos     = 0;
    m_Authstate       = GD_ACCESSTOKEN;
    m_window          = 0;

    m_netMngr         = new QNetworkAccessManager(this);

    connect(m_netMngr, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(slotAuthFinished(QNetworkReply*)));
}

Authorize::~Authorize()
{
    if (m_reply)
        m_reply->abort();
}

bool Authorize::authenticated()
{
    if (m_access_token.isEmpty())
    {
        return false;
    }

    return true;
}

/**
 * Starts authentication by opening the browser
 */
void Authorize::doOAuth()
{
    QUrl url(QString::fromLatin1("https://accounts.google.com/o/oauth2/auth"));
    QUrlQuery urlQuery;
    urlQuery.addQueryItem(QString::fromLatin1("scope"),         m_scope);
    urlQuery.addQueryItem(QString::fromLatin1("redirect_uri"),  m_redirect_uri);
    urlQuery.addQueryItem(QString::fromLatin1("response_type"), m_response_type);
    urlQuery.addQueryItem(QString::fromLatin1("client_id"),     m_client_id);
    urlQuery.addQueryItem(QString::fromLatin1("access_type"),   QString::fromLatin1("offline"));
    url.setQuery(urlQuery);
    qCDebug(KIPIPLUGINS_LOG) << "OAuth URL: " << url;
    QDesktopServices::openUrl(url);

    emit signalBusy(false);

    m_window = new QDialog(QApplication::activeWindow(),0);
    m_window->setModal(true);
    m_window->setWindowTitle(i18n("Google Drive Authorization"));

    QDialogButtonBox* const buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
    QPushButton* const okButton       = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);

    m_window->connect(buttonBox, SIGNAL(accepted()),
                      this, SLOT(slotAccept()));

    m_window->connect(buttonBox, SIGNAL(rejected()),
                      this, SLOT(slotReject()));

    QLineEdit* const textbox      = new QLineEdit();
    QPlainTextEdit* const infobox = new QPlainTextEdit(i18n("Please follow the instructions in the browser. "
                                                            "After logging in and authorizing the application, "
                                                            "copy the code from the browser, paste it in the "
                                                            "textbox below, and click OK."));
    QVBoxLayout* const layout = new QVBoxLayout;
    m_window->setLayout(layout);
    infobox->setReadOnly(true);
    layout->addWidget(infobox);
    layout->addWidget(textbox);
    layout->addWidget(buttonBox);

    m_window->exec();

    if (m_window->result() == QDialog::Accepted && !(textbox->text().isEmpty()))
    {
        qCDebug(KIPIPLUGINS_LOG) << "1";
        m_code = textbox->text();
    }

    if (textbox->text().isEmpty())
    {
        qCDebug(KIPIPLUGINS_LOG) << "3";
        emit signalTextBoxEmpty();
    }

    if (m_code != QString::fromLatin1("0"))
    {
        getAccessToken();
    }
}

void Authorize::slotAccept()
{
    m_window->close();
    m_window->setResult(QDialog::Accepted);
}

void Authorize::slotReject()
{
    m_window->close();
    m_window->setResult(QDialog::Rejected);
}

/**
 * Gets access token from googledrive after authentication by user
 */
void Authorize::getAccessToken()
{
    QUrl url(QString::fromLatin1("https://accounts.google.com/o/oauth2/token?"));
    QUrlQuery urlQuery;
    urlQuery.addQueryItem(QString::fromLatin1("scope"),         m_scope);
    urlQuery.addQueryItem(QString::fromLatin1("response_type"), m_response_type);
    urlQuery.addQueryItem(QString::fromLatin1("token_uri"),     m_token_uri);
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

    QNetworkRequest netRequest(url);
    netRequest.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/x-www-form-urlencoded"));

    m_reply = m_netMngr->post(netRequest, postData);

    m_Authstate = GD_ACCESSTOKEN;
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

    QNetworkRequest netRequest(url);
    netRequest.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/x-www-form-urlencoded"));

    m_reply = m_netMngr->post(netRequest, postData);

    m_Authstate = GD_REFRESHTOKEN;
    m_buffer.resize(0);
    emit signalBusy(true);
}

void Authorize::slotAuthFinished(QNetworkReply* reply)
{
    if (reply != m_reply)
    {
        return;
    }

    m_reply = 0;

    if (reply->error() != QNetworkReply::NoError)
    {
        if (m_Authstate == GD_ACCESSTOKEN)
        {
            emit signalBusy(false);
            emit signalAccessTokenFailed(m_reply->error(), reply->errorString());
        }
        else
        {
            emit signalBusy(false);
            QMessageBox::critical(QApplication::activeWindow(),
                                  i18n("Error"), reply->errorString());
        }

        reply->deleteLater();
        return;
    }

    m_buffer.append(reply->readAll());

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

    reply->deleteLater();
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
    if (pathValues.count() == 0)
        return QStringList();

    QString token(getToken(jsonStr, pathValues[0], QString::fromLatin1("]")));

    for (int i = 1; i < pathValues.count(); ++i)
    {
        token = getToken(token, pathValues[i], QString::fromLatin1("]"));
    }

    QStringList tokens;
    QString nextToken;

    m_continuePos = 0;

    while (!(nextToken = getValue(token, key)).isEmpty())
    {
        token = token.mid(m_continuePos);
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

    if (endPos != -1)
        m_continuePos = endPos;
    else
        m_continuePos = beginPos + token.length();

    return token;
}

int Authorize::getTokenEnd(const QString& object, int beginPos)
{
    int beginDividerPos(object.indexOf(QString::fromLatin1("["), beginPos));
    int endDividerPos(object.indexOf(QString::fromLatin1("]"),   beginPos + 1));

    while ((beginDividerPos < endDividerPos) && beginDividerPos != -1)
    {
        beginDividerPos = object.indexOf(QString::fromLatin1("["), endDividerPos);
        endDividerPos   = object.indexOf(QString::fromLatin1("]"), endDividerPos + 1);
    }

    return endDividerPos + 1;
}

} // namespace KIPIGoogleServicesPlugin
