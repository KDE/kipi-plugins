/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-11-20
 * Description : a dialog to batch remove comments
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

#include "commentremovedialog.h"
#include "commentremovedialog.moc"

// Qt includes

#include <QCheckBox>
#include <QCloseEvent>
#include <QLabel>
#include <QVBoxLayout>
// KDE includes

#include <kcomponentdata.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kglobal.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmenu.h>
#include <kpushbutton.h>
#include <ktextedit.h>
#include <ktoolinvocation.h>

// LibKExiv2 includes

#include <libkexiv2/kexiv2.h>

// Local includes

#include "pluginsversion.h"
#include "kpaboutdata.h"

namespace KIPIMetadataEditPlugin
{

class CommentRemoveDialogDialogPrivate
{

public:

    CommentRemoveDialogDialogPrivate()
    {
        about                  = 0;
        removeHOSTCommentCheck = 0;
        removeJFIFCommentCheck = 0;
        removeEXIFCommentCheck = 0;
        removeXMPCaptionCheck  = 0;
        removeIPTCCaptionCheck = 0;
    }

    QCheckBox                *removeHOSTCommentCheck;
    QCheckBox                *removeJFIFCommentCheck;
    QCheckBox                *removeEXIFCommentCheck;
    QCheckBox                *removeXMPCaptionCheck;
    QCheckBox                *removeIPTCCaptionCheck;

    KIPIPlugins::KPAboutData *about;
};

CommentRemoveDialog::CommentRemoveDialog(QWidget* parent)
                   : KDialog(parent), d(new CommentRemoveDialogDialogPrivate)
{
    setButtons(Help | Ok | Cancel);
    setDefaultButton(Ok);
    setCaption(i18n("Remove Image Caption"));
    setModal(true);

    // ---------------------------------------------------------------
    // About data and help button.

    d->about = new KIPIPlugins::KPAboutData(ki18n("Edit Metadata"),
                                            0,
                                            KAboutData::License_GPL,
                                            ki18n("A Plugin to edit pictures' metadata."),
                                            ki18n("(c) 2006-2009, Gilles Caulier"));

    d->about->addAuthor(ki18n("Gilles Caulier"), ki18n("Author and Maintainer"),
                        "caulier dot gilles at gmail dot com");

    disconnect(this, SIGNAL(helpClicked()),
               this, SLOT(slotHelp()));

    KHelpMenu* helpMenu = new KHelpMenu(this, d->about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction *handbook   = new QAction(i18n("Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    button(Help)->setMenu(helpMenu->menu());

    // ------------------------------------------------------------

    setMainWidget(new QWidget(this));
    QVBoxLayout *vlay = new QVBoxLayout(mainWidget());

    d->removeHOSTCommentCheck = new QCheckBox(i18n("Remove caption hosted by %1",
                                    KGlobal::mainComponent().aboutData()->programName()),
                                    mainWidget());
    d->removeJFIFCommentCheck = new QCheckBox(i18n("Remove JFIF Comment section"), mainWidget());
    d->removeEXIFCommentCheck = new QCheckBox(i18n("Remove EXIF Comment"), mainWidget());
    d->removeXMPCaptionCheck  = new QCheckBox(i18n("Remove XMP Caption"), mainWidget());
    d->removeIPTCCaptionCheck = new QCheckBox(i18n("Remove IPTC Caption"), mainWidget());

    if (!KExiv2Iface::KExiv2::supportXmp())
        d->removeXMPCaptionCheck->setEnabled(false);

    QLabel *note = new QLabel(i18n("<b>Note: Captions from currently selected images "
                                   "will be permanently removed.</b>"), mainWidget());
    note->setWordWrap(true);

    // ------------------------------------------------------------

    vlay->addWidget(d->removeHOSTCommentCheck);
    vlay->addWidget(d->removeJFIFCommentCheck);
    vlay->addWidget(d->removeEXIFCommentCheck);
    vlay->addWidget(d->removeXMPCaptionCheck);
    vlay->addWidget(d->removeIPTCCaptionCheck);
    vlay->addWidget(note);
    vlay->setMargin(0);
    vlay->setSpacing(KDialog::spacingHint());

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
    KToolInvocation::invokeHelp("metadataedit", "kipi-plugins");
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
    reject();
}

void CommentRemoveDialog::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group("Comments Remove Settings");
    setCheckedRemoveHOSTComment(group.readEntry("Remove HOST Comment", true));
    setCheckedRemoveJFIFComment(group.readEntry("Remove JFIF Comment", true));
    setCheckedRemoveEXIFComment(group.readEntry("Remove EXIF Comment", true));
    setCheckedRemoveXMPCaption(group.readEntry("Remove XMP Caption", true));
    setCheckedRemoveIPTCCaption(group.readEntry("Remove IPTC Caption", true));
    KConfigGroup group2 = config.group(QString("Comments Remove Dialog"));
    restoreDialogSize(group2);
}

void CommentRemoveDialog::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group("Comments Remove Settings");
    group.writeEntry("Remove HOST Comment", removeHOSTCommentIsChecked());
    group.writeEntry("Remove JFIF Comment", removeJFIFCommentIsChecked());
    group.writeEntry("Remove EXIF Comment", removeEXIFCommentIsChecked());
    group.writeEntry("Remove XMP Caption", removeXMPCaptionIsChecked());
    group.writeEntry("Remove IPTC Caption", removeIPTCCaptionIsChecked());
    KConfigGroup group2 = config.group(QString("Comments Remove Dialog"));
    saveDialogSize(group2);
    config.sync();
}

void CommentRemoveDialog::slotOk()
{
    saveSettings();
    accept();
}

bool CommentRemoveDialog::removeHOSTCommentIsChecked()
{
    return d->removeHOSTCommentCheck->isChecked();
}

bool CommentRemoveDialog::removeJFIFCommentIsChecked()
{
    return d->removeJFIFCommentCheck->isChecked();
}

bool CommentRemoveDialog::removeEXIFCommentIsChecked()
{
    return d->removeEXIFCommentCheck->isChecked();
}

bool CommentRemoveDialog::removeXMPCaptionIsChecked()
{
    return d->removeXMPCaptionCheck->isChecked();
}

bool CommentRemoveDialog::removeIPTCCaptionIsChecked()
{
    return d->removeIPTCCaptionCheck->isChecked();
}

void CommentRemoveDialog::setCheckedRemoveHOSTComment(bool c)
{
    d->removeHOSTCommentCheck->setChecked(c);
}

void CommentRemoveDialog::setCheckedRemoveJFIFComment(bool c)
{
    d->removeJFIFCommentCheck->setChecked(c);
}

void CommentRemoveDialog::setCheckedRemoveEXIFComment(bool c)
{
    d->removeEXIFCommentCheck->setChecked(c);
}

void CommentRemoveDialog::setCheckedRemoveXMPCaption(bool c)
{
    d->removeXMPCaptionCheck->setChecked(c);
}

void CommentRemoveDialog::setCheckedRemoveIPTCCaption(bool c)
{
    d->removeIPTCCaptionCheck->setChecked(c);
}

}  // namespace KIPIMetadataEditPlugin
