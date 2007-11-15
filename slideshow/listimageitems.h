/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-21
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2003-2007 Gilles Caulier <caulier dot gilles at gmail dot com>
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

// QT includes

#include <qevent.h>
#include <qwidget.h>
#include <qstring.h>

// KDE includes

#include <klistbox.h>
#include <kurl.h>

namespace KIPISlideShowPlugin
{

class ImageItem : public QListBoxText
{

public:

    ImageItem(QListBox * parent, QString const & name, QString const & comments, QString const & path,
              QString const & album)
    : QListBoxText(parent), _name(name), _comments(comments), _path(path), _album(album)
    {}

    QString comments()                   { return _comments; }
    QString name()                       { return _name;     }
    QString path()                       { return _path;     }
    QString album()                      { return _album;    }
    void setName(const QString &newName) { setText(newName); }

private:

    QString _name;
    QString _comments;
    QString _path;
    QString _album;
};

class ListImageItems : public KListBox 
{
    Q_OBJECT

    public:

        ListImageItems(QWidget *parent=0, const char *name=0);

    signals:

        void addedDropItems(KURL::List filesUrl);

    protected:

        void dragEnterEvent(QDragEnterEvent *e);
        void dropEvent(QDropEvent *e);
};

}  // NameSpace KIPISlideShowPlugin

#endif /* LISTIMAGEITEM_H */
