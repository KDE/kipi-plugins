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

#ifndef ENFUSESTACK_H
#define ENFUSESTACK_H

// Qt includes

#include <QTreeWidget>
#include <QString>
#include <QPixmap>

// KDE includes

#include <kurl.h>
#include <kfileitem.h>

namespace KIPI
{
class Interface;
}

using namespace KIPI;

namespace KIPIExpoBlendingPlugin
{

class Manager;
class EnfuseStackListPriv;

class EnfuseStackItem : public QTreeWidgetItem
{

public:

    EnfuseStackItem(QTreeWidget* parent);
    virtual ~EnfuseStackItem();

    void setUrl(const KUrl& url);
    KUrl url() const;

    void setTargetFileName(const QString& fn);
    QString targetFileName() const;

    void setOn(bool b);
    bool isOn() const;

    void setThumbnail(const QPixmap& pix);

private:

    KUrl    m_url;
};

// ---------------------------------------------------------------------

class EnfuseStackList : public QTreeWidget
{
    Q_OBJECT

public:

    EnfuseStackList(Manager* mngr, QWidget* parent);
    virtual ~EnfuseStackList();

    void addItem(const KUrl& url);

    KUrl::List urls();

private Q_SLOTS:

    void slotKDEPreview(const KFileItem&, const QPixmap&);
    void slotThumbnail(const KUrl& url, const QPixmap& pix);

private:

    EnfuseStackListPriv* const d;
};

}  // namespace KIPIExpoBlendingPlugin

#endif /* ENFUSESTACK_H */
