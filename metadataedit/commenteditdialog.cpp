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

#include "commenteditdialog.h"
#include "commenteditdialog.moc"

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

class CommentEditDialogDialogPrivate
{

public:

    CommentEditDialogDialogPrivate()
    {
        userCommentEdit      = 0;
        about                = 0;
        syncJFIFCommentCheck = 0;
        syncEXIFCommentCheck = 0;
        syncXMPCaptionCheck  = 0;
        syncIPTCCaptionCheck = 0;
    }

    QCheckBox                *syncJFIFCommentCheck;
    QCheckBox                *syncEXIFCommentCheck;
    QCheckBox                *syncIPTCCaptionCheck;
    QCheckBox                *syncXMPCaptionCheck;

    KTextEdit                *userCommentEdit;

    KIPIPlugins::KPAboutData *about;
};

CommentEditDialog::CommentEditDialog(const QString& comment, QWidget* parent)
                 : KDialog(parent), d(new CommentEditDialogDialogPrivate)
{
    setButtons(Help | Ok | Cancel);
    setDefaultButton(Ok);
    setCaption(i18n("Edit Image Caption"));
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

    QLabel *title = new QLabel(i18n("Enter the image caption entered through <b>%1</b>. "
                                    "This field is not limited (excepted with IPTC). UTF-8 encoding "
                                    "will be used to save text.",
                                    KGlobal::mainComponent().aboutData()->programName()),
                               mainWidget());
    title->setWordWrap(true);

    d->userCommentEdit = new KTextEdit(mainWidget());
    d->userCommentEdit->setText(comment);

    d->syncJFIFCommentCheck = new QCheckBox(i18n("Sync JFIF Comment section"), mainWidget());
    d->syncEXIFCommentCheck = new QCheckBox(i18n("Sync EXIF Comment"), mainWidget());
    d->syncXMPCaptionCheck  = new QCheckBox(i18n("Sync XMP Caption"), mainWidget());
    d->syncIPTCCaptionCheck = new QCheckBox(i18n("Sync IPTC caption (warning: limited to 2000 printable "
                                                 "Ascii characters)"), mainWidget());

    if (!KExiv2Iface::KExiv2::supportXmp())
        d->syncXMPCaptionCheck->setEnabled(false);

    QLabel *note = new QLabel(i18n("<b>Note: captions from currently selected images "
                                   "will be permanently replaced.</b>"), mainWidget());
    note->setWordWrap(true);

    // ------------------------------------------------------------

    vlay->addWidget(title);
    vlay->addWidget(d->userCommentEdit);
    vlay->addWidget(d->syncJFIFCommentCheck);
    vlay->addWidget(d->syncEXIFCommentCheck);
    vlay->addWidget(d->syncXMPCaptionCheck);
    vlay->addWidget(d->syncIPTCCaptionCheck);
    vlay->addWidget(note);
    vlay->setMargin(0);
    vlay->setSpacing(KDialog::spacingHint());

    // ------------------------------------------------------------

    connect(this, SIGNAL(cancelClicked()),
            this, SLOT(slotCancel()));

    connect(this, SIGNAL(okClicked()),
            this, SLOT(slotOk()));

    // ------------------------------------------------------------

    d->userCommentEdit->setFocus();
    readSettings();
}

CommentEditDialog::~CommentEditDialog()
{
    delete d->about;
    delete d;
}

void CommentEditDialog::slotHelp()
{
    KToolInvocation::invokeHelp("metadataedit", "kipi-plugins");
}

void CommentEditDialog::closeEvent(QCloseEvent *e)
{
    if (!e) return;
    saveSettings();
    e->accept();
}

void CommentEditDialog::slotCancel()
{
    saveSettings();
    reject();
}

void CommentEditDialog::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group("Comments Edit Settings");
    setCheckedSyncJFIFComment(group.readEntry("Sync JFIF Comment", true));
    setCheckedSyncEXIFComment(group.readEntry("Sync EXIF Comment", true));
    setCheckedSyncXMPCaption(group.readEntry("Sync XMP Caption", true));
    setCheckedSyncIPTCCaption(group.readEntry("Sync IPTC Caption", true));
    KConfigGroup group2 = config.group(QString("Comments Edit Dialog"));
    restoreDialogSize(group2);
}

void CommentEditDialog::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group("Comments Edit Settings");
    group.writeEntry("Sync JFIF Comment", syncJFIFCommentIsChecked());
    group.writeEntry("Sync EXIF Comment", syncEXIFCommentIsChecked());
    group.writeEntry("Sync XMP Caption", syncXMPCaptionIsChecked());
    group.writeEntry("Sync IPTC Caption", syncIPTCCaptionIsChecked());
    KConfigGroup group2 = config.group(QString("Comments Edit Dialog"));
    saveDialogSize(group2);
    config.sync();
}

void CommentEditDialog::slotOk()
{
    saveSettings();
    accept();
}

bool CommentEditDialog::syncJFIFCommentIsChecked()
{
    return d->syncJFIFCommentCheck->isChecked();
}

bool CommentEditDialog::syncEXIFCommentIsChecked()
{
    return d->syncEXIFCommentCheck->isChecked();
}

bool CommentEditDialog::syncXMPCaptionIsChecked()
{
    return d->syncXMPCaptionCheck->isChecked();
}

bool CommentEditDialog::syncIPTCCaptionIsChecked()
{
    return d->syncIPTCCaptionCheck->isChecked();
}

QString CommentEditDialog::getComments()
{
    return d->userCommentEdit->toPlainText();
}

void CommentEditDialog::setCheckedSyncJFIFComment(bool c)
{
    d->syncJFIFCommentCheck->setChecked(c);
}

void CommentEditDialog::setCheckedSyncEXIFComment(bool c)
{
    d->syncEXIFCommentCheck->setChecked(c);
}

void CommentEditDialog::setCheckedSyncXMPCaption(bool c)
{
    d->syncXMPCaptionCheck->setChecked(c);
}

void CommentEditDialog::setCheckedSyncIPTCCaption(bool c)
{
    d->syncIPTCCaptionCheck->setChecked(c);
}

}  // namespace KIPIMetadataEditPlugin
