/*
 Copyright 2014 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either 
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public 
 License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "youtubejobcomposite.h"
#include "youtubejob.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>
#include <QStandardPaths>
#include <KLocalizedString>
#include <KAccounts/getcredentialsjob.h>
#include <KAccounts/core.h>
#include <Accounts/Manager>
#include <Accounts/Application>

namespace KIPIGoogleServicesPlugin
{

QDebug operator<<(QDebug s, const Accounts::Service& service)
{
    s.nospace() << qPrintable(service.displayName()) << ',' << qPrintable(service.name()) << '\n';
    return s;
}
QDebug operator<<(QDebug s, const Accounts::Provider& provider)
{
    s.nospace() << "Provider(" << qPrintable(provider.displayName()) << ',' << qPrintable(provider.name()) << ")\n";
    return s;
}

YoutubeJobComposite::YoutubeJobComposite()
    : Job()
{
    auth = new Authorize(0,QString("https://gdata.youtube.com"));
    auth->doOAuth();
    connect(auth,SIGNAL(signalRefreshTokenObtained(const QString)),
            this,SLOT(slotAccessTokenReceived(const QString)));    
}

void YoutubeJobComposite::start()
{
    const QJsonValue jsonId = data().value(QStringLiteral("accountId"));
    if (jsonId.isNull() || jsonId.isUndefined()) {
        setError(1);
        setErrorText(i18n("No YouTube account configured in your accounts."));
        emitResult();
        return;
    }
    const Accounts::AccountId id = jsonId.toInt();

    //TODO: make async
    QByteArray accessToken;
    {
        auto job = new GetCredentialsJob(id, this);
        bool b = job->exec();
        if (!b) {
            qWarning() << "Couldn't fetch credentials";
            setError(job->error());
            setErrorText(job->errorText());
            emitResult();
            return;
        }
        accessToken = job->credentialsData()[QStringLiteral("AccessToken")].toByteArray();
    }

    m_pendingJobs = 0;
    const QJsonArray urls = data().value(QStringLiteral("urls")).toArray();
    foreach(const QJsonValue& url, urls) {
        YoutubeJob* job = new YoutubeJob(QUrl(url.toString()),
                                         accessToken,
                                         data().value(QStringLiteral("videoTitle")).toString(),
                                         data().value(QStringLiteral("videoTags")).toString().split(QLatin1Char(',')),
                                         data().value(QStringLiteral("videoDesc")).toString(), this);
        connect(job, &KJob::finished, this, &YoutubeJobComposite::subjobFinished);
        job->start();
        m_pendingJobs++;
    }
}

void YoutubeJobComposite::subjobFinished(KJob* subjob)
{
    m_pendingJobs--;
    if (subjob->error()) {
        setError(subjob->error());
        setErrorText(subjob->errorText());
        emitResult();
        return;
    }
    if (m_pendingJobs==0) {
        if (!error()) {
            const QJsonValue url = qobject_cast<YoutubeJob*>(subjob)->outputUrl();
            Q_EMIT output({{ QStringLiteral("url"), url.toString() }});
        }
        emitResult();
    }
}

void YoutubeJobComposite::slotAccessTokenReceived(const QString refresh_token)
{
    qDebug()<<"Access Token Obtained is : "<< auth->m_access_token << "Refresh Token Obtained is "<< refresh_token;
}

}