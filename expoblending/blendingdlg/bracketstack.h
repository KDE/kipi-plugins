/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-12-13
 * Description : a tool to blend bracketed images.
 *
 * Copyright (C) 2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef BRACKETSTACK_H
#define BRACKETSTACK_H

// Qt includes

#include <QTreeWidget>
#include <QString>
#include <QPixmap>

// KDE includes.

#include <kurl.h>

namespace KIPIExpoBlendingPlugin
{

class BracketStackItem : public QTreeWidgetItem
{

public:

    BracketStackItem(QTreeWidget* parent);
    virtual ~BracketStackItem();

    void setUrl(const KUrl& url);
    KUrl url() const;

    void setAlignedUrl(const KUrl& alignedUrl);
    KUrl alignedUrl() const;

    void setOn(bool b);
    bool isOn() const;

    void setThumbnail(const QPixmap& pix);
    void setExposure(const QString& exp);

private:

    KUrl m_url;
    KUrl m_alignedUrl;
};

// ---------------------------------------------------------------------

class BracketStackList : public QTreeWidget
{

public:

    BracketStackList(QWidget *parent);
    virtual ~BracketStackList();

    KUrl::List itemsList();
};

}  // namespace KIPIExpoBlendingPlugin

#endif /* BRACKETSTACK_H */
