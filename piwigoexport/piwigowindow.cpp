/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2010-02-15
 * Description : a plugin to export to a remote Piwigo server.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006      by Colin Guthrie <kde@colin.guthr.ie>
 * Copyright (C) 2006-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008      by Andrea Diamantini <adjam7 at gmail dot com>
 * Copyright (C) 2010      by Frederic Coiffier <frederic dot coiffier at free dot com>
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

#include "piwigowindow.moc"

// Qt includes

#include <Qt>
#include <QCheckBox>
#include <QDialog>
#include <QFileInfo>
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

// KDE includes

#include <kaboutdata.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
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

    QWidget*               widget;

    QTreeWidget*           albumView;

    QPushButton*           confButton;

    QCheckBox*             resizeCheckBox;
    QSpinBox*              widthSpinBox;
    QSpinBox*              heightSpinBox;
    QSpinBox*              thumbDimensionSpinBox;

    QHash<QString, GAlbum> albumDict;

    KUrlLabel*             logo;

    PiwigoTalker*          talker;
    Piwigo*                pPiwigo;

    QProgressDialog*       progressDlg;
    unsigned int           uploadCount;
    unsigned int           uploadTotal;
    QStringList*           pUploadList;

};

PiwigoWindow::Private::Private(PiwigoWindow* const parent)
{
    widget = new QWidget(parent);
    parent->setMainWidget(widget);
    parent->setModal(false);

    QHBoxLayout* hlay = new QHBoxLayout(widget);

    // ---------------------------------------------------------------------------

    logo = new KUrlLabel;
    logo->setText(QString());
    logo->setUrl("http://piwigo.org");
    logo->setPixmap(QPixmap(KStandardDirs::locate("data", "kipiplugin_piwigoexport/pics/piwigo_logo.png")));
    logo->setAlignment(Qt::AlignLeft);

    // ---------------------------------------------------------------------------

    albumView = new QTreeWidget;
    QStringList labels;
    labels << i18n("Albums");
    albumView->setHeaderLabels(labels);

    // ---------------------------------------------------------------------------

    QFrame* optionFrame = new QFrame;
    QVBoxLayout* vlay   = new QVBoxLayout();

    confButton = new QPushButton;
    confButton->setText(i18n("Change Account"));
    confButton->setIcon(KIcon("system-switch-user"));
    confButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QGroupBox* optionsBox = new QGroupBox(i18n("Options"));
    QVBoxLayout* vlay2    = new QVBoxLayout();

    resizeCheckBox        = new QCheckBox(optionsBox);
    resizeCheckBox->setText(i18n("Resize photos before uploading"));

    QGridLayout* glay     = new QGridLayout;
    QLabel* widthLabel    = new QLabel(i18n("Maximum width:"));

    widthSpinBox          = new QSpinBox;
    widthSpinBox->setRange(1,1600);
    widthSpinBox->setValue(800);

    QLabel *heightLabel   = new QLabel(i18n("Maximum height:"));

    heightSpinBox         = new QSpinBox;
    heightSpinBox->setRange(1,1600);
    heightSpinBox->setValue(600);

    QHBoxLayout* hlay2    = new QHBoxLayout;
    QLabel* resizeThumbLabel= new QLabel(i18n("Maximum thumbnail dimension:"));

    thumbDimensionSpinBox = new QSpinBox;
    thumbDimensionSpinBox->setRange(32,800);
    thumbDimensionSpinBox->setValue(128);
    thumbDimensionSpinBox->setToolTip(i18n("Thumbnail size is ignored with Piwigo > 2.4"));

    resizeCheckBox->setChecked(false);
    widthSpinBox->setEnabled(false);
    heightSpinBox->setEnabled(false);
    thumbDimensionSpinBox->setEnabled(true);

    // ---------------------------------------------------------------------------

    glay->addWidget(widthLabel, 0, 0);
    glay->addWidget(widthSpinBox, 0, 1);
    glay->addWidget(heightLabel, 1, 0);
    glay->addWidget(heightSpinBox, 1, 1);
    glay->setSpacing(spacingHint());
    glay->setMargin(spacingHint());

    // ---------------------------------------------------------------------------

    hlay2->addWidget(resizeThumbLabel);
    hlay2->addWidget(thumbDimensionSpinBox);
    hlay2->setSpacing(spacingHint());
    hlay2->setMargin(spacingHint());

    // ---------------------------------------------------------------------------

    vlay2->addWidget(resizeCheckBox);
    vlay2->addLayout(glay);
    vlay2->addLayout(hlay2);
    vlay2->addStretch(0);
    vlay2->setSpacing(spacingHint());
    vlay2->setMargin(spacingHint());

    optionsBox->setLayout(vlay2);

    // ---------------------------------------------------------------------------

    vlay->addWidget(confButton);
    vlay->addWidget(optionsBox);
    vlay->setSpacing(spacingHint());
    vlay->setMargin(spacingHint());

    optionFrame->setLayout(vlay);

    // ---------------------------------------------------------------------------

    hlay->addWidget(logo);
    hlay->addWidget(albumView);
    hlay->addWidget(optionFrame);
    hlay->setSpacing(spacingHint());
    hlay->setMargin(spacingHint());

    widget->setLayout(hlay);
}

// --------------------------------------------------------------------------------------------------------------

PiwigoWindow::PiwigoWindow(QWidget* const parent, Piwigo* const pPiwigo)
    : KPToolDialog(parent), 
      d(new Private(this))
{
    d->pPiwigo = pPiwigo;

    setWindowTitle( i18n("Piwigo Export") );
    setButtons(Close | User1 | Help);
    setModal(false);

    // About data.
    KPAboutData* about = new KPAboutData(ki18n("Piwigo Export"),
                                         0,
                                         KAboutData::License_GPL,
                                         ki18n("A Kipi plugin to export image collections to a remote Piwigo server."),
                                         ki18n("(c) 2003-2005, Renchi Raju\n"
                                               "(c) 2006-2007, Colin Guthrie\n"
                                               "(c) 2006-2012, Gilles Caulier\n"
                                               "(c) 2008, Andrea Diamantini\n"
                                               "(c) 2012, Frédéric Coiffier\n"));

    about->addAuthor(ki18n("Renchi Raju"), ki18n("Author"),
                     "renchi dot raju at gmail dot com");

    about->addAuthor(ki18n("Colin Guthrie"), ki18n("Maintainer"),
                     "kde at colin dot guthr dot ie");

    about->addAuthor(ki18n("Andrea Diamantini"), ki18n("Developer"),
                     "adjam7 at gmail dot com");

    about->addAuthor(ki18n("Gilles Caulier"), ki18n("Developer"),
                     "caulier dot gilles at gmail dot com");

    about->addAuthor(ki18n("Frédéric Coiffier"), ki18n("Developer"),
                     "fcoiffie at gmail dot com");

    about->setHandbookEntry("piwigoexport");
    setAboutData(about);

    // User1 Button : to upload selected photos
    KPushButton* addPhotoBtn = button( User1 );
    addPhotoBtn->setText( i18n("Start Upload") );
    addPhotoBtn->setIcon( KIcon("network-workgroup") );
    addPhotoBtn->setEnabled(false);
    connect(addPhotoBtn, SIGNAL(clicked()),
            this, SLOT(slotAddPhoto()));

    // we need to let d->talker work..
    d->talker = new PiwigoTalker(d->widget);

    // setting progressDlg and its numeric hints
    d->progressDlg = new QProgressDialog(this);
    d->progressDlg->setModal(true);
    d->progressDlg->setAutoReset(true);
    d->progressDlg->setAutoClose(true);
    d->uploadCount = 0;
    d->uploadTotal = 0;
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
    KConfig config("kipirc");
    KConfigGroup group = config.group("PiwigoSync Galleries");

    group.writeEntry("Resize",          d->resizeCheckBox->isChecked());
    group.writeEntry("Maximum Width",   d->widthSpinBox->value());
    group.writeEntry("Maximum Height",  d->heightSpinBox->value());
    group.writeEntry("Thumbnail Width", d->thumbDimensionSpinBox->value());

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

    connect(d->logo, SIGNAL(leftClickedUrl(QString)),
            this, SLOT(slotProcessUrl(QString)));

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

void PiwigoWindow::slotProcessUrl(const QString& url)
{
    KToolInvocation::self()->invokeBrowser(url);
}

void PiwigoWindow::readSettings()
{
    // read Config
    KConfig config("kipirc");
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

    d->widthSpinBox->setValue(group.readEntry("Maximum Width", 800));
    d->heightSpinBox->setValue(group.readEntry("Maximum Height", 600));

    d->thumbDimensionSpinBox->setValue(group.readEntry("Thumbnail Width", 128));
}

void PiwigoWindow::slotDoLogin()
{
    KUrl url(d->pPiwigo->url());
    if (url.protocol().isEmpty())
    {
        url.setProtocol("http");
        url.setHost(d->pPiwigo->url());
    }

    // If we've done something clever, save it back to the piwigo.
    if (!url.url().isEmpty() && d->pPiwigo->url() != url.url())
    {
        d->pPiwigo->setUrl(url.url());
        d->pPiwigo->save();
    }

    d->talker->login(url.url(), d->pPiwigo->username(), d->pPiwigo->password());
}

void PiwigoWindow::slotLoginFailed(const QString& msg)
{
    if (KMessageBox::warningYesNo(this,
                                  i18n("Failed to login into remote piwigo. ")
                                  + msg
                                  + i18n("\nDo you want to check your settings and try again?"))
            != KMessageBox::Yes)
    {
        close();
        return;
    }

    QPointer<PiwigoEdit> configDlg = new PiwigoEdit(kapp->activeWindow(), d->pPiwigo, i18n("Edit Piwigo Data") );
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
        button( User1 )->setEnabled(false);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
        bool loggedIn = d->talker->loggedIn();
        button( User1 )->setEnabled(loggedIn && d->albumView->currentItem());
    }
}

void PiwigoWindow::slotProgressInfo(const QString& msg)
{
    d->progressDlg->setLabelText(msg);
}

void PiwigoWindow::slotError(const QString& msg)
{
    d->progressDlg->hide();
    KMessageBox::error(this, msg);
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
        GAlbum album = workList.takeFirst();

        int parentRefNum = album.parent_ref_num;
        if ( parentRefNum == -1 )
        {
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, cleanName(album.name) );
            item->setIcon(0, KIcon("inode-directory") );
            item->setData(1, Qt::UserRole, QVariant(album.ref_num) );
            item->setText(2, i18n("Album") );

            kDebug() << "Top : " << album.name << " " << album.ref_num << "\n";

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
                    item->setIcon(0, KIcon("inode-directory") );
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
    QTreeWidgetItem* item = d->albumView->currentItem();
    if (!item)
    {
        return;
    }

    // stop loading if user clicked an image
    if ( item->text(2) == i18n("Image") )
        return;

    if (!item)
    {
        button( User1 )->setEnabled(false);
    }
    else
    {
        kDebug() << "Album selected\n";

        int albumId = item->data(1, Qt::UserRole).toInt();
        kDebug() << albumId << "\n";
        if (d->talker->loggedIn() && albumId )
        {
            button( User1 )->setEnabled(true);
        }
        else
        {
            button( User1 )->setEnabled(false);
        }
    }
}

void PiwigoWindow::slotAddPhoto()
{
    const KUrl::List urls(iface()->currentSelection().images());

    if ( urls.isEmpty())
    {
        KMessageBox::error(this, i18n("Nothing to upload - please select photos to upload."));
        return;
    }

    for (KUrl::List::const_iterator it = urls.constBegin(); it != urls.constEnd(); ++it)
    {
        d->pUploadList->append( (*it).path() );
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

    QTreeWidgetItem* item = d->albumView->currentItem();
    int column            = d->albumView->currentColumn();
    QString albumTitle    = item->text(column);
    const GAlbum& album   = d->albumDict.value(albumTitle);
    QString photoPath     = d->pUploadList->takeFirst();
    bool res              = d->talker->addPhoto(album.ref_num, photoPath,
                            d->resizeCheckBox->isChecked(),
                            d->widthSpinBox->value(),
                            d->heightSpinBox->value(),
                            d->thumbDimensionSpinBox->value() );

    if (!res)
    {
        slotAddPhotoFailed( "" );
        return;
    }

    d->progressDlg->setLabelText( i18n("Uploading file %1", KUrl(photoPath).fileName()) );

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

    if (KMessageBox::warningContinueCancel(this,
                                           i18n("Failed to upload photo into "
                                                "remote piwigo. ")
                                           + msg
                                           + i18n("\nDo you want to continue?"))
            != KMessageBox::Continue)
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
}

void PiwigoWindow::slotSettings()
{
    // TODO: reload albumlist if OK slot used.
    QPointer<PiwigoEdit> dlg = new PiwigoEdit(kapp->activeWindow(), d->pPiwigo, i18n("Edit Piwigo Data") );
    if ( dlg->exec() == QDialog::Accepted )
    {
        slotDoLogin();
    }
    delete dlg;
}

QString PiwigoWindow::cleanName(const QString& str) const
{
    QString plain = str;
    plain.replace("&lt;", "<");
    plain.replace("&gt;", ">");
    plain.replace("&quot;", "\"");
    plain.replace("&amp;", "&");

    return plain;
}

} // namespace KIPIPiwigoExportPlugin
