/* ============================================================
 * Authors: Caulier Gilles <caulier dot gilles at gmail dot com>
 * Date   : 2006-10-12
 * Description : a dialog to edit EXIF metadata
 * 
 * Copyright 2006 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef EXIFEDITDIALOG_H
#define EXIFEDITDIALOG_H

// Qt includes.

#include <qcstring.h>

// KDE includes.

#include <kdialogbase.h>
#include <kurl.h>

namespace KIPI
{
    class Interface;
}

namespace KIPIMetadataEditPlugin
{

class EXIFEditDialogDialogPrivate;

class EXIFEditDialog : public KDialogBase
{
    Q_OBJECT

public:

    EXIFEditDialog(QWidget* parent, KURL::List urls, KIPI::Interface *iface);
    ~EXIFEditDialog();

public slots:

    void slotModified();

protected slots:

    void slotOk();
    void slotHelp();
    void slotClose();

protected:

    void closeEvent(QCloseEvent *);
    bool eventFilter(QObject *, QEvent *);

private slots:

    void slotItemChanged();
    void slotApply();
    void slotUser1();
    void slotUser2();

private:

    void readSettings();
    void saveSettings();

private:

    EXIFEditDialogDialogPrivate *d;
};

}  // namespace KIPIMetadataEditPlugin

#endif /* EXIFEDITDIALOG_H */
