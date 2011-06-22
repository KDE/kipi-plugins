/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-02-15
 * Description : a plugin to export to a remote Piwigo server.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006 by Colin Guthrie <kde@colin.guthr.ie>
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008 by Andrea Diamantini <adjam7 at gmail dot com>
 * Copyright (C) 2010 by Frederic Coiffier <frederic dot coiffier at free dot com>
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

#include <QCheckBox>
#include <QDialog>
#include <QFileInfo>
#include <QGroupBox>
#include <QPushButton>
#include <QSpinBox>
#include <Qt>
#include <QTreeWidgetItem>
#include <QPointer>
#include <QHBoxLayout>
#include <QVBoxLayout>

// KDE includes

#include <kaboutdata.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
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

// Local includes

#include "piwigos.h"
#include "piwigoconfig.h"
#include "piwigoitem.h"
#include "piwigotalker.h"
#include "imagedialog.h"
#include "kpaboutdata.h"

namespace KIPIPiwigoExportPlugin
{

class PiwigoWindow::Private
{
public:

    Private(PiwigoWindow* parent);

    QWidget*               widget;

    QTreeWidget*           albumView;

    QPushButton*           addPhotoBtn;

    QCheckBox*             captTitleCheckBox;
    QCheckBox*             captDescrCheckBox;
    QCheckBox*             resizeCheckBox;
    QCheckBox*             downloadHQCheckBox;

    QSpinBox*              dimensionSpinBox;
    QSpinBox*              thumbDimensionSpinBox;

    QHash<QString, GAlbum> albumDict;

    KUrlLabel*             logo;
};

PiwigoWindow::Private::Private(PiwigoWindow* parent)
{
    widget = new QWidget(parent);
    parent->setMainWidget(widget);
    parent->setModal(false);

    QHBoxLayout *hlay = new QHBoxLayout(widget);

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

    QFrame *optionFrame = new QFrame;
    QVBoxLayout *vlay   = new QVBoxLayout();

    addPhotoBtn = new QPushButton;
    addPhotoBtn->setText(i18n("&Add Selected Photos"));
    addPhotoBtn->setIcon(KIcon("list-add"));
    addPhotoBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    addPhotoBtn->setEnabled(false);

    QGroupBox *optionsBox = new QGroupBox(i18n("Override Default Options"));
    QVBoxLayout *vlay2    = new QVBoxLayout();

    captTitleCheckBox     = new QCheckBox(optionsBox);
    captTitleCheckBox->setText(i18n("EXIF Comment (instead of file name) sets Title"));

    captDescrCheckBox     = new QCheckBox(optionsBox);
    captDescrCheckBox->setText(i18n("EXIF Comment (instead of file name) sets Comment"));

    resizeCheckBox        = new QCheckBox(optionsBox);
    resizeCheckBox->setText(i18n("Resize photos before uploading"));

    QHBoxLayout *hlay2    = new QHBoxLayout;
    QLabel *resizeLabel   = new QLabel(i18n("Maximum dimension:"));

    dimensionSpinBox      = new QSpinBox;
    dimensionSpinBox->setRange(1,1600);
    dimensionSpinBox->setValue(600);

    QHBoxLayout *hlay3    = new QHBoxLayout;
    QLabel *resizeThumbLabel= new QLabel(i18n("Maximum thumbnail dimension:"));

    thumbDimensionSpinBox = new QSpinBox;
    thumbDimensionSpinBox->setRange(32,800);
    thumbDimensionSpinBox->setValue(128);

    downloadHQCheckBox    = new QCheckBox(optionsBox);
    downloadHQCheckBox->setText(i18n("Also download the full size version"));

    captTitleCheckBox->setChecked(true);
    captDescrCheckBox->setChecked(false);
    resizeCheckBox->setChecked(false);
    dimensionSpinBox->setEnabled(false);
    thumbDimensionSpinBox->setEnabled(true);
    downloadHQCheckBox->setChecked(false);

    // ---------------------------------------------------------------------------

    hlay2->addWidget(resizeLabel);
    hlay2->addWidget(dimensionSpinBox);
    hlay2->setSpacing(KDialog::spacingHint());
    hlay2->setMargin(KDialog::spacingHint());

    // ---------------------------------------------------------------------------

    hlay3->addWidget(resizeThumbLabel);
    hlay3->addWidget(thumbDimensionSpinBox);
    hlay3->setSpacing(KDialog::spacingHint());
    hlay3->setMargin(KDialog::spacingHint());

    // ---------------------------------------------------------------------------

    vlay2->addWidget(captTitleCheckBox);
    vlay2->addWidget(captDescrCheckBox);
    vlay2->addWidget(resizeCheckBox);
    vlay2->addLayout(hlay2);
    vlay2->addLayout(hlay3);
    vlay2->addWidget(downloadHQCheckBox);
    vlay2->setSpacing(KDialog::spacingHint());
    vlay2->setMargin(KDialog::spacingHint());

    optionsBox->setLayout(vlay2);

    // ---------------------------------------------------------------------------

    vlay->addWidget(addPhotoBtn);
    vlay->addWidget(optionsBox);
    vlay->setSpacing(KDialog::spacingHint());
    vlay->setMargin(KDialog::spacingHint());

    optionFrame->setLayout(vlay);

    // ---------------------------------------------------------------------------

    hlay->addWidget(logo);
    hlay->addWidget(albumView);
    hlay->addWidget(optionFrame);
    hlay->setSpacing(KDialog::spacingHint());
    hlay->setMargin(KDialog::spacingHint());

    widget->setLayout(hlay);
}

// --------------------------------------------------------------------------------------------------------------

PiwigoWindow::PiwigoWindow(KIPI::Interface* interface, QWidget *parent, Piwigo* pPiwigo)
    : KDialog(parent),
      m_interface(interface),
      mpPiwigo(pPiwigo),
      d(new Private(this))
{
    setWindowTitle( i18n("Piwigo Export") );
    setButtons( KDialog::Close | KDialog::User1 | KDialog::Help);
    setModal(false);

    // About data.
    m_about = new KIPIPlugins::KPAboutData(ki18n("Piwigo Export"),
                                           0,
                                           KAboutData::License_GPL,
                                           ki18n("A Kipi plugin to export image collections to a remote Piwigo server."),
                                           ki18n("(c) 2003-2005, Renchi Raju\n"
                                                 "(c) 2006-2007, Colin Guthrie\n"
                                                 "(c) 2006-2009, Gilles Caulier\n"
                                                 "(c) 2008, Andrea Diamantini\n"
                                                 "(c) 2010, Frédéric Coiffier\n"));

    m_about->addAuthor(ki18n("Renchi Raju"), ki18n("Author"),
                       "renchi dot raju at gmail dot com");

    m_about->addAuthor(ki18n("Colin Guthrie"), ki18n("Maintainer"),
                       "kde at colin dot guthr dot ie");

    m_about->addAuthor(ki18n("Andrea Diamantini"), ki18n("Developer"),
                       "adjam7 at gmail dot com");

    m_about->addAuthor(ki18n("Gilles Caulier"), ki18n("Developer"),
                       "caulier dot gilles at gmail dot com");

    m_about->addAuthor(ki18n("Frédéric Coiffier"), ki18n("Developer"),
                       "fcoiffie at gmail dot com");

    // help button

    disconnect(this, SIGNAL(helpClicked()),
               this, SLOT(slotHelp()));

    KHelpMenu *helpMenu = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction *handbook   = new QAction(i18n("Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    button(Help)->setMenu(helpMenu->menu());

    // User1 Button : to conf piwigo settings
    KPushButton *confButton = button( User1 );
    confButton->setText( i18n("Settings") );
    confButton->setIcon( KIcon("configure") );
    connect(confButton, SIGNAL(clicked()),
            this, SLOT(slotSettings() ) );

    // we need to let m_talker work..
    m_talker = new PiwigoTalker(d->widget);

    // setting progressDlg and its numeric hints
    m_progressDlg = new QProgressDialog(this);
    m_progressDlg->setModal(true);
    m_progressDlg->setAutoReset(true);
    m_progressDlg->setAutoClose(true);
    m_uploadCount = 0;
    m_uploadTotal = 0;
    mpUploadList  = new QStringList;

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
    group.writeEntry("Set title",       d->captTitleCheckBox->isChecked());
    group.writeEntry("Set description", d->captDescrCheckBox->isChecked());
    group.writeEntry("Download HQ",     d->downloadHQCheckBox->isChecked());
    group.writeEntry("Maximum Width",   d->dimensionSpinBox->value());
    group.writeEntry("Thumbnail Width", d->thumbDimensionSpinBox->value());

    delete m_talker;

    delete mpUploadList;
    delete m_about;

    delete d;
}

void PiwigoWindow::connectSignals()
{
    connect(d->albumView, SIGNAL(itemSelectionChanged()),
            this , SLOT( slotAlbumSelected() ) );

    connect(d->addPhotoBtn, SIGNAL(clicked()),
            this, SLOT(slotAddPhoto()));

    connect(d->resizeCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(slotEnableSpinBox(int)));

    connect(d->logo, SIGNAL(leftClickedUrl(const QString&)),
            this, SLOT(slotProcessUrl(const QString&)));

    connect(m_progressDlg, SIGNAL( canceled() ),
            this, SLOT( slotAddPhotoCancel() ));

    connect(m_talker, SIGNAL(signalProgressInfo(const QString&)),
            this, SLOT(slotProgressInfo(const QString&)));

    connect(m_talker, SIGNAL(signalError(const QString&)),
            this, SLOT(slotError(const QString&)));

    connect(m_talker, SIGNAL(signalBusy(bool)),
            this, SLOT(slotBusy(bool)));

    connect(m_talker, SIGNAL(signalLoginFailed(const QString&)),
            this, SLOT(slotLoginFailed(const QString&)));

    connect(m_talker, SIGNAL(signalAlbums(const QList<GAlbum>&)),
            this, SLOT(slotAlbums(const QList<GAlbum>&)));

    connect(m_talker, SIGNAL(signalAddPhotoSucceeded()),
            this, SLOT(slotAddPhotoSucceeded()));

    connect(m_talker, SIGNAL(signalAddPhotoFailed(const QString&)),
            this, SLOT(slotAddPhotoFailed(const QString&)));
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
        d->dimensionSpinBox->setEnabled(true);
        d->dimensionSpinBox->setValue(group.readEntry("Maximum Width", 600));
    }
    else
    {
        d->resizeCheckBox->setChecked(false);
        d->dimensionSpinBox->setEnabled(false);
    }

    if (group.readEntry("Set title", true))
        d->captTitleCheckBox->setChecked(true);
    else
        d->captTitleCheckBox->setChecked(false);

    if (group.readEntry("Set description", false))
        d->captDescrCheckBox->setChecked(true);
    else
        d->captDescrCheckBox->setChecked(false);

    if (group.readEntry("Download HQ", false))
        d->downloadHQCheckBox->setChecked(true);
    else
        d->downloadHQCheckBox->setChecked(false);

    d->thumbDimensionSpinBox->setValue(group.readEntry("Thumbnail Width", 128));
}

void PiwigoWindow::slotHelp()
{
    KToolInvocation::invokeHelp("piwigoexport", "kipi-plugins");
}

void PiwigoWindow::slotDoLogin()
{
    KUrl url(mpPiwigo->url());
    if (url.protocol().isEmpty())
    {
        url.setProtocol("http");
        url.setHost(mpPiwigo->url());
    }

    // If we've done something clever, save it back to the piwigo.
    if (mpPiwigo->url() != url.url())
    {
        mpPiwigo->setUrl(url.url());
        mpPiwigo->save();
    }

    m_talker->login(url.url(), mpPiwigo->username(), mpPiwigo->password());
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

    QPointer<PiwigoEdit> configDlg = new PiwigoEdit(kapp->activeWindow(), mpPiwigo, i18n("Edit Piwigo Data") );
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
        d->addPhotoBtn->setEnabled(false);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
        bool loggedIn = m_talker->loggedIn();
        d->addPhotoBtn->setEnabled(loggedIn && d->albumView->currentItem());
    }
}

void PiwigoWindow::slotProgressInfo(const QString& msg)
{
    m_progressDlg->setLabelText(msg);
}

void PiwigoWindow::slotError(const QString& msg)
{
    m_progressDlg->hide();
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

    // stop loading if user clicked an image
    if ( item->text(2) == i18n("Image") )
        return;

    if (!item)
    {
        d->addPhotoBtn->setEnabled(false);
    }
    else
    {
        kDebug() << "Album selected\n";

        int albumId = item->data(1, Qt::UserRole).toInt();
        kDebug() << albumId << "\n";
        if (m_talker->loggedIn() && albumId )
        {
            d->addPhotoBtn->setEnabled(true);
        }
        else
        {
            d->addPhotoBtn->setEnabled(false);
        }
    }
}

void PiwigoWindow::slotAddPhoto()
{
    const KUrl::List urls(m_interface->currentSelection().images());

    if ( urls.isEmpty())
    {
        KMessageBox::error(this, i18n("Nothing to upload - please select photos to upload."));
        return;
    }

    for (KUrl::List::const_iterator it = urls.constBegin(); it != urls.constEnd(); ++it)
    {
        mpUploadList->append( (*it).path() );
    }

    m_uploadTotal = mpUploadList->count();
    m_progressDlg->reset();
    m_progressDlg->setMaximum(m_uploadTotal);
    m_uploadCount = 0;
    slotAddPhotoNext();
}

void PiwigoWindow::slotAddPhotoNext()
{
    if ( mpUploadList->isEmpty() )
    {
        m_progressDlg->reset();
        m_progressDlg->hide();
        return;
    }

    QTreeWidgetItem* item = d->albumView->currentItem();
    int column            = d->albumView->currentColumn();
    QString albumTitle    = item->text(column);
    const GAlbum& album   = d->albumDict.value(albumTitle);
    QString photoPath     = mpUploadList->takeFirst();
    QString photoName     = QFileInfo(photoPath).baseName();
    bool res              = m_talker->addPhoto(album.ref_num, photoPath, photoName,
                            d->captTitleCheckBox->isChecked(),
                            d->captDescrCheckBox->isChecked(),
                            d->resizeCheckBox->isChecked(),
                            d->downloadHQCheckBox->isChecked(),
                            d->dimensionSpinBox->value(),
                            d->thumbDimensionSpinBox->value() );

    if (!res)
    {
        slotAddPhotoFailed( "" );
        return;
    }

    m_progressDlg->setLabelText( i18n("Uploading file %1", KUrl(photoPath).fileName()) );

    if (m_progressDlg->isHidden())
        m_progressDlg->show();
}

void PiwigoWindow::slotAddPhotoSucceeded()
{
    m_uploadCount++;
    m_progressDlg->setValue(m_uploadCount);
    slotAddPhotoNext();
}

void PiwigoWindow::slotAddPhotoFailed(const QString& msg)
{
    m_progressDlg->reset();
    m_progressDlg->hide();

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
    m_progressDlg->reset();
    m_progressDlg->hide();
    m_talker->cancel();
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
    }
    d->dimensionSpinBox->setEnabled(b);
}

void PiwigoWindow::slotSettings()
{
    // TODO: reload albumlist if OK slot used.
    QPointer<PiwigoEdit> dlg = new PiwigoEdit(kapp->activeWindow(), mpPiwigo, i18n("Edit Piwigo Data") );
    if ( dlg->exec() == QDialog::Accepted )
    {
        slotDoLogin();
    }
    delete dlg;
}


QString PiwigoWindow::cleanName(const QString& str)
{
    QString plain = str;
    plain.replace("&lt;", "<");
    plain.replace("&gt;", ">");
    plain.replace("&quot;", "\"");
    plain.replace("&amp;", "&");

    return plain;
}

} // namespace KIPIPiwigoExportPlugin
