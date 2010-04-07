/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-10-11
 * Description : a dialog to edit XMP metadata
 *
 * Copyright (C) 2007-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef XMPEDITDIALOG_H
#define XMPEDITDIALOG_H

// KDE includes

#include <kpagedialog.h>
#include <kurl.h>

class QCloseEvent;
class QEvent;
class QObject;

namespace KIPI
{
class Interface;
}

namespace KIPIMetadataEditPlugin
{

class XMPEditDialogPrivate;

class XMPEditDialog : public KPageDialog
{
    Q_OBJECT

public:

    XMPEditDialog(QWidget* parent, KUrl::List urls, KIPI::Interface *iface);
    ~XMPEditDialog();

public Q_SLOTS:

    void slotModified();

protected Q_SLOTS:

    void slotOk();
    void slotHelp();
    void slotClose();

protected:

    void closeEvent(QCloseEvent *);
    bool eventFilter(QObject *, QEvent *);

private Q_SLOTS:

    void slotItemChanged();
    void slotApply();
    void slotUser1();
    void slotUser2();

private:

    void readSettings();
    void saveSettings();

    int  activePageIndex();
    void showPage(int page);

private:

    XMPEditDialogPrivate* const d;
};

}  // namespace KIPIMetadataEditPlugin

#endif /* XMPEDITDIALOG_H */
