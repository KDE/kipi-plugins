/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2011-03-14
 * Description : a dialog to edit EXIF,IPTC and XMP metadata
 *
 * Copyright (C) 2011 by Victor Dodon <dodon dot victor at gmail dot com>
 * Copyright (C) 2006-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef EDITALLMETADATA_H
#define EDITALLMETADATA_H

// Qt includes

#include <QObject>

// KDE includes

#include <kpagedialog.h>
#include <kurl.h>

class QObject;
class QCloseEvent;
class Event;

namespace KIPI
{
    class Interface;
}

using namespace KIPI;

namespace KIPIMetadataEditPlugin
{

class MetadataEditDialog : public KDialog
{
    Q_OBJECT

public:

    MetadataEditDialog(QWidget* parent, const KUrl::List& urls, Interface* iface);
    ~MetadataEditDialog();

    KUrl::List::iterator currentItem() const;
    Interface* iface() const;

public Q_SLOTS:

    void slotModified();

private Q_SLOTS:

    void slotHelp();
    void slotOk();
    void slotClose();
    void slotItemChanged();
    void slotApply();
    void slotNext();
    void slotPrevious();
    void slotSetReadOnly(bool);

protected:

    void closeEvent(QCloseEvent*);
    bool eventFilter(QObject*, QEvent*);

private:

    void saveSettings();
    void readSettings();

private:

    class MetadataEditDialogPrivate;
    MetadataEditDialogPrivate* const d;
};

}  // namespace KIPIMetadataEditPlugin

#endif  /* EDITALLMETADATA_H */
