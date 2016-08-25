/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2014-09-30
 * Description : a plugin to export to a remote Piwigo server.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006      by Colin Guthrie <kde at colin dot guthr dot ie>
 * Copyright (C) 2006-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008      by Andrea Diamantini <adjam7 at gmail dot com>
 * Copyright (C) 2010-2014 by Frederic Coiffier <frederic dot coiffier at free dot com>
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

#include "piwigowindow.h"

// Qt includes

#include <QCheckBox>
#include <QDialog>
#include <QFileInfo>
#include <QBuffer>
#include <QImage>
#include <QGroupBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTreeWidgetItem>
#include <QPointer>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextStream>
#include <QFile>
#include <QProgressDialog>
#include <QApplication>
#include <QIcon>
#include <QMenu>
#include <QMessageBox>
#include <QLabel>

// KDE includes

#include <kconfig.h>
#include <klocalizedstring.h>
#include <kconfiggroup.h>

// Libkipi includes

#include <KIPI/Interface>
#include <KIPI/ImageCollection>

// Local includes

#include "kipiplugins_debug.h"
#include "piwigos.h"
#include "piwigoconfig.h"
#include "piwigoitem.h"
#include "piwigotalker.h"
#include "kpimagedialog.h"
#include "kpaboutdata.h"

namespace KIPIPiwigoExportPlugin
{

class PiwigoWindow::Private
{
public:

    Private(PiwigoWindow* const parent);

    QWidget*                       widget;

    QTreeWidget*                   albumView;

    QPushButton*                   confButton;

    QCheckBox*                     resizeCheckBox;
    QSpinBox*                      widthSpinBox;
    QSpinBox*                      heightSpinBox;
    QSpinBox*                      qualitySpinBox;

    QHash<QString, GAlbum>         albumDict;

    PiwigoTalker*                  talker;
    Piwigo*                        pPiwigo;

    QProgressDialog*               progressDlg;
    unsigned int                   uploadCount;
    unsigned int                   uploadTotal;
    QStringList*                   pUploadList;
};

PiwigoWindow::Private::Private(PiwigoWindow* const parent)
{
    talker      = 0;
    pPiwigo     = 0;
    progressDlg = 0;
    uploadCount = 0;
    uploadTotal = 0;
    pUploadList = 0;
    widget      = new QWidget(parent);
    parent->setMainWidget(widget);
    parent->setModal(false);

    QHBoxLayout* const hlay = new QHBoxLayout(widget);

    // ---------------------------------------------------------------------------

    QLabel* const logo = new QLabel();
    logo->setContentsMargins(QMargins());
    logo->setScaledContents(false);
    logo->setOpenExternalLinks(true);
    logo->setTextFormat(Qt::RichText);
    logo->setFocusPolicy(Qt::NoFocus);
    logo->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
    logo->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
    logo->setToolTip(i18n("Visit Piwigo website"));
    logo->setAlignment(Qt::AlignLeft);
    QImage img = QImage(QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                               QLatin1String("kipiplugin_piwigo/pics/piwigo_logo.png")));

    QByteArray byteArray;
    QBuffer    buffer(&byteArray);
    img.save(&buffer, "PNG");
    logo->setText(QString::fromLatin1("<a href=\"%1\">%2</a>")
                  .arg(QLatin1String("http://piwigo.org"))
                  .arg(QString::fromLatin1("<img src=\"data:image/png;base64,%1\">")
                  .arg(QString::fromLatin1(byteArray.toBase64().data()))));

    // ---------------------------------------------------------------------------

    albumView = new QTreeWidget;
    QStringList labels;
    labels << i18n("Albums");
    albumView->setHeaderLabels(labels);

    // ---------------------------------------------------------------------------

    QFrame* const optionFrame = new QFrame;
    QVBoxLayout* const vlay   = new QVBoxLayout();

    confButton = new QPushButton;
    confButton->setText(i18n("Change Account"));
    confButton->setIcon(QIcon::fromTheme(QString::fromLatin1("system-switch-user")));
    confButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QGroupBox* const optionsBox = new QGroupBox(i18n("Options"));
    QVBoxLayout* const vlay2    = new QVBoxLayout();

    resizeCheckBox           = new QCheckBox(optionsBox);
    resizeCheckBox->setText(i18n("Resize photos before uploading"));

    QGridLayout* const glay  = new QGridLayout;
    QLabel* const widthLabel = new QLabel(i18n("Maximum width:"));

    widthSpinBox   = new QSpinBox;
    widthSpinBox->setRange(1,8000);
    widthSpinBox->setValue(1600);

    QLabel* const heightLabel   = new QLabel(i18n("Maximum height:"));

    heightSpinBox  = new QSpinBox;
    heightSpinBox->setRange(1,8000);
    heightSpinBox->setValue(1600);

    QLabel* const qualityLabel= new QLabel(i18n("Resized JPEG quality:"));

    qualitySpinBox = new QSpinBox;
    qualitySpinBox->setRange(1,100);
    qualitySpinBox->setValue(95);

    resizeCheckBox->setChecked(false);
    widthSpinBox->setEnabled(false);
    heightSpinBox->setEnabled(false);
    qualitySpinBox->setEnabled(false);

    // ---------------------------------------------------------------------------

    glay->addWidget(widthLabel,     0, 0);
    glay->addWidget(widthSpinBox,   0, 1);
    glay->addWidget(heightLabel,    1, 0);
    glay->addWidget(heightSpinBox,  1, 1);
    glay->addWidget(qualityLabel,   2, 0);
    glay->addWidget(qualitySpinBox, 2, 1);

    // ---------------------------------------------------------------------------

    vlay2->addWidget(resizeCheckBox);
    vlay2->addLayout(glay);
    vlay2->addStretch(0);

    optionsBox->setLayout(vlay2);

    // ---------------------------------------------------------------------------

    vlay->addWidget(confButton);
    vlay->addWidget(optionsBox);

    optionFrame->setLayout(vlay);

    // ---------------------------------------------------------------------------

    hlay->addWidget(logo);
    hlay->addWidget(albumView);
    hlay->addWidget(optionFrame);

    widget->setLayout(hlay);
}

// --------------------------------------------------------------------------------------------------------------

PiwigoWindow::PiwigoWindow(QWidget* const parent, Piwigo* const pPiwigo)
    : KPToolDialog(parent),
      d(new Private(this))
{
    d->pPiwigo = pPiwigo;

    setWindowTitle( i18n("Piwigo Export") );
    setModal(false);

    // About data.
    KPAboutData* about = new KPAboutData(ki18n("Piwigo Export"),
                                         ki18n("A tool to export image collections to a remote Piwigo server."),
                                         ki18n("(c) 2003-2005, Renchi Raju\n"
                                               "(c) 2006-2007, Colin Guthrie\n"
                                               "(c) 2006-2015, Gilles Caulier\n"
                                               "(c) 2008, Andrea Diamantini\n"
                                               "(c) 2010-2014, Frédéric Coiffier\n"));

    about->addAuthor(ki18n("Renchi Raju").toString(),
                     ki18n("Developer").toString(),
                     QString::fromLatin1("renchi dot raju at gmail dot com"));

    about->addAuthor(ki18n("Colin Guthrie").toString(),
                     ki18n("Developer").toString(),
                     QString::fromLatin1("kde at colin dot guthr dot ie"));

    about->addAuthor(ki18n("Andrea Diamantini").toString(),
                     ki18n("Developer").toString(),
                     QString::fromLatin1("adjam7 at gmail dot com"));

    about->addAuthor(ki18n("Gilles Caulier").toString(),
                     ki18n("Developer").toString(),
                     QString::fromLatin1("caulier dot gilles at gmail dot com"));

    about->addAuthor(ki18n("Frédéric Coiffier").toString(),
                     ki18n("Developer").toString(),
                     QString::fromLatin1("frederic dot coiffier at free dot com"));

    about->setHandbookEntry(QString::fromLatin1("tool-piwigoexport"));
    setAboutData(about);

    // "Start Upload" button
    startButton()->setText( i18n("Start Upload") );
    startButton()->setEnabled(false);

    connect(startButton(), SIGNAL(clicked()),
            this, SLOT(slotAddPhoto()));

    // we need to let d->talker work..
    d->talker      = new PiwigoTalker(d->widget);

    // setting progressDlg and its numeric hints
    d->progressDlg = new QProgressDialog(this);
    d->progressDlg->setModal(true);
    d->progressDlg->setAutoReset(true);
    d->progressDlg->setAutoClose(true);
    d->progressDlg->setMaximum(0);
    d->progressDlg->reset();

    d->pUploadList = new QStringList;

    // connect functions
    connectSignals();

    // read Settings
    readSettings();

    slotDoLogin();
}

PiwigoWindow::~PiwigoWindow()
{
    // write config
    KConfig config(QString::fromLatin1("kipirc"));
    KConfigGroup group = config.group("PiwigoSync Galleries");

    group.writeEntry("Resize",          d->resizeCheckBox->isChecked());
    group.writeEntry("Maximum Width",   d->widthSpinBox->value());
    group.writeEntry("Maximum Height",  d->heightSpinBox->value());
    group.writeEntry("Quality",         d->qualitySpinBox->value());
    group.deleteEntry("Thumbnail Width"); // Old config, no longer used

    delete d->talker;
    delete d->pUploadList;
    delete d;
}

void PiwigoWindow::connectSignals()
{
    connect(d->albumView, SIGNAL(itemSelectionChanged()),
            this , SLOT(slotAlbumSelected()) );

    connect(d->confButton, SIGNAL(clicked()),
             this, SLOT(slotSettings()) );

    connect(d->resizeCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(slotEnableSpinBox(int)));

    connect(d->progressDlg, SIGNAL(canceled()),
            this, SLOT(slotAddPhotoCancel()));

    connect(d->talker, SIGNAL(signalProgressInfo(QString)),
            this, SLOT(slotProgressInfo(QString)));

    connect(d->talker, SIGNAL(signalError(QString)),
            this, SLOT(slotError(QString)));

    connect(d->talker, SIGNAL(signalBusy(bool)),
            this, SLOT(slotBusy(bool)));

    connect(d->talker, SIGNAL(signalLoginFailed(QString)),
            this, SLOT(slotLoginFailed(QString)));

    connect(d->talker, SIGNAL(signalAlbums(QList<GAlbum>)),
            this, SLOT(slotAlbums(QList<GAlbum>)));

    connect(d->talker, SIGNAL(signalAddPhotoSucceeded()),
            this, SLOT(slotAddPhotoSucceeded()));

    connect(d->talker, SIGNAL(signalAddPhotoFailed(QString)),
            this, SLOT(slotAddPhotoFailed(QString)));
}

void PiwigoWindow::readSettings()
{
    // read Config
    KConfig config(QString::fromLatin1("kipirc"));
    KConfigGroup group = config.group("PiwigoSync Galleries");

    if (group.readEntry("Resize", false))
    {
        d->resizeCheckBox->setChecked(true);
        d->widthSpinBox->setEnabled(true);
        d->heightSpinBox->setEnabled(true);
    }
    else
    {
        d->resizeCheckBox->setChecked(false);
        d->heightSpinBox->setEnabled(false);
        d->widthSpinBox->setEnabled(false);
    }

    d->widthSpinBox->setValue(group.readEntry("Maximum Width", 1600));
    d->heightSpinBox->setValue(group.readEntry("Maximum Height", 1600));

    d->qualitySpinBox->setValue(group.readEntry("Quality", 95));
}

void PiwigoWindow::slotDoLogin()
{
    QUrl url(d->pPiwigo->url());

    if (url.scheme().isEmpty())
    {
        url.setScheme(QString::fromLatin1("http"));
        url.setHost(d->pPiwigo->url());
    }

    // If we've done something clever, save it back to the piwigo.
    if (!url.url().isEmpty() && d->pPiwigo->url() != url.url())
    {
        d->pPiwigo->setUrl(url.url());
        d->pPiwigo->save();
    }

    d->talker->login(url, d->pPiwigo->username(), d->pPiwigo->password());
}

void PiwigoWindow::slotLoginFailed(const QString& msg)
{
    if (QMessageBox::question(this, i18n("Login Failed"),
                              i18n("Failed to login into remote piwigo. ") + msg +
                              i18n("\nDo you want to check your settings and try again?"))
            != QMessageBox::Yes)
    {
        close();
        return;
    }

    QPointer<PiwigoEdit> configDlg = new PiwigoEdit(QApplication::activeWindow(), d->pPiwigo, i18n("Edit Piwigo Data") );

    if ( configDlg->exec() != QDialog::Accepted )
    {
        delete configDlg;
        return;
    }

    slotDoLogin();
    delete configDlg;
}

void PiwigoWindow::slotBusy(bool val)
{
    if (val)
    {
        setCursor(Qt::WaitCursor);
        startButton()->setEnabled(false);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
        bool loggedIn = d->talker->loggedIn();
        startButton()->setEnabled(loggedIn && d->albumView->currentItem());
    }
}

void PiwigoWindow::slotProgressInfo(const QString& msg)
{
    d->progressDlg->setLabelText(msg);
}

void PiwigoWindow::slotError(const QString& msg)
{
    d->progressDlg->hide();
    QMessageBox::critical(this, QString(), msg);
}

void PiwigoWindow::slotAlbums(const QList<GAlbum>& albumList)
{
    d->albumDict.clear();
    d->albumView->clear();

    // album work list
    QList<GAlbum> workList(albumList);
    QList<QTreeWidgetItem *> parentItemList;

    // fill QTreeWidget
    while ( !workList.isEmpty() )
    {
        // the album to work on
        GAlbum album     = workList.takeFirst();
        int parentRefNum = album.parent_ref_num;

        if ( parentRefNum == -1 )
        {
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, cleanName(album.name) );
            item->setIcon(0, QIcon::fromTheme(QString::fromLatin1("inode-directory")) );
            item->setData(1, Qt::UserRole, QVariant(album.ref_num) );
            item->setText(2, i18n("Album") );

            qCDebug(KIPIPLUGINS_LOG) << "Top : " << album.name << " " << album.ref_num << "\n";

            d->albumView->addTopLevelItem(item);
            d->albumDict.insert(album.name, album);
            parentItemList << item;
        }
        else
        {
            QTreeWidgetItem *parentItem = 0;
            bool found                  = false;
            int i                       = 0;

            while ( !found && i < parentItemList.size() )
            {
                parentItem = parentItemList.at(i);

                if (parentItem && (parentItem->data(1, Qt::UserRole).toInt() == parentRefNum))
                {
                    QTreeWidgetItem *item = new QTreeWidgetItem(parentItem);
                    item->setText(0, cleanName(album.name) );
                    item->setIcon(0, QIcon::fromTheme(QString::fromLatin1("inode-directory")) );
                    item->setData(1, Qt::UserRole, album.ref_num );
                    item->setText(2, i18n("Album") );

                    parentItem->addChild(item);
                    d->albumDict.insert(album.name, album);
                    parentItemList << item;
                    found = true;
                }

                i++;
            }
        }
    }
}

void PiwigoWindow::slotAlbumSelected()
{
    QTreeWidgetItem* const item = d->albumView->currentItem();

    // stop loading if user clicked an image
    if (item && item->text(2) == i18n("Image") )
        return;

    if (!item)
    {
        startButton()->setEnabled(false);
    }
    else
    {
        qCDebug(KIPIPLUGINS_LOG) << "Album selected\n";

        int albumId = item->data(1, Qt::UserRole).toInt();
        qCDebug(KIPIPLUGINS_LOG) << albumId << "\n";

        if (d->talker->loggedIn() && albumId )
        {
            startButton()->setEnabled(true);
        }
        else
        {
            startButton()->setEnabled(false);
        }
    }
}

void PiwigoWindow::slotAddPhoto()
{
    const QList<QUrl> urls(iface()->currentSelection().images());

    if ( urls.isEmpty())
    {
        QMessageBox::critical(this, QString(), i18n("Nothing to upload - please select photos to upload."));
        return;
    }

    for (QList<QUrl>::const_iterator it = urls.constBegin(); it != urls.constEnd(); ++it)
    {
        d->pUploadList->append( (*it).toLocalFile() );
    }

    d->uploadTotal = d->pUploadList->count();
    d->progressDlg->reset();
    d->progressDlg->setMaximum(d->uploadTotal);
    d->uploadCount = 0;
    slotAddPhotoNext();
}

void PiwigoWindow::slotAddPhotoNext()
{
    if ( d->pUploadList->isEmpty() )
    {
        d->progressDlg->reset();
        d->progressDlg->hide();
        return;
    }

    QTreeWidgetItem* const item = d->albumView->currentItem();
    int column                  = d->albumView->currentColumn();
    QString albumTitle          = item->text(column);
    const GAlbum& album         = d->albumDict.value(albumTitle);
    QString photoPath           = d->pUploadList->takeFirst();
    bool res                    = d->talker->addPhoto(album.ref_num, photoPath,
                                                      d->resizeCheckBox->isChecked(),
                                                      d->widthSpinBox->value(),
                                                      d->heightSpinBox->value(),
                                                      d->qualitySpinBox->value() );

    if (!res)
    {
        slotAddPhotoFailed( i18n("The file %1 is not a supported image or video format", QUrl(photoPath).fileName()) );
        return;
    }

    d->progressDlg->setLabelText( i18n("Uploading file %1", QUrl(photoPath).fileName()) );

    if (d->progressDlg->isHidden())
        d->progressDlg->show();
}

void PiwigoWindow::slotAddPhotoSucceeded()
{
    d->uploadCount++;
    d->progressDlg->setValue(d->uploadCount);
    slotAddPhotoNext();
}

void PiwigoWindow::slotAddPhotoFailed(const QString& msg)
{
    d->progressDlg->reset();
    d->progressDlg->hide();

    if (QMessageBox::question(this, i18n("Uploading Failed"),
                              i18n("Failed to upload media into remote Piwigo. ") + msg +
                              i18n("\nDo you want to continue?"))
            != QMessageBox::Yes)
    {
        return;
    }
    else
    {
        slotAddPhotoNext();
    }
}

void PiwigoWindow::slotAddPhotoCancel()
{
    d->progressDlg->reset();
    d->progressDlg->hide();
    d->talker->cancel();
}

void PiwigoWindow::slotEnableSpinBox(int n)
{
    bool b;

    switch (n)
    {
        case 0:
            b = false;
            break;
        case 1:
        case 2:
            b = true;
            break;
        default:
            b = false;
            break;
    }

    d->widthSpinBox->setEnabled(b);
    d->heightSpinBox->setEnabled(b);
    d->qualitySpinBox->setEnabled(b);
}

void PiwigoWindow::slotSettings()
{
    // TODO: reload albumlist if OK slot used.
    QPointer<PiwigoEdit> dlg = new PiwigoEdit(QApplication::activeWindow(), d->pPiwigo, i18n("Edit Piwigo Data") );

    if ( dlg->exec() == QDialog::Accepted )
    {
        slotDoLogin();
    }

    delete dlg;
}

QString PiwigoWindow::cleanName(const QString& str) const
{
    QString plain = str;
    plain.replace(QString::fromLatin1("&lt;"), QString::fromLatin1("<"));
    plain.replace(QString::fromLatin1("&gt;"), QString::fromLatin1(">"));
    plain.replace(QString::fromLatin1("&quot;"), QString::fromLatin1("\""));
    plain.replace(QString::fromLatin1("&amp;"), QString::fromLatin1("&"));

    return plain;
}

} // namespace KIPIPiwigoExportPlugin
