/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-21
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2008 Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * Copyright (C) 2003-2009 Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef LISTIMAGEITEM_H
#define LISTIMAGEITEM_H

// Qt includes

#include <QWidget>
#include <QString>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QListWidget>
#include <QListWidgetItem>

// KDE includes

#include <kurl.h>
#include <kio/previewjob.h>

// LibKIPI includes

#include <libkipi/interface.h>

namespace KIPIAdvancedSlideshowPlugin
{

class ImageItem : public QObject, public QListWidgetItem
{
    Q_OBJECT

public:

    ImageItem(QListWidget* parent, QString const & name, QString const & comments,
              QString const & path, QString const & album, KIPI::Interface*);
    ~ImageItem();

    QString comments();
    QString name();
    QString path();
    QString album();
    void setName(const QString &newName);

private Q_SLOTS:

    void slotThumbnail(const KUrl&, const QPixmap&);

private:

    QString          m_name;
    QString          m_comments;
    QString          m_path;
    QString          m_album;

    KIPI::Interface* m_interface;
};

class ListImageItems : public QListWidget
{
    Q_OBJECT

public:

    ListImageItems(QWidget *parent = 0);

Q_SIGNALS:

    void addedDropItems(KUrl::List filesUrl);

protected:

    void dragEnterEvent(QDragEnterEvent*);
    void dragMoveEvent(QDragMoveEvent*);
    void dropEvent(QDropEvent*);
};

}  // namespace KIPIAdvancedSlideshowPlugin

#endif /* LISTIMAGEITEM_H */
