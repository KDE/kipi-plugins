/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-18
 * Description : images list settings page.
 *
 * Copyright (C) 2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef IMAGES_PAGE_H
#define IMAGES_PAGE_H

// Qt includes.

#include <QListWidget>
#include <QObject>
#include <QWidget>
#include <QPixmap>

// KDE includes.

#include <kurl.h>

// Local includes.

#include "emailsettingscontainer.h"

namespace KIPI
{
    class Interface;
}

namespace KIPISendimagesPlugin
{

class ImagesPagePriv;

class ImagesListViewItem : public QListWidgetItem
{

public:

    ImagesListViewItem(QListWidget *view, KIPI::Interface *iface, const KUrl& url);
    ~ImagesListViewItem();

    void setUrl(const KUrl& url);
    KUrl url();

    void setComments(const QString& comments);
    QString comments();

    void setTags(const QStringList& tags);
    QStringList tags();

    void setRating(int rating);
    int rating();

    void setThumb(const QPixmap& pix);

    EmailItem emailItem();
    
private: 

    EmailItem m_item;
};

class ImagesListView : public QListWidget
{

    Q_OBJECT
    
public:

    ImagesListView(QWidget *parent);
    ~ImagesListView();

signals:

    void addedDropedItems(const KUrl::List& urls);

private:

    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);
};

class ImagesPage : public QWidget
{
    Q_OBJECT
    
public:

    ImagesPage(QWidget* parent, KIPI::Interface *iface);
    ~ImagesPage();

    QList<EmailItem> imagesList();

public slots:

    void slotAddImages(const KUrl::List& list);

private slots:


    void slotAddItems();
    void slotRemoveItems();
    void slotThumbnail(const KUrl& url, const QPixmap& pix);

private:

    ImagesPagePriv* d;
};

}  // namespace KIPISendimagesPlugin

#endif // IMAGES_PAGE_H 
