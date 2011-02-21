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

#ifndef YANDEXFOTKI_ALBUM_H
#define YANDEXFOTKI_ALBUM_H

// Qt includes

#include <QObject>
#include <QString>
#include <QDateTime>

namespace KIPIYandexFotkiPlugin
{

class YandexFotkiAlbum
{
public:

    YandexFotkiAlbum();
    YandexFotkiAlbum(const YandexFotkiAlbum& album);

    virtual ~YandexFotkiAlbum()
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

    bool isProtected() const
    {
        return !m_password.isNull();
    }

    void setPassword(const QString& password)
    {
        m_password = password;
    }

    virtual QString toString() const
    {
        return m_title;
    }

    // virtual YandexFotkiAlbum& operator=(const YandexFotkiAlbum& album);

protected:

    friend class YandexFotkiTalker;

    YandexFotkiAlbum(const QString& urn,
                     const QString& author,
                     const QString& title,
                     const QString& summary,
                     const QString& apiEditUrl,
                     const QString& apiSelfUrl,
                     const QString& apiPhotosUrl,
                     const QDateTime& publishedDate,
                     const QDateTime& editedDate,
                     const QDateTime& updatedDate,
                     const QString& password
                    );

    QString m_urn;
    QString m_author;
    QString m_title;
    QString m_summary;

    QString m_apiEditUrl;
    QString m_apiSelfUrl;
    QString m_apiPhotosUrl;

    QDateTime m_publishedDate;
    QDateTime m_editedDate;
    QDateTime m_updatedDate;

    QString m_password;

#ifndef KDE_NO_DEBUG_OUTPUT
    friend QDebug operator<<(QDebug d, const YandexFotkiAlbum& p);
#endif // KDE_NO_DEBUG_OUTPUT

};

#ifndef KDE_NO_DEBUG_OUTPUT
QDebug operator<<(QDebug d, const YandexFotkiAlbum& p);
#endif // KDE_NO_DEBUG_OUTPUT

} // KIPIYandexFotkiPlugin

#endif // YANDEXFOTKI_ALBUM_H
