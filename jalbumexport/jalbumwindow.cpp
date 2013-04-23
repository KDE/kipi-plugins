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

#include "jalbumwindow.moc"

// Qt includes

#include <QCheckBox>
#include <QDialog>
#include <QDir>
#include <QFileInfo>
#include <QGroupBox>
#include <QPushButton>
#include <QSpinBox>
#include <QProcess>
#include <Qt>
#include <QTreeWidgetItem>
#include <QPointer>
#include <QSpacerItem>
#include <QtGui/QHBoxLayout>

// KDE includes

#include <kaboutdata.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <khelpmenu.h>
#include <kicon.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <krun.h>
#include <ktoolinvocation.h>
#include <kurllabel.h>
#include <kstandarddirs.h>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>

// Local includes

#include "jalbum.h"
#include "jalbumconfig.h"
#include "kpimagedialog.h"
#include "kpaboutdata.h"
#include "kpimageinfo.h"

namespace KIPIJAlbumExportPlugin
{

class JAlbumWindow::Private
{
public:

    Private(JAlbumWindow* const parent);

    QWidget*        widget;
    QPushButton*    newAlbumBtn;
    JAlbum*         jalbum;
    KLineEdit*      albumName;
};

JAlbumWindow::Private::Private(JAlbumWindow* const parent)
{
    jalbum = 0;
    widget = new QWidget(parent);

    parent->setMainWidget(widget);
    parent->setModal(false);

//    QFrame* const optionFrame = new QFrame;
    QVBoxLayout* const vlay   = new QVBoxLayout();

    QLabel* const albumLabel   = new QLabel(i18n("jAlbum Album name to export to:"));
    vlay->addWidget(albumLabel);

    albumName = new KLineEdit();
    vlay->addWidget(albumName);

    newAlbumBtn = new QPushButton;
    newAlbumBtn->setText(i18n("&Export"));
    newAlbumBtn->setIcon(KIcon("folder-new"));
    newAlbumBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    newAlbumBtn->setEnabled(true);

    // ---------------------------------------------------------------------------

    vlay->addWidget(newAlbumBtn);
//    optionFrame->setLayout(vlay);
    widget->setLayout(vlay);
}

// --------------------------------------------------------------------------------------------------------------

JAlbumWindow::JAlbumWindow(QWidget* const parent, JAlbum* const pJAlbum)
    : KPToolDialog(parent),
      d(new Private(this))
{
    d->jalbum = pJAlbum;

    setWindowTitle( i18n("jAlbum Export") );
    setButtons( KDialog::Close | KDialog::User1 | KDialog::Help);
    setModal(false);

    KPAboutData* const about = new KPAboutData(ki18n("jAlbum Export"),
                                               0,
                                               KAboutData::License_GPL,
                                               ki18n("A Kipi plugin to launch jAlbum using selected images."),
                                               ki18n("(c) 2013, Andrew Goodbody\n"));

    about->addAuthor(ki18n("Andrew Goodbody"), ki18n("Author"),
                     "ajg zero two at elfringham dot co dot uk");

    about->setHandbookEntry("jalbumexport");
    setAboutData(about);

    // User1 Button : to conf jalbum settings
    KPushButton* const confButton = button( User1 );
    confButton->setText( i18n("Settings") );
    confButton->setIcon( KIcon("configure") );

    connect(confButton, SIGNAL(clicked()),
            this, SLOT(slotSettings()) );

    // connect functions
    connectSignals();

    // read Settings
    readSettings();
}

JAlbumWindow::~JAlbumWindow()
{
    // write config
    KConfig config("kipirc");
    KConfigGroup group = config.group("jAlbum Album");

    group.writeEntry("Album Name",   d->albumName->text());

//    delete d->uploadList;

    delete d;
}

void JAlbumWindow::connectSignals()
{
    connect(d->newAlbumBtn, SIGNAL(clicked()),
            this, SLOT(slotNewAlbum()));
}

void JAlbumWindow::readSettings()
{
    // read Config
    KConfig config("kipirc");
    KConfigGroup group = config.group("jAlbum Album");

    d->albumName->setText(group.readEntry("Album Name", ""));
}

void JAlbumWindow::slotError(const QString& msg)
{
//    d->progressDlg->hide();
    KMessageBox::error(this, msg);
}

void JAlbumWindow::slotNewAlbum()
{
    QString newAlbumPath;
    QString destFile;

    // photoPath
    const KUrl::List urls(iface()->currentSelection().images());

    if (urls.isEmpty())
        return; // NO photo selected: FIXME: do something

    newAlbumPath = d->jalbum->albumPath().path() + QDir::separator() + d->albumName->text();

    struct stat stbuf;

    if (::stat(newAlbumPath.toLocal8Bit().data(), &stbuf) == 0)
    {
        if (KMessageBox::warningYesNo(this,
                     i18n("Album %1 already exists, do you wish to overwrite it?", d->albumName->text()))
                     == KMessageBox::No)
        {
            return;
        }
    }

    if (!JAlbum::createDir(newAlbumPath))
    {
        KMessageBox::information(this,
                 i18n("Failed to create album directory"));
        kDebug() << "Failed to create album directory";
        return;
    }

    destFile   = newAlbumPath + QDir::separator() + "albumfiles.txt";
    FILE* file = fopen(destFile.toLocal8Bit().data(), "w");

    if (!file)
    {
        KMessageBox::information(this,
                i18n("Could not open 'albumfiles.txt' for writing"));
        kDebug() << "Could not open 'albumfiles.txt' for writing";
        return;
    }

    for (KUrl::List::ConstIterator it = urls.constBegin(); it != urls.constEnd(); ++it)
    {
        fprintf(file, "%s\t%s\n", (*it).fileName().toLocal8Bit().data(), (*it).path().toLocal8Bit().data() );
    }

    fclose(file);

    destFile = newAlbumPath + QDir::separator() + "jalbum-settings.jap";
    file     = fopen(destFile.toLocal8Bit().data(), "w");

    if (!file)
    {
        KMessageBox::information(this,
                i18n("Could not open 'jalbum-settings.jap' for writing"));
        kDebug() << "Could not open 'jalbum-settings.jap' for writing";
        return;
    }

    fprintf(file, "#jAlbum Project\n");

    fclose(file);

    QStringList args;
    args.append("-Xmx400M");
    args.append("-jar");
    args.append(d->jalbum->jarPath().path());
    args.append(destFile);
    QProcess::startDetached("java", args);
}

void JAlbumWindow::slotSettings()
{
    QPointer<JAlbumEdit> dlg = new JAlbumEdit(kapp->activeWindow(), d->jalbum, i18n("Edit jAlbum Data") );

    if( dlg->exec() == QDialog::Accepted )
    {
        // should do something to validate something here       slotDoLogin();
    }

    delete dlg;
}

} // namespace KIPIJAlbumExportPlugin
