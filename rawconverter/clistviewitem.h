/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-25
 * Description : Raw file list view used into batch converter.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef CLISTVIEWITEM_H
#define CLISTVIEWITEM_H

// Qt includes

#include <QBrush>
#include <QTreeWidgetItem>
#include <QPainter>
#include <QPixmap>
#include <QString>
#include <QIcon>

// KDE includes

#include <kurl.h>

namespace KIPIRawConverterPlugin
{

class CListViewItem : public QTreeWidgetItem
{

public:

    CListViewItem(QTreeWidget *view, const QPixmap& pixmap, const KUrl& url, const QString& fileName)
        : QTreeWidgetItem(view)
    {
         setThumbnail(pixmap);
         setUrl(url);
         setDestFileName(fileName);
         setEnabled(true);
    }

    ~CListViewItem(){}

    void setThumbnail(const QPixmap& pix)
    {
        int iconSize = treeWidget()->iconSize().width();
        QPixmap pixmap(iconSize+2, iconSize+2);
        pixmap.fill(Qt::transparent);
        QPainter p(&pixmap);
        p.drawPixmap((pixmap.width()/2)  - (pix.width()/2),
                     (pixmap.height()/2) - (pix.height()/2), pix);
        setIcon(0, QIcon(pixmap));
    }

    void setProgressIcon(const QIcon& icon) 
    {
        setIcon(1, icon);
    }

    void setUrl(const KUrl& url) 
    {
        m_url = url;
        setText(1, m_url.fileName());
    }

    KUrl url() const
    {
        return m_url;
    }

    void setDestFileName(const QString& str) 
    {
        m_destFileName = str;
        setText(2, m_destFileName);
    }

    QString destFileName() const
    {
        return m_destFileName;
    }

    void setIdentity(const QString& str) 
    {
        m_identity = str;
        setText(3, m_identity);
    }

    QString identity() const
    {
        return m_identity;
    }

    QString destPath() const
    {
        QString path = url().directory() + "/" + destFileName();
        return path;
    }

    void setEnabled(bool d)
    {
        m_enabled = d;
        setForeground(0, QBrush(Qt::gray));
        setForeground(1, QBrush(Qt::gray));
        setForeground(2, QBrush(Qt::gray));
        setForeground(3, QBrush(Qt::gray));
    }

    bool isEnabled()
    {
        return m_enabled;
    }

private:

    bool    m_enabled;

    QString m_destFileName;
    QString m_identity;

    KUrl    m_url;
};

} // namespace KIPIRawConverterPlugin

#endif /* CLISTVIEWITEM_H */
