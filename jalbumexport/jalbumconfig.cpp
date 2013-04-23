/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-02-28
 * Description : a plugin to launch jAlbum using selected images.
 *
 * Copyright (C) 2013 by Andrew Goodbody <ajg zero two at elfringham dot co dot uk>
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

#include "jalbumconfig.moc"

// Qt includes

#include <QFrame>
#include <QGridLayout>
#include <QPushButton>
#include <QDir>

// KDE includes

#include <kdebug.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <khbox.h>
#include <kcombobox.h>
#include <kpushbutton.h>
#include <kurl.h>
#include <kurlrequester.h>
#include <kmessagebox.h>

// LibKIPI includes

#include <libkipi/version.h>
#include <libkipi/interface.h>

// Local includes

#include "jalbum.h"

namespace KIPIJAlbumExportPlugin
{

class JAlbumEdit::Private
{
public:

    Private()
    {
        jalbum             = 0;
        albumsInput        = 0;
        jarInput           = 0;
        albumsDialog       = 0;
        jarDialog          = 0;
        albumsSearchButton = 0;
    }

    JAlbum*             jalbum;
    KUrlComboRequester* albumsInput;
    KUrlComboRequester* jarInput;
    KFileDialog*        albumsDialog;
    KFileDialog*        jarDialog;
    KPushButton*        albumsSearchButton;
    KPushButton*        jarSearchButton;
    KUrl                albumsPath;
    KUrl                jarPath;
};

JAlbumEdit::JAlbumEdit(QWidget* const pParent, JAlbum* const pJAlbum, const QString& title)
    : KDialog(pParent, Qt::Dialog), d(new Private())
{
    d->jalbum = pJAlbum;

    setCaption(title);

    QFrame* const page              = new QFrame(this);
    QGridLayout* const centerLayout = new QGridLayout();
    page->setMinimumSize(500, 100);
    setMainWidget(page);

    //---------------------------------------------

    KHBox* const hbox   = new KHBox();
    QLabel* const label = new QLabel(hbox);
    d->albumsInput      = new KUrlComboRequester(hbox);
    d->albumsDialog     = 0;

    if(d->albumsInput->button())
        d->albumsInput->button()->hide();

    d->albumsInput->comboBox()->setEditable(true);

    label->setText(i18n("jAlbum albums location:"));
    d->albumsInput->setWhatsThis(i18n(
                    "Sets the path where the jAlbum albums are stored."));
    d->albumsInput->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    d->albumsSearchButton = new KPushButton(i18n("Select Albums Location"), this);
    d->albumsSearchButton->setIcon(KIcon("folder-pictures"));
    d->albumsSearchButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    centerLayout->addWidget(hbox);
    centerLayout->addWidget(d->albumsSearchButton);

    //---------------------------------------------

    KHBox* const hbox2   = new KHBox();
    QLabel* const label2 = new QLabel(hbox2);
    d->jarInput          = new KUrlComboRequester(hbox2);
    d->jarDialog         = 0;

    if(d->jarInput->button())
        d->jarInput->button()->hide();

    d->jarInput->comboBox()->setEditable(true);

    label2->setText(i18n("jAlbum jar file:"));
    d->jarInput->setWhatsThis(i18n(
                    "Sets the path of the jAlbum jar file."));
    d->jarInput->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    d->jarSearchButton = new KPushButton(i18n("Select jAlbum jar File Location"), this);
    d->jarSearchButton->setIcon(KIcon("folder-pictures"));
    d->jarSearchButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    centerLayout->addWidget(hbox2);
    centerLayout->addWidget(d->jarSearchButton);

    //---------------------------------------------

    page->setLayout(centerLayout);

    resize(QSize(300, 150).expandedTo(minimumSizeHint()));

    // setting initial data
    d->albumsInput->setUrl(d->jalbum->albumPath().prettyUrl());
    d->albumsPath = d->jalbum->albumPath();
    d->jarInput->setUrl(d->jalbum->jarPath().prettyUrl());
    d->jarPath = d->jalbum->jarPath();

    connect(d->albumsInput, SIGNAL(textChanged(QString)),
            this, SLOT(slotAlbumsPathChanged(QString)));

    connect(d->albumsSearchButton, SIGNAL(clicked(bool)),
            this, SLOT(slotShowAlbumDialogClicked(bool)));

    connect(d->jarInput, SIGNAL(textChanged(QString)),
            this, SLOT(slotJarPathChanged(QString)));

    connect(d->jarSearchButton, SIGNAL(clicked(bool)),
            this, SLOT(slotShowJarDialogClicked(bool)));

    connect(this, SIGNAL(okClicked()),
            this, SLOT(slotOk()));
}

JAlbumEdit::~JAlbumEdit()
{
    delete d;
}

void JAlbumEdit::slotShowAlbumDialogClicked(bool checked)
{
    Q_UNUSED(checked);

    d->albumsDialog = new KFileDialog(KUrl(), "*", this);
    d->albumsDialog->setMode(KFile::Directory);
    d->albumsDialog->setWindowTitle(i18n("Select Albums Location"));
    d->albumsDialog->setOperationMode(KFileDialog::Opening);
    d->albumsDialog->setUrl(d->albumsPath);

    if (d->albumsDialog->exec() == KFileDialog::Accepted)
    {
        d->albumsPath.setUrl("file:///" + QDir::toNativeSeparators(d->albumsDialog->selectedUrl().path()));
#ifndef WIN32
        d->albumsPath.cleanPath();
#endif
        updateAlbumsPath();
    }

    delete d->albumsDialog;
}

void JAlbumEdit::slotShowJarDialogClicked(bool checked)
{
    Q_UNUSED(checked);

    d->jarDialog = new KFileDialog(KUrl(), "*", this);
    d->jarDialog->setMode(KFile::File | KFile::ExistingOnly | KFile::LocalOnly);
    d->jarDialog->setWindowTitle(i18n("Select jar File Location"));
    d->jarDialog->setOperationMode(KFileDialog::Other);
    d->jarDialog->setUrl(d->jarPath.directory());
    d->jarDialog->setSelection(d->jarPath.fileName());

    if (d->jarDialog->exec() == KFileDialog::Accepted)
    {
        d->jarPath.setUrl("file:///" + QDir::toNativeSeparators(d->jarDialog->selectedUrl().path()));
#ifndef WIN32
        d->jarPath.cleanPath();
#endif
        updateJarPath();
    }

    delete d->jarDialog;
}

void JAlbumEdit::updateAlbumsPath()
{
    QString urlstring;

    if (d->albumsPath.isValid())
    {
        urlstring = d->albumsPath.prettyUrl();
        d->albumsInput->setUrl(urlstring);
    }
}

void JAlbumEdit::updateJarPath()
{
    QString urlstring;

    if (d->jarPath.isValid())
    {
        urlstring = d->jarPath.prettyUrl();
        d->jarInput->setUrl(urlstring);
    }
}

void JAlbumEdit::slotAlbumsPathChanged(const QString& path)
{
    d->albumsPath.setUrl("file:///" + QDir::toNativeSeparators(path));
#ifndef WIN32
    d->albumsPath.cleanPath();
#endif
}

void JAlbumEdit::slotJarPathChanged(const QString& path)
{
    d->jarPath.setUrl("file:///" + QDir::toNativeSeparators(path));
#ifndef WIN32
    d->jarPath.cleanPath();
#endif
}

void JAlbumEdit::slotOk()
{
    struct stat stbuf;

    if (::stat(QFile::encodeName(d->albumsPath.path()), &stbuf) != 0)
    {
        if (KMessageBox::warningYesNo(this,
                     i18n("Directory %1 does not exist, do you wish to create it?", QDir::toNativeSeparators(d->albumsPath.path())))
                     == KMessageBox::No)
        {
            return;
        }

        if (!JAlbum::createDir(d->albumsPath.path()))
        {
            KMessageBox::information(this,
                     i18n("Failed to create directory"));
            return;
        }
    }
    else
    {
        if (!S_ISDIR(stbuf.st_mode))
        {
            KMessageBox::information(this, i18n("Chosen path is not a directory"));
            return;
        }
    }

    d->jalbum->setPath(d->albumsPath.prettyUrl());
    d->jalbum->setJar(d->jarPath.prettyUrl());
    d->jalbum->save();
    accept();
}

} // namespace KIPIJAlbumExportPlugin
