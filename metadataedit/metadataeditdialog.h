/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-10-11
 * Description : a plugin to edit pictures metadata
 *
 * Copyright 2006 by Gilles Caulier
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

#ifndef METADATAEDITDIALOG_H
#define METADATAEDITDIALOG_H

// KDE includes.

#include <kdialogbase.h>
#include <kurl.h>

// LibKipi includes.

#include <libkipi/interface.h>

class QListViewItem;

namespace KIPIMetadataEditPlugin
{

class MetadataEditDialogPriv;

class MetadataEditDialog :public KDialogBase 
{
    Q_OBJECT

public:

    MetadataEditDialog(KIPI::Interface* interface, QWidget* parent);
    ~MetadataEditDialog();

    void setImages(const KURL::List& images);

protected:

    void closeEvent(QCloseEvent *);

protected slots:

    void slotApply();
    void slotHelp();
    void slotClose();

private slots:

    void slotEditExif();
    void slotLoadExif();
    void slotRemoveExif();
    void slotEditIptc();
    void slotLoadIptc();
    void slotRemoveIptc();

private:

    bool promptUserClose();
    void readSettings();
    void saveSettings();

private:

    MetadataEditDialogPriv *d;
};

}  // NameSpace KIPIMetadataEditPlugin

#endif /* METADATAEDITDIALOG_H */

