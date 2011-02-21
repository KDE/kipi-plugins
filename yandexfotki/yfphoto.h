/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-11-18
 * Description : Yandex.Fotki web service backend
 *
 * Copyright (C) 2010 by Roman Tsisyk <roman at tsisyk dot com>
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

#ifndef YANDEXFOTKI_PHOTO_H
#define YANDEXFOTKI_PHOTO_H

// Qt includes

#include <QString>
#include <QDateTime>
#include <QStringList>

namespace KIPIYandexFotkiPlugin
{

class YandexFotkiPhoto
{
public:

    enum Access
    {
        ACCESS_PUBLIC = 0,
        ACCESS_FRIENDS,
        ACCESS_PRIVATE ,
    };

    YandexFotkiPhoto()
        :   m_access(ACCESS_PUBLIC),
            m_hideOriginal(false),
            m_disableComments(false),
            m_adult(false)
    {

    }

    virtual ~YandexFotkiPhoto()
    {
        // nothing
    }

    const QString& urn() const
    {
        return m_urn;
    }

    const QString& author() const
    {
        return m_author;
    }

    const QString& title() const
    {
        return m_title;
    }

    void setTitle(const QString& title)
    {
        m_title = title;
    }

    const QString& summary() const
    {
        return m_summary;
    }

    void setSummary(const QString& summary)
    {
        m_summary = summary;
    }

    const QDateTime& publishedDate() const
    {
        return m_publishedDate;
    }

    const QDateTime& editedDate() const
    {
        return m_editedDate;
    }

    const QDateTime& updatedDate() const
    {
        return m_updatedDate;
    }

    const QDateTime& createdDate() const
    {
        return m_createdDate;
    }

    Access access() const
    {
        return m_access;
    }

    void setAccess(Access access)
    {
        m_access = access;
    }

    bool isHideOriginal() const
    {
        return m_hideOriginal;
    }

    void setHideOriginal(bool hideOriginal)
    {
        m_hideOriginal = hideOriginal;
    }

    bool isDisableComments() const
    {
        return m_disableComments;
    }

    void setDisableComments(bool disableComments)
    {
        m_disableComments = disableComments;
    }

    bool isAdult() const
    {
        return m_adult;
    }

    void setAdult(bool adult)
    {
        m_adult = adult;
    }

    QString localUrl() const
    {
        return m_localUrl;
    }

    void setLocalUrl(const QString& localUrl)
    {
        m_localUrl = localUrl;
    }

    QString originalUrl() const
    {
        return m_originalUrl;
    }

    void setOriginalUrl(const QString& originalUrl)
    {
        m_originalUrl = originalUrl;
    }

    QString remoteUrl() const
    {
        return m_remoteUrl;
    }

    virtual QString toString() const
    {
        return m_title;
    }

    YandexFotkiPhoto(const YandexFotkiPhoto& photo);

    // public
    QStringList tags;

protected:

    friend class YandexFotkiTalker;

    YandexFotkiPhoto(const QString& urn,
                     const QString& author,
                     const QString& title,
                     const QString& summary,
                     const QString& apiEditUrl,
                     const QString& apiSelfUrl,
                     const QString& apiMediaUrl,
                     const QString& apiAlbumUrl,
                     const QDateTime& publishedDate,
                     const QDateTime& editedDate,
                     const QDateTime& updatedDate,
                     const QDateTime& createdDate,
                     Access access,
                     bool hideOriginal,
                     bool disableComments,
                     bool adult,
                     const QString& remoteUrl
                    );

    QString m_urn;
    QString m_author;
    QString m_title;
    QString m_summary;

    QString m_apiEditUrl;
    QString m_apiSelfUrl;
    QString m_apiMediaUrl;
    QString m_apiAlbumUrl;

    QDateTime m_publishedDate;
    QDateTime m_editedDate;
    QDateTime m_updatedDate;
    QDateTime m_createdDate;

    Access m_access;
    bool m_hideOriginal;
    bool m_disableComments;
    bool m_adult;

    QString m_remoteUrl;
    QString m_localUrl;
    QString m_originalUrl;

#ifndef KDE_NO_DEBUG_OUTPUT
    friend QDebug operator<<(QDebug d, const YandexFotkiPhoto& p);
#endif // KDE_NO_DEBUG_OUTPUT

};

#ifndef KDE_NO_DEBUG_OUTPUT
QDebug operator<<(QDebug d, const YandexFotkiPhoto& p);
#endif // KDE_NO_DEBUG_OUTPUT

} // KIPIYandexFotkiPlugin

#endif // YANDEXFOTKI_PHOTO_H
