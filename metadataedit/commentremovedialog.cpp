/* ============================================================
 * Authors: Caulier Gilles <caulier dot gilles at kdemail dot net>
 * Date   : 2006-11-20
 * Description : a dialog to batch remove comments
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

// Qt includes.

#include <qtimer.h>
#include <qlabel.h>
#include <qframe.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qwhatsthis.h>

// KDE includes.

#include <klocale.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kapplication.h>
#include <khelpmenu.h>
#include <kpopupmenu.h>

// Local includes.

#include "kpaboutdata.h"
#include "commentremovedialog.h"
#include "commentremovedialog.moc"

namespace KIPIMetadataEditPlugin
{

class CommentRemoveDialogDialogPrivate
{

public:

    CommentRemoveDialogDialogPrivate()
    {
        about                  = 0;
        removeJFIFCommentCheck = 0;
        removeEXIFCommentCheck = 0;
        removeIPTCCaptionCheck = 0;
    }

    QCheckBox                *removeJFIFCommentCheck;
    QCheckBox                *removeEXIFCommentCheck;
    QCheckBox                *removeIPTCCaptionCheck;

    KIPIPlugins::KPAboutData *about;
};

CommentRemoveDialog::CommentRemoveDialog(QWidget* parent)
                 : KDialogBase(Plain, i18n("Remove Pictures Comments"),
                               Help|Ok|Cancel, Ok,
                               parent, 0, true, true)
{
    d = new CommentRemoveDialogDialogPrivate;

    // ---------------------------------------------------------------
    // About data and help button.

    d->about = new KIPIPlugins::KPAboutData(I18N_NOOP("Edit Metadata"),
                                            NULL,
                                            KAboutData::License_GPL,
                                            I18N_NOOP("A Plugin to edit pictures metadata"),
                                            "(c) 2006, Gilles Caulier");

    d->about->addAuthor("Gilles Caulier", I18N_NOOP("Author and Maintainer"),
                        "caulier dot gilles at kdemail dot net");

    KHelpMenu* helpMenu = new KHelpMenu(this, d->about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Edit Metadata Handbook"),
                                 this, SLOT(slotHelp()), 0, -1, 0);
    actionButton(Help)->setPopup( helpMenu->menu() );

    // ------------------------------------------------------------

    QVBoxLayout *vlay = new QVBoxLayout(plainPage(), 0, KDialog::spacingHint());

    d->removeJFIFCommentCheck = new QCheckBox(i18n("Remove JFIF comment section"), plainPage());
    d->removeEXIFCommentCheck = new QCheckBox(i18n("Remove EXIF user comment"), plainPage());
    d->removeIPTCCaptionCheck = new QCheckBox(i18n("Remove IPTC caption"), plainPage());

    QLabel *note = new QLabel(i18n("<b>Note: Comments from current selected pictures "
                                   "will be permanently removed.</b>"), plainPage());
    

    vlay->addWidget(d->removeJFIFCommentCheck);
    vlay->addWidget(d->removeEXIFCommentCheck);
    vlay->addWidget(d->removeIPTCCaptionCheck);
    vlay->addWidget(note);

    // ------------------------------------------------------------

    readSettings();
}

CommentRemoveDialog::~CommentRemoveDialog()
{
    delete d->about;
    delete d;
}

void CommentRemoveDialog::slotHelp()
{
    KApplication::kApplication()->invokeHelp("metadataedit", "kipi-plugins");
}

void CommentRemoveDialog::closeEvent(QCloseEvent *e)
{
    if (!e) return;
    saveSettings();
    e->accept();
}

void CommentRemoveDialog::slotCancel()
{
    saveSettings();
    KDialogBase::slotCancel();
}

void CommentRemoveDialog::readSettings()
{
    KConfig config("kipirc");
    config.setGroup("Comments Remove Settings");
    setCheckedRemoveJFIFComment(config.readBoolEntry("Remove JFIF Comment", true));
    setCheckedRemoveEXIFComment(config.readBoolEntry("Remove EXIF Comment", true));
    setCheckedRemoveIPTCCaption(config.readBoolEntry("Remove IPTC Caption", true));
    resize(configDialogSize(config, QString("Comments Remove Dialog")));
}

void CommentRemoveDialog::saveSettings()
{
    KConfig config("kipirc");
    config.setGroup("Comments Remove Settings");
    config.writeEntry("Remove JFIF Comment", removeJFIFCommentIsChecked());
    config.writeEntry("Remove EXIF Comment", removeEXIFCommentIsChecked());
    config.writeEntry("Remove IPTC Caption", removeIPTCCaptionIsChecked());
    saveDialogSize(config, QString("Comments Remove Dialog"));
    config.sync();
}

void CommentRemoveDialog::slotOk()
{
    saveSettings();
    accept();
}

bool CommentRemoveDialog::removeJFIFCommentIsChecked()
{
    return d->removeJFIFCommentCheck->isChecked();
}

bool CommentRemoveDialog::removeEXIFCommentIsChecked()
{
    return d->removeEXIFCommentCheck->isChecked();
}

bool CommentRemoveDialog::removeIPTCCaptionIsChecked()
{
    return d->removeIPTCCaptionCheck->isChecked();
}

void CommentRemoveDialog::setCheckedRemoveJFIFComment(bool c)
{
    d->removeJFIFCommentCheck->setChecked(c);
}

void CommentRemoveDialog::setCheckedRemoveEXIFComment(bool c)
{
    d->removeEXIFCommentCheck->setChecked(c);
}

void CommentRemoveDialog::setCheckedRemoveIPTCCaption(bool c)
{
    d->removeIPTCCaptionCheck->setChecked(c);
}

}  // namespace KIPIMetadataEditPlugin
