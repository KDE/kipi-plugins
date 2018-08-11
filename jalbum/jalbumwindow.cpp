/* ============================================================
 *
 * This file is a part of KDE project
 *
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

#include "jalbumwindow.h"

// C ANSI includes

#include <sys/stat.h>

// Qt includes

#include <QCloseEvent>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTextStream>
#include <QProcess>
#include <QTreeWidgetItem>
#include <QPointer>
#include <QSpacerItem>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QApplication>

// KDE includes

#include <klocalizedstring.h>
#include <kwindowconfig.h>
#include <kconfig.h>

// Libkipi includes

#include <KIPI/Interface>
#include <KIPI/ImageCollection>

// Local includes

#include "jalbum.h"
#include "jalbumconfig.h"
#include "kputil.h"
#include "kpimagedialog.h"
#include "kpaboutdata.h"
#include "kpimageinfo.h"
#include "kipiplugins_debug.h"

namespace KIPIJAlbumExportPlugin
{

class JAlbumWindow::Private
{
public:

    Private(JAlbumWindow* const parent);

    QWidget*   widget;
    JAlbum*    jalbum;
    QLineEdit* albumName;
};

JAlbumWindow::Private::Private(JAlbumWindow* const parent)
{
    jalbum = 0;
    widget = new QWidget(parent);

    parent->setMainWidget(widget);
    parent->setModal(false);

//    QFrame* const optionFrame = new QFrame;
    QVBoxLayout* const vlay    = new QVBoxLayout();

    QLabel* const albumLabel   = new QLabel(i18n("jAlbum Album name to export to:"));
    vlay->addWidget(albumLabel);

    albumName                  = new QLineEdit();
    vlay->addWidget(albumName);

    widget->setLayout(vlay);
}

// --------------------------------------------------------------------------------------------------------------

JAlbumWindow::JAlbumWindow(QWidget* const parent, JAlbum* const pJAlbum)
    : KPToolDialog(parent),
      d(new Private(this))
{
    d->jalbum = pJAlbum;

    // User1 Button : to conf jalbum settings
    QPushButton* const confButton = new QPushButton(i18n("Settings"));
    confButton->setIcon( QIcon::fromTheme(QLatin1String("configure")));
    addButton(confButton, QDialogButtonBox::ApplyRole);

    connect(confButton, SIGNAL(clicked(bool)),
            this, SLOT(slotSettings(bool)) );

    // connect functions
    connectSignals();

    setWindowIcon(QIcon::fromTheme(QString::fromLatin1("kipi-jalbum")));
    setWindowTitle(i18n("jAlbum Export"));
    setModal(false);

    KPAboutData* const about = new KPAboutData(ki18n("jAlbum Export"),
                                               ki18n("A Kipi plugin to launch jAlbum using selected images."),
                                               ki18n("(c) 2013-2017, Andrew Goodbody\n"));

    about->addAuthor(QString::fromLatin1("Andrew Goodbody"), QString::fromLatin1("Author"),
                     QString::fromLatin1("ajg zero two at elfringham dot co dot uk"));

    about->setHandbookEntry(QString::fromLatin1("jalbum"));
    setAboutData(about);

    // read Settings
    readSettings();
}

JAlbumWindow::~JAlbumWindow()
{
    saveSettings();

    delete d;
}

void JAlbumWindow::connectSignals()
{
    connect(this, &JAlbumWindow::finished,
            this, &JAlbumWindow::slotFinished);

    connect(startButton(), &QPushButton::clicked,
            this, &JAlbumWindow::slotNewAlbum);
}

void JAlbumWindow::slotError(const QString& msg)
{
//    d->progressDlg->hide();
    QMessageBox::critical(this, i18n("Error"), msg);
}

void JAlbumWindow::slotNewAlbum()
{
    // photoPath
    const QList<QUrl> urls(iface()->currentSelection().images());

    if (urls.isEmpty())
        return; // NO photo selected: FIXME: do something

    QDir newAlbumDir = QDir(d->jalbum->albumPath().path());
    if (newAlbumDir.cd(d->albumName->text()))
    {
        if (QMessageBox::warning(this,
                    i18n("Overwrite?"),
                    i18n("Album %1 already exists, do you wish to overwrite it?", d->albumName->text()),
                    QMessageBox::Yes | QMessageBox::No)
                    == QMessageBox::No)
        {
            return;
        }
    }
    else if (!newAlbumDir.mkpath(newAlbumDir.path() + QString::fromLatin1("/") + d->albumName->text()))
    {
        QMessageBox::information(this,
                i18n("Create dir Failed"),
                i18n("Failed to create album directory"));
        qCDebug(KIPIPLUGINS_LOG) << "Failed to create album directory";
        return;
    }
    else
    {
        newAlbumDir.cd(d->albumName->text());
    }

    QFile createFile(newAlbumDir.filePath(QString::fromLatin1("albumfiles.txt")));
    if (!createFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::information(this,
                i18n("Writing Failed"),
                i18n("Could not open 'albumfiles.txt' for writing"));
        qCDebug(KIPIPLUGINS_LOG) << "Could not open 'albumfiles.txt' for writing";
        return;
    }

    QTextStream out(&createFile);
    for (QList<QUrl>::ConstIterator it = urls.constBegin(); it != urls.constEnd(); ++it)
    {
        out << (*it).fileName().toLocal8Bit().data() << "\t" << (*it).path().toLocal8Bit().data() << "\n";
    }

    createFile.close();

    QFile settingsFile(newAlbumDir.filePath(QString::fromLatin1("jalbum-settings.jap")));
    if (!settingsFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::information(this,
                i18n("Writing Failed"),
                i18n("Could not open 'jalbum-settings.jap' for writing"));
        qCDebug(KIPIPLUGINS_LOG) << "Could not open 'jalbum-settings.jap' for writing";
        return;
    }

    QTextStream out2(&settingsFile);
    out2 << "#jAlbum Project\n";

    settingsFile.close();

    QString javaExecutable;
    QDir jrePath = QFileInfo(d->jalbum->jarPath().path()).dir();
    if (jrePath.cd(QString::fromLatin1("jre64/bin/")))
    {
        javaExecutable = jrePath.filePath(QString::fromLatin1("java"));
    }
    else
    {
        javaExecutable = QString::fromLatin1("java");
    }

    QStringList args;
    args.append(QString::fromLatin1("-Xmx400M"));
    args.append(QString::fromLatin1("-jar"));
    args.append(d->jalbum->jarPath().path());
    args.append(QDir::toNativeSeparators(newAlbumDir.filePath(QString::fromLatin1("jalbum-settings.jap"))));

    QProcess process;
    process.setProcessEnvironment(adjustedEnvironmentForAppImage());
    process.startDetached(javaExecutable, args);
    accept();
}

void JAlbumWindow::slotSettings(bool clicked)
{
    Q_UNUSED(clicked);
    QPointer<JAlbumEdit> dlg = new JAlbumEdit(QApplication::activeWindow(), d->jalbum, i18n("Edit jAlbum Data") );

    if( dlg->exec() == QDialog::Accepted )
    {
        // should do something to validate something here       slotDoLogin();
    }

    delete dlg;
}

void JAlbumWindow::closeEvent(QCloseEvent* e)
{
    if (!e)
    {
        return;
    }

    slotFinished();
    e->accept();
}

void JAlbumWindow::slotFinished()
{
    saveSettings();
}

void JAlbumWindow::readSettings()
{
    // read Config
    KConfig config(QString::fromLatin1("kipirc"));
    KConfigGroup group = config.group("jAlbum Album");

    d->albumName->setText(group.readEntry("Album Name", QString()));
}

void JAlbumWindow::saveSettings()
{
    // write Config
    KConfig config(QString::fromLatin1("kipirc"));
    KConfigGroup group = config.group("jAlbum Album");

    group.writeEntry("Album Name", d->albumName->text());
    config.sync();
}

} // namespace KIPIJAlbumExportPlugin
