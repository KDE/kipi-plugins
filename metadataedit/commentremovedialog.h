/* ============================================================
 * Authors: Caulier Gilles <caulier dot gilles at kdemail dot net>
 * Date   : 2006-11-20
 * Description : a dialog to batch remove comments
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

#ifndef COMMENTREMOVEDIALOG_H
#define COMMENTREMOVEDIALOG_H

// KDE includes.

#include <kdialogbase.h>

namespace KIPIMetadataEditPlugin
{

class CommentRemoveDialogDialogPrivate;

class CommentRemoveDialog : public KDialogBase
{
    Q_OBJECT

public:

    CommentRemoveDialog(QWidget* parent);
    ~CommentRemoveDialog();

    bool removeHOSTCommentIsChecked();
    bool removeJFIFCommentIsChecked();
    bool removeEXIFCommentIsChecked();
    bool removeIPTCCaptionIsChecked();

    void setCheckedRemoveHOSTComment(bool c);
    void setCheckedRemoveJFIFComment(bool c);
    void setCheckedRemoveEXIFComment(bool c);
    void setCheckedRemoveIPTCCaption(bool c);

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

    CommentRemoveDialogDialogPrivate *d;
};

}  // namespace KIPIMetadataEditPlugin

#endif /* COMMENTREMOVEDIALOG_H */
