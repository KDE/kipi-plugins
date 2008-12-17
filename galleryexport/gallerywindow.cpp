/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-01
 * Description : a plugin to export to a remote Gallery server.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2006 by Colin Guthrie <kde@colin.guthr.ie>
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008 by Andrea Diamantini <adjam7 at gmail dot com>
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

#include "gallerywindow.h"

// Qt includes.

#include <QCheckBox>
#include <QDialog>
#include <QFileInfo>
#include <QGroupBox>
#include <QPushButton>
#include <QSpinBox>
#include <Qt>
#include <QTreeWidgetItem>
// KDE includes.

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

// LibKIPI includes.

#include <libkipi/interface.h>

// Local includes.

#include "albumdlg.h"
#include "galleries.h"
#include "galleryconfig.h"
#include "galleryitem.h"
#include "gallerytalker.h"
#include "imagedialog.h"
#include "kpaboutdata.h"

using namespace KIPIGalleryExportPlugin;

// -------------------------------------------------------------------------------

class GalleryWindow::Private
{
private:
    Private(GalleryWindow* parent);

    QWidget *widget;
    QTreeWidget *albumView;
    QPushButton *newAlbumBtn;
    QPushButton *addPhotoBtn;
    QCheckBox *captTitleCheckBox;
    QCheckBox *captDescrCheckBox;
    QCheckBox *resizeCheckBox;
    QSpinBox *dimensionSpinBox;

    QHash<QString, GAlbum> albumDict;

    friend class GalleryWindow;
};


GalleryWindow::Private::Private(GalleryWindow* parent)
{
    widget = new QWidget(parent);
    parent->setMainWidget(widget);
    parent->setModal(false);

    QHBoxLayout *galleryWidgetLayout = new QHBoxLayout(widget);

    // --- creating and setting objects

    // 1st. QListWidget albumView
    albumView = new QTreeWidget;
    albumView->setHeaderLabel(i18n("albums"));
    galleryWidgetLayout->addWidget(albumView);

    // 2nd. GroupBox optionBox
    QFrame *optionFrame = new QFrame;
    QVBoxLayout *frameLayout = new QVBoxLayout();

    newAlbumBtn = new QPushButton;
    newAlbumBtn->setText(i18n("&New Album"));
    newAlbumBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    newAlbumBtn->setEnabled(false);
    frameLayout->addWidget(newAlbumBtn);

    addPhotoBtn = new QPushButton;
    addPhotoBtn->setText(i18n("&Add Photos"));
    addPhotoBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    addPhotoBtn->setEnabled(false);
    frameLayout->addWidget(addPhotoBtn);

    QGroupBox *optionsBox = new QGroupBox(i18n("Override Default Options"));
    QVBoxLayout *optionsBoxLayout = new QVBoxLayout();

    captTitleCheckBox = new QCheckBox(optionsBox);
    captTitleCheckBox->setText(i18n("Comment sets Title"));
    optionsBoxLayout->addWidget(captTitleCheckBox);

    captDescrCheckBox = new QCheckBox(optionsBox);
    captDescrCheckBox->setText(i18n("Comment sets Description"));
    optionsBoxLayout->addWidget(captDescrCheckBox);

    resizeCheckBox = new QCheckBox(optionsBox);
    resizeCheckBox->setText(i18n("Resize photos before uploading"));
    optionsBoxLayout->addWidget(resizeCheckBox);

    QHBoxLayout *dimLayout = new QHBoxLayout;

    QLabel *resizeLabel = new QLabel(i18n("Maximum dimension:"));
    dimLayout->addWidget(resizeLabel);

    dimensionSpinBox  = new QSpinBox;
    dimensionSpinBox->setRange(1,1600);
    dimensionSpinBox->setValue(600);
    dimLayout->addWidget(dimensionSpinBox);

    optionsBoxLayout->addLayout(dimLayout);

    captTitleCheckBox->setChecked(true);
    captDescrCheckBox->setChecked(false);
    resizeCheckBox->setChecked(false);
    dimensionSpinBox->setEnabled(false);

    optionsBox->setLayout(optionsBoxLayout);
    frameLayout->addWidget(optionsBox);

    optionFrame->setLayout(frameLayout);
    galleryWidgetLayout->addWidget(optionFrame);

    widget->setLayout(galleryWidgetLayout);
}

// --------------------------------------------------------------------------------------------------------------------------

GalleryWindow::GalleryWindow(KIPI::Interface* interface, QWidget *parent, Gallery* pGallery)
        : KDialog(parent),
        m_interface(interface),
        mpGallery(pGallery),
        d(new Private(this))
{
    setWindowTitle( i18n("Gallery Export") );
    setButtons( KDialog::Close | KDialog::User1 | KDialog::Help);
    setModal(false);

    // About data.
    m_about = new KIPIPlugins::KPAboutData(ki18n("Gallery Export"),
                                           0,
                                           KAboutData::License_GPL,
                                           ki18n("A Kipi plugin to export image collection to remote Gallery server."),
                                           ki18n("(c) 2003-2005, Renchi Raju\n"
                                                    "(c) 2006-2007, Colin Guthrie\n"
                                                    "(c) 2008, Andrea Diamantini\n")
    );

    m_about->addAuthor(ki18n("Renchi Raju"), ki18n("Author"),
                       "renchi at pooh dot tam dot uiuc dot edu");

    m_about->addAuthor(ki18n("Colin Guthrie"), ki18n("Maintainer"),
                       "kde at colin dot guthr dot ie");

    m_about->addAuthor(ki18n("Andrea Diamantini"), ki18n("Developer"),
                       "adjam7 at gmail dot com");

    // help button
    disconnect(this, SIGNAL(helpClicked()), this, SLOT(slotHelp()));

    KPushButton *helpButton = button( Help );
    KHelpMenu *helpMenu = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction *handbook = new QAction(i18n("Plugin Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)), this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    helpButton->setDelayedMenu( helpMenu->menu() );

    // User1 Button : to conf gallery settings
    KPushButton *confButton = button( User1 );
    confButton->setText( i18n("settings") );
    confButton->setIcon( KIcon("applications-system") );
    connect(confButton, SIGNAL(clicked()), this, SLOT(slotSettings() ) );

    // we need to let m_talker work..
    m_talker = new GalleryTalker(d->widget);

    // setting progressDlg and its numeric hints
    m_progressDlg = new QProgressDialog(this);
    m_progressDlg->setModal(true);
    m_progressDlg->setAutoReset(true);
    m_progressDlg->setAutoClose(true);
    m_uploadCount = 0;
    m_uploadTotal = 0;
    mpUploadList = new QStringList;

    // connect functions
    connectSignals();

    // read Settings
    readSettings();

    slotDoLogin();
}


GalleryWindow::~GalleryWindow()
{
    // write config
    KConfig config("kipirc");
    KConfigGroup group = config.group("GallerySync Galleries");

    group.writeEntry("Resize", d->resizeCheckBox->isChecked());
    group.writeEntry("Set title", d->captTitleCheckBox->isChecked());
    group.writeEntry("Set description", d->captDescrCheckBox->isChecked());
    group.writeEntry("Maximum Width",  d->dimensionSpinBox->value());
    delete mpUploadList;
    delete d;
    delete m_about;
}


void GalleryWindow::connectSignals()
{
    connect(d->albumView, SIGNAL(itemSelectionChanged()), this , SLOT(slotAlbumSelected()));
    connect(d->newAlbumBtn, SIGNAL(clicked()), this, SLOT(slotNewAlbum()));
    connect(d->addPhotoBtn, SIGNAL(clicked()), this, SLOT(slotAddPhoto()));
    connect(d->resizeCheckBox, SIGNAL(stateChanged(int)), this, SLOT(slotEnableSpinBox(int)));

    connect(m_progressDlg, SIGNAL( canceled() ), this, SLOT( slotAddPhotoCancel() ));

    connect(m_talker, SIGNAL(signalError(const QString&)), this, SLOT(slotError(const QString&)));
    connect(m_talker, SIGNAL(signalBusy(bool)), this, SLOT(slotBusy(bool)));
    connect(m_talker, SIGNAL(signalLoginFailed(const QString&)), this, SLOT(slotLoginFailed(const QString&)));
    connect(m_talker, SIGNAL(signalAlbums(const QList<GAlbum>&)), this, SLOT(slotAlbums(const QList<GAlbum>&)));
    connect(m_talker, SIGNAL(signalPhotos(const QList<GPhoto>&)), this, SLOT(slotPhotos(const QList<GPhoto>&)));
    connect(m_talker, SIGNAL(signalAddPhotoSucceeded()), this, SLOT(slotAddPhotoSucceeded()));
    connect(m_talker, SIGNAL(signalAddPhotoFailed(const QString&)), this, SLOT(slotAddPhotoFailed(const QString&)));
}


void GalleryWindow::readSettings()
{
    // read Config
    KConfig config("kipirc");
    KConfigGroup group = config.group("GallerySync Galleries");

    if (group.readEntry("Resize", false)) {
        d->resizeCheckBox->setChecked(true);
        d->dimensionSpinBox->setEnabled(true);
        d->dimensionSpinBox->setValue(group.readEntry("Maximum Width", 600));
    } else {
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

}



void GalleryWindow::slotHelp()
{
    KToolInvocation::invokeHelp("galleryexport", "kipi-plugins");
    return;
}


void GalleryWindow::slotDoLogin()
{
    GalleryTalker::setGallery2((2 == mpGallery->version()));

    KUrl url(mpGallery->url());
    if (url.protocol().isEmpty()) {
        url.setProtocol("http");
        url.setHost(mpGallery->url());
    }
    if (!url.url().endsWith(".php")) {
        if (GalleryTalker::isGallery2())
            url.addPath("main.php");
        else
            url.addPath("gallery_remote2.php");
    }
    // If we've done something clever, save it back to the gallery.
    if (mpGallery->url() != url.url()) {
        mpGallery->setUrl(url.url());
        mpGallery->save();
    }

    m_talker->login(url.url(), mpGallery->username(), mpGallery->password());
}



void GalleryWindow::slotLoginFailed(const QString& msg)
{
    if (KMessageBox::warningYesNo(this,
                                  i18n("Failed to login into remote gallery. ")
                                  + msg
                                  + i18n("\nDo you want to check your settings and try again?"))
            != KMessageBox::Yes) {
        close();
        return;
    }

    GalleryEdit configDlg(kapp->activeWindow(), mpGallery, i18n("Edit Gallery Data") );
    if ( configDlg.exec() != QDialog::Accepted )
    {
        return;
    }
    slotDoLogin();
}



void GalleryWindow::slotBusy(bool val)
{
    if (val)
    {
        setCursor(Qt::WaitCursor);
        d->newAlbumBtn->setEnabled(false);
        d->addPhotoBtn->setEnabled(false);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
        bool loggedIn = m_talker->loggedIn();
        d->newAlbumBtn->setEnabled(loggedIn);
        d->addPhotoBtn->setEnabled(loggedIn && d->albumView->currentItem());
    }
}

void GalleryWindow::slotError(const QString& msg)
{
    m_progressDlg->hide();
    KMessageBox::error(this, msg);
}



void GalleryWindow::slotAlbums(const QList<GAlbum>& albumList)
{
    d->albumDict.clear();
    d->albumView->clear();

//     typedef QList<GAlbum> GAlbumList;
    QVector<int> ref_num_vect( albumList.size() );

    for (int i = 0; i < albumList.size(); ++i)
    {
        const GAlbum& album = albumList.at(i);
        ref_num_vect.insert( i, album.name.toInt() );

        int parentRefNum = album.parent_ref_num;
        if ( parentRefNum == 0 )
        {
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, album.title );
            item->setIcon(0, KIcon("inode-directory") );

            d->albumView->addTopLevelItem(item);
            d->albumDict.insert(album.title, album);
        }
        else
        {
            int n = ref_num_vect.indexOf( parentRefNum );
            const GAlbum& parentAlbum = albumList.at(n);    // ERROR
            QString parentTitle = parentAlbum.title;
            QTreeWidgetItem *parentItem = ( d->albumView->findItems(parentTitle, Qt::MatchExactly) ).at(0);

            QTreeWidgetItem *item = new QTreeWidgetItem(parentItem);
            item->setText(0, album.title );
            item->setIcon(0, KIcon("inode-directory") );

            d->albumDict.insert(album.title, album);
        }
    }
}



void GalleryWindow::slotPhotos(const QList<GPhoto>& photoList)
{
    QTreeWidgetItem* parentItem = d->albumView->currentItem();

    // clean item to avoid duplications
    QList<QTreeWidgetItem *> childrenList = parentItem->takeChildren();
    foreach( QTreeWidgetItem *child, childrenList )
        parentItem->removeChild( child );

    typedef QList<GPhoto> GPhotoList;
    GPhotoList::const_iterator iterator;
    for (iterator = photoList.begin(); iterator != photoList.end(); ++iterator)
    {
        QString plain = (*iterator).caption;
        plain.replace("&lt;", "<");
        plain.replace("&gt;", ">");
        plain.replace("&quot;", "\"");
        plain.replace("&amp;", "&");

        QTreeWidgetItem *item = new QTreeWidgetItem(parentItem);
        item->setText(0, plain );
        item->setIcon(0, KIcon("image-x-generic") );
    }
}



void GalleryWindow::slotAlbumSelected()
{
    QTreeWidgetItem* item = d->albumView->currentItem();
    int column = d->albumView->currentColumn();
    if (!item)
    {
        d->addPhotoBtn->setEnabled(false);
    }
    else
    {
        if (m_talker->loggedIn())
        {
                QString title = item->text(column);
                const GAlbum& album = d->albumDict.value(title);
                if ( album.name != "" )
                {
                    d->addPhotoBtn->setEnabled(true);
                    m_talker->listPhotos(album.name);
                }
                else
                {
                    d->addPhotoBtn->setEnabled(false);
                }
        }
    }
}


// FIXME reenable me! yeah..
// void GalleryWindow::slotOpenPhoto(const KUrl& url)
// {
//     new KRun(url, this);
// }



void GalleryWindow::slotNewAlbum()
{
    AlbumDlg *dlg = new AlbumDlg(d->widget);
    dlg->titleEdit->setFocus();
    if ( dlg->exec() != QDialog::Accepted )
    {
        return;
    }

    QString name = dlg->nameEdit->text();
    QString title = dlg->titleEdit->text();
    QString caption = dlg->captionEdit->text();
    delete dlg;

    // check for prohibited chars in the album name
    // \ / * ? " ' & < > | . + # ( ) or spaces
    // TODO: Change this to a QRegExp check.
    QChar ch;
    bool  clean = true;
    for (int i = 0; i < name.length(); ++i) {
        ch = name[i];
        if (ch == '\\') {
            clean = false;
            break;
        } else if (ch == '/') {
            clean = false;
            break;
        } else if (ch == '*') {
            clean = false;
            break;
        } else if (ch == '?') {
            clean = false;
            break;
        } else if (ch == '"') {
            clean = false;
            break;
        } else if (ch == '\'') {
            clean = false;
            break;
        } else if (ch == '&') {
            clean = false;
            break;
        } else if (ch == '<') {
            clean = false;
            break;
        } else if (ch == '>') {
            clean = false;
            break;
        } else if (ch == '|') {
            clean = false;
            break;
        } else if (ch == '.') {
            clean = false;
            break;
        } else if (ch == '+') {
            clean = false;
            break;
        } else if (ch == '#') {
            clean = false;
            break;
        } else if (ch == '(') {
            clean = false;
            break;
        } else if (ch == ')') {
            clean = false;
            break;
        } else if (ch == ' ') {
            clean = false;
            break;
        }
    }

    if (!clean) {
        KMessageBox::error(this, i18n("Sorry, these characters are not allowed in album name: \\ / * ? \" \' & < > | . + # ( ) or spaces"));
        return;
    }

    QString parentAlbumName;

    QTreeWidgetItem* item = d->albumView->currentItem();
    int column = d->albumView->currentColumn();
    if (item) {
        const GAlbum& album = d->albumDict.value( item->text(column) );
        parentAlbumName = album.name;
    } else {
        parentAlbumName = "0";
    }

    m_talker->createAlbum(parentAlbumName, name, title, caption);
}



void GalleryWindow::slotAddPhoto()
{
    QTreeWidgetItem* item = d->albumView->currentItem();
    int column = d->albumView->currentColumn();
    if (!item)
        return;     // NO album selected: FIXME: do something

    // albumName
    QString albumTitle = item->text(column);
    if(!d->albumDict.contains(albumTitle))
        return;     // NO album name found: FIXME: do something

    // photoPath
    KUrl::List urls = KIPIPlugins::ImageDialog::getImageURLs(this, m_interface);
    if (urls.isEmpty())
        return; // NO photo selected: FIXME: do something

    for (KUrl::List::iterator it = urls.begin(); it != urls.end(); ++it)
    {
        mpUploadList->append( (*it).path() );
    }

    m_uploadTotal = mpUploadList->count();
    m_progressDlg->reset();
    m_progressDlg->setMaximum(m_uploadTotal);
    m_uploadCount = 0;
    slotAddPhotoNext();
}


void GalleryWindow::slotAddPhotoNext()
{
    if ( mpUploadList->isEmpty() )
    {
        m_progressDlg->reset();
        m_progressDlg->hide();
        slotAlbumSelected();        // ?
        return;
    }

    QTreeWidgetItem* item = d->albumView->currentItem();
    int column = d->albumView->currentColumn();
    QString albumTitle = item->text(column);
    const GAlbum& album = d->albumDict.value(albumTitle);

    QString photoPath = mpUploadList->takeFirst();
    QString photoName = QFileInfo(photoPath).baseName();
    bool res = m_talker->addPhoto(album.name, photoPath, photoName,
                                  d->captTitleCheckBox->isChecked(),
                                  d->captDescrCheckBox->isChecked(),
                                  d->resizeCheckBox->isChecked(),
                                  d->dimensionSpinBox->value() );

    if (!res)
    {
        slotAddPhotoFailed( "" );
        return;
    }

    m_progressDlg->setLabelText( i18n("Uploading file ") +  KUrl(photoPath).fileName()  );
    if (m_progressDlg->isHidden())
        m_progressDlg->show();
}

void GalleryWindow::slotAddPhotoSucceeded()
{
    m_uploadCount++;
    m_progressDlg->setValue(m_uploadCount);
    slotAddPhotoNext();
}



void GalleryWindow::slotAddPhotoFailed(const QString& msg)
{
    m_progressDlg->reset();
    m_progressDlg->hide();

    if (KMessageBox::warningContinueCancel(this,
                                           i18n("Failed to upload photo into "
                                                "remote gallery. ")
                                           + msg
                                           + i18n("\nDo you want to continue?"))
            != KMessageBox::Continue) {
    } else {
        slotAddPhotoNext();
    }
}



void GalleryWindow::slotAddPhotoCancel()
{
    m_progressDlg->reset();
    m_progressDlg->hide();

    m_talker->cancel();
}

void GalleryWindow::slotEnableSpinBox(int n)
{
    bool b;
    switch(n) {
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

void GalleryWindow::slotSettings()
{
    GalleryEdit dlg(kapp->activeWindow(), mpGallery, i18n("Edit Gallery Data") );
    dlg.exec();
}


#include "gallerywindow.moc"
