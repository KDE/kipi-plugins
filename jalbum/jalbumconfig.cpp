/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2013-02-28
 * Description : a plugin to launch jAlbum using selected images.
 *
 * Copyright (C) 2013-2017 by Andrew Goodbody <ajg zero two at elfringham dot co dot uk>
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

#include "jalbumconfig.h"

#include <sys/stat.h>

// Qt includes

#include <QFileDialog>
#include <QMessageBox>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QFrame>
#include <QDir>
#include <QUrl>

// KDE includes

#include <klocalizedstring.h>

// Libkipi includes

#include <KIPI/Interface>

// Local includes

#include "jalbum.h"
#include "jalbumconfig.h"
#include "kipiplugins_debug.h"

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

    JAlbum*      jalbum;
    QLineEdit*   albumsInput;
    QLineEdit*   jarInput;
    QFileDialog* albumsDialog;
    QFileDialog* jarDialog;
    QPushButton* albumsSearchButton;
    QPushButton* jarSearchButton;
    QUrl         albumsPath;
    QUrl         jarPath;
};

JAlbumEdit::JAlbumEdit(QWidget* const pParent, JAlbum* const pJAlbum, const QString& title)
    : QDialog(pParent, Qt::Dialog), d(new Private())
{
    d->jalbum = pJAlbum;

    setWindowTitle(title);

    QGridLayout* const centerLayout = new QGridLayout();
    centerLayout->setGeometry(QRect(0, 0, 2, 5));
    setMinimumSize(500, 100);

    //---------------------------------------------

    d->albumsInput            = new QLineEdit(this);
    d->albumsDialog           = 0;

    d->albumsInput->setEnabled(true);

    d->albumsSearchButton     = new QPushButton(i18n("Select Albums Location"), this);
    d->albumsSearchButton->setIcon(QIcon::fromTheme(QLatin1String("folder-pictures")));
    d->albumsSearchButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    centerLayout->addWidget(d->albumsInput, 0, 0, 1, 2);
    centerLayout->addWidget(d->albumsSearchButton);

    //---------------------------------------------

    d->jarInput               = new QLineEdit(this);
    d->jarDialog              = 0;

    d->jarInput->setEnabled(true);

    d->jarSearchButton        = new QPushButton(i18n("Select jAlbum jar File Location"), this);
    d->jarSearchButton->setIcon(QIcon::fromTheme(QLatin1String("folder-pictures")));
    d->jarSearchButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    centerLayout->addWidget(d->jarInput, 2, 0, 1, 2);
    centerLayout->addWidget(d->jarSearchButton);

    QPushButton *okButton     = new QPushButton(i18n("OK"));
    QPushButton *cancelButton = new QPushButton(i18n("Cancel"));
    centerLayout->addWidget(okButton, 4, 0);
    centerLayout->addWidget(cancelButton, 4, 1);

    //---------------------------------------------

    setLayout(centerLayout);

    resize(QSize(300, 150).expandedTo(minimumSizeHint()));

    // setting initial data
    d->albumsInput->setText(d->jalbum->albumPath().toLocalFile());
    d->albumsPath = d->jalbum->albumPath();
    d->jarInput->setText(d->jalbum->jarPath().toLocalFile());
    d->jarPath    = d->jalbum->jarPath();

    connect(d->albumsInput, SIGNAL(textChanged(QString)),
            this, SLOT(slotAlbumsPathChanged(QString)));
    connect(d->albumsSearchButton, SIGNAL(clicked(bool)),
            this, SLOT(slotShowAlbumDialogClicked(bool)));
    connect(d->jarInput, SIGNAL(textChanged(QString)),
            this, SLOT(slotJarPathChanged(QString)));
    connect(d->jarSearchButton, SIGNAL(clicked(bool)),
            this, SLOT(slotShowJarDialogClicked(bool)));
    connect(okButton, SIGNAL(clicked()),
            this, SLOT(slotOk()));
    connect(cancelButton, SIGNAL(clicked()),
            this, SLOT(reject()));
}

JAlbumEdit::~JAlbumEdit()
{
    delete d;
}

void JAlbumEdit::slotShowAlbumDialogClicked(bool checked)
{
    Q_UNUSED(checked);

    d->albumsDialog = new QFileDialog(this, QLatin1String(""), QUrl().toString(), QLatin1String("*"));
    d->albumsDialog->setFileMode(QFileDialog::Directory);
    d->albumsDialog->setWindowTitle(i18n("Select Albums Location"));
    d->albumsDialog->setAcceptMode(QFileDialog::AcceptOpen);
    d->albumsDialog->setDirectoryUrl(d->albumsPath);
    d->albumsDialog->setOptions(QFileDialog::ShowDirsOnly);

    if (d->albumsDialog->exec() == QFileDialog::Accepted)
    {
        d->albumsPath.setPath(QDir::toNativeSeparators(d->albumsDialog->selectedUrls().first().toLocalFile()));
        updateAlbumsPath();
    }

    delete d->albumsDialog;
}

void JAlbumEdit::slotShowJarDialogClicked(bool checked)
{
    Q_UNUSED(checked);

    d->jarDialog = new QFileDialog(this, QLatin1String(""), QUrl().toString(), QLatin1String("*"));
    d->jarDialog->setFileMode(QFileDialog::ExistingFile);
    d->jarDialog->setWindowTitle(i18n("Select jar File Location"));
//    d->jarDialog->setOperationMode(QFileDialog::Other);
    d->jarDialog->setDirectoryUrl(QUrl(d->jarPath.path()));
    d->jarDialog->selectFile(d->jarPath.fileName());

    if (d->jarDialog->exec() == QFileDialog::Accepted)
    {
        d->jarPath.setPath(QDir::toNativeSeparators(d->jarDialog->selectedUrls().first().toLocalFile()));
        updateJarPath();
    }

    delete d->jarDialog;
}

void JAlbumEdit::updateAlbumsPath()
{
    QString urlstring;

    if (d->albumsPath.isValid())
    {
        urlstring = d->albumsPath.toLocalFile();
        d->albumsInput->setText(urlstring);
    }
}

void JAlbumEdit::updateJarPath()
{
    QString urlstring;

    if (d->jarPath.isValid())
    {
        urlstring = d->jarPath.toLocalFile();
        d->jarInput->setText(urlstring);
    }
}

void JAlbumEdit::slotAlbumsPathChanged(const QString& path)
{
    d->albumsPath.setPath(QDir::toNativeSeparators(path));
}

void JAlbumEdit::slotJarPathChanged(const QString& path)
{
    d->jarPath.setPath(QDir::toNativeSeparators(path));
}

void JAlbumEdit::slotOk()
{
    QDir albumsDir = QDir(d->albumsPath.path());

    if (!albumsDir.exists())
    {
        if (QFile::exists(d->albumsPath.path()))
        {
            QMessageBox::information(this, i18n("Not a directory"), i18n("Chosen path is not a directory"));
            return;
        }
        else if (QMessageBox::warning(this,
                    i18n("Missing directory"),
                    i18n("Directory %1 does not exist, do you wish to create it?",
                        QDir::toNativeSeparators(d->albumsPath.path())),
                    QMessageBox::Yes | QMessageBox::No)
                            == QMessageBox::No)
        {
            return;
        }

        if (!albumsDir.mkpath(d->albumsPath.path()))
        {
            QMessageBox::information(this,
                    i18n("Failed to create directory"),
                    i18n("Failed to create directory"));
            return;
        }
    }

    d->jalbum->setPath(d->albumsPath.toLocalFile());
    d->jalbum->setJar(d->jarPath.toLocalFile());
    d->jalbum->save();
    accept();
}

} // namespace KIPIJAlbumExportPlugin
