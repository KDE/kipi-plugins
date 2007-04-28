/* ============================================================
 * Copyright 2003 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright 2004 by Tudor Calin <tudor@1xtech.com>

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
// Qt
#include <qstring.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qfileinfo.h>
#include <qlabel.h>
#include <qlayout.h>
// KDE
#include <klocale.h>
#include <kbuttonbox.h>
#include <kiconloader.h>
// Local
#include "savefiledialog.h"

namespace KIPIKameraKlientPlugin
{

SavefileDialog::SavefileDialog(const QString& file, QWidget *parent, const char* name, bool modal) : QDialog(parent, name, modal) {
    QFileInfo fileInfo(file);
    setCaption(i18n("File Already Exists"));
    QLabel *descLbl = new QLabel(i18n("The file '%1' already exists!").arg(fileInfo.absFilePath()), this);
    renameEdit = new QLineEdit(this);
    renameEdit->setText(fileInfo.fileName());
    connect(renameEdit, SIGNAL(textChanged(const QString &)), this, SLOT(slot_renameEnabled()));
    KButtonBox *bbox = new KButtonBox(this);
    renameBtn = bbox->addButton(i18n("Rename"), this, SLOT(slot_rename()), true);
    renameBtn->setEnabled(false);
    bbox->addButton(i18n("Skip"), this, SLOT(slot_skip()), false);
    bbox->addButton(i18n("Skip All"), this, SLOT(slot_skipAll()), true);
    bbox->addButton(i18n("Overwrite"), this, SLOT(slot_overwrite()), true);
    bbox->addButton(i18n("Overwrite All"), this, SLOT(slot_overwriteAll()), true);
    QPushButton *cancelBtn = bbox->addButton(i18n("&Cancel"), this, SLOT(reject()), true);
    cancelBtn->setDefault(true);
    bbox->layout();
    QGridLayout *layout = new QGridLayout(this, 0, 0, 15);
    layout->addMultiCellWidget(descLbl, 0, 0, 0, 3);
    layout->addMultiCellWidget(renameEdit, 3, 3, 0, 3);
    layout->addMultiCellWidget(bbox, 4, 4, 0, 3);
}

SavefileDialog::~SavefileDialog() {
}

SavefileDialog::Operation SavefileDialog::saveFileOperation() {
    return op;
}

QString SavefileDialog::renameFile() {
    return renameEdit->text();
}

void SavefileDialog::slot_renameEnabled() {
    renameBtn->setEnabled(true);
    renameBtn->setDefault(true);
}

}  // NameSpace KIPIKameraKlientPlugin

#include "savefiledialog.moc"
