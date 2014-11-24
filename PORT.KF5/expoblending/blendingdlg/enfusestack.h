/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-12-13
 * Description : a tool to blend bracketed images.
 *
 * Copyright (C) 2009-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include <QPoint>
#include <QList>
#include <QIcon>

// KDE includes

#include <kurl.h>
#include <kfileitem.h>

// Local includes

#include "enfusesettings.h"

namespace KIPI
{
class Interface;
}

using namespace KIPI;

namespace KIPIExpoBlendingPlugin
{

class EnfuseStackItem : public QTreeWidgetItem
{

public:

    explicit EnfuseStackItem(QTreeWidget* const parent);
    virtual ~EnfuseStackItem();

    /** Return the preview image url assigned to item.
     */
    KUrl url() const;

    void setEnfuseSettings(const EnfuseSettings& settings);
    EnfuseSettings enfuseSettings() const;

    void setOn(bool b);
    bool isOn() const;

    void setProgressAnimation(const QPixmap& pix);
    void setThumbnail(const QPixmap& pix);
    void setProcessedIcon(const QIcon& icon);
    bool asValidThumb() const;

private:

    class EnfuseStackItemPriv;
    EnfuseStackItemPriv* const d;
};

// ---------------------------------------------------------------------

class EnfuseStackList : public QTreeWidget
{
    Q_OBJECT

public:

    EnfuseStackList(QWidget* const parent);
    virtual ~EnfuseStackList();

    void setTemplateFileName(KPSaveSettingsWidget::OutputFormat, const QString&);

    void setThumbnail(const KUrl& url, const QImage& img);
    void setOnItem(const KUrl& url, bool on);
    void removeItem(const KUrl& url);
    void clearSelected();
    void addItem(const KUrl& url, const EnfuseSettings& settings);
    void processingItem(const KUrl& url, bool run);
    void processedItem(const KUrl& url, bool success);

    QList<EnfuseSettings> settingsList();

Q_SIGNALS:

    void signalItemClicked(const KUrl&);

private:

    EnfuseStackItem* findItemByUrl(const KUrl& url);

private Q_SLOTS:

    void slotItemClicked(QTreeWidgetItem*);
    void slotContextMenu(const QPoint&);
    void slotRemoveItem();
    void slotProgressTimerDone();

private:

    class EnfuseStackListPriv;
    EnfuseStackListPriv* const d;
};

}  // namespace KIPIExpoBlendingPlugin

#endif /* ENFUSESTACK_H */
