/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-11-20
 * Description : a dialog to batch edit comments
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef COMMENTEDITDIALOG_H
#define COMMENTEDITDIALOG_H

// Qt includes

#include <QString>

// KDE includes

#include <kdialog.h>

namespace KIPIMetadataEditPlugin
{

class CommentEditDialogDialogPrivate;

class CommentEditDialog : public KDialog
{
    Q_OBJECT

public:

    CommentEditDialog(const QString& comment, QWidget* parent);
    ~CommentEditDialog();

    bool syncJFIFCommentIsChecked();
    bool syncEXIFCommentIsChecked();
    bool syncXMPCaptionIsChecked();
    bool syncIPTCCaptionIsChecked();

    void setCheckedSyncJFIFComment(bool c);
    void setCheckedSyncEXIFComment(bool c);
    void setCheckedSyncXMPCaption(bool c);
    void setCheckedSyncIPTCCaption(bool c);

    QString getComments();

protected Q_SLOTS:

    void slotOk();
    void slotHelp();
    void slotCancel();

protected:

    void closeEvent(QCloseEvent *);

private:

    void readSettings();
    void saveSettings();

private:

    CommentEditDialogDialogPrivate* const d;
};

}  // namespace KIPIMetadataEditPlugin

#endif /* COMMENTEDITDIALOG_H */
