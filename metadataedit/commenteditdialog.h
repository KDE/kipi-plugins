/* ============================================================
 * Authors: Caulier Gilles <caulier dot gilles at kdemail dot net>
 * Date   : 2006-11-20
 * Description : a dialog to batch edit comments
 * 
 * Copyright 2006 by Gilles Caulier <caulier dot gilles at kdemail dot net>
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

#ifndef COMMENTEDITDIALOG_H
#define COMMENTEDITDIALOG_H

// Qt includes.

#include <qstring.h>

// KDE includes.

#include <kdialogbase.h>

namespace KIPIMetadataEditPlugin
{

class CommentEditDialogDialogPrivate;

class CommentEditDialog : public KDialogBase
{
    Q_OBJECT

public:

    CommentEditDialog(QWidget* parent);
    ~CommentEditDialog();

    bool syncJFIFCommentIsChecked();
    bool syncEXIFCommentIsChecked();
    bool syncIPTCCaptionIsChecked();

    void setCheckedSyncJFIFComment(bool c);
    void setCheckedSyncEXIFComment(bool c);
    void setCheckedSyncIPTCCaption(bool c);

    QString getComments();

protected slots:

    void slotOk();
    void slotHelp();
    void slotCancel();

protected:

    void closeEvent(QCloseEvent *);

private:

    void readSettings();
    void saveSettings();

private:

    CommentEditDialogDialogPrivate *d;
};

}  // namespace KIPIMetadataEditPlugin

#endif /* COMMENTEDITDIALOG_H */
