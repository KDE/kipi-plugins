/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-09-04
 * Description : a tool to blend bracketed images/create HDR images.
 *
 * Copyright (C) 2013 by Soumajyoti Sarkar <ergy dot ergy at gmail dot com>
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

#ifndef PFSHDRSTACK_H
#define PFSHDRSTACK_H

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

#include "pfshdrsettings.h"

namespace KIPI
{
class Interface;
}

using namespace KIPI;

namespace KIPIExpoBlendingPlugin
{

class PfsHdrStackItem : public QTreeWidgetItem
{

public:

    explicit PfsHdrStackItem(QTreeWidget* const parent);
    virtual ~PfsHdrStackItem();

    /** Return the preview image url assigned to item.
     */
    KUrl url() const;

    void setPfsHdrSettings(const PfsHdrSettings& settings);
    PfsHdrSettings pfshdrSettings() const;

    void setOn(bool b);
    bool isOn() const;

    void setProgressAnimation(const QPixmap& pix);
    void setThumbnail(const QPixmap& pix);
    void setProcessedIcon(const QIcon& icon);
    bool asValidThumb() const;

private:

    class PfsHdrStackItemPriv;
    PfsHdrStackItemPriv* const d;
};

// ---------------------------------------------------------------------

class PfsHdrStackList : public QTreeWidget
{
    Q_OBJECT

public:

    PfsHdrStackList(QWidget* const parent);
    virtual ~PfsHdrStackList();

    void setTemplateFileName(KPSaveSettingsWidget::OutputFormat, const QString&);

    void setThumbnail(const KUrl& url, const QImage& img);
    void setOnItem(const KUrl& url, bool on);
    void removeItem(const KUrl& url);
    void clearSelected();
    void addItem(const KUrl& url, const PfsHdrSettings& settings);
    void processingItem(const KUrl& url, bool run);
    void processedItem(const KUrl& url, bool success);

    QList<PfsHdrSettings> settingsList();

Q_SIGNALS:

    void signalItemClicked(const KUrl&);

private:

    PfsHdrStackItem* findItemByUrl(const KUrl& url);

private Q_SLOTS:

    void slotItemClicked(QTreeWidgetItem*);
    void slotContextMenu(const QPoint&);
    void slotRemoveItem();
    void slotProgressTimerDone();

private:

    class PfsHdrStackListPriv;
    PfsHdrStackListPriv* const d;
};

}  // namespace KIPIExpoBlendingPlugin

#endif /* PFSHDRSTACK_H */
