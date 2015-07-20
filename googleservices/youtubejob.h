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

#ifndef YOUTUBEJOB_H
#define YOUTUBEJOB_H

#include <KPasswordDialog>
#include <KJob>
#include <QMap>
#include <QPointer>
#include <QString>
#include <QJsonValue>
#include <QNetworkAccessManager>
#include <QUrl>

namespace KIPIGoogleServicesPlugin
{

class YoutubeJob : public KJob
{
    Q_OBJECT
    public:
        YoutubeJob(const QUrl& url, const QByteArray &token, const QString& title, const QStringList& tags, const QString& description, QObject* parent = Q_NULLPTR);
        void start() override;

        QString outputUrl() const { return m_output; }

    private:
        void fileFetched(KJob*);
        void createLocation();
        void locationCreated();
        void uploadVideo(const QByteArray& data);
        void videoUploaded();

        QUrl m_url;
        QByteArray m_token;
        QString m_output;
        QNetworkAccessManager m_manager;
        QByteArray m_metadata;
        QUrl m_uploadUrl;
};

} // namespace KIPIGoogleServicesPlugin

#endif /* YOUTUBEJOB_H */
