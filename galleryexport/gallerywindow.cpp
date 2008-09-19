/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-24
 * Description : 
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2006 by Colin Guthrie <kde@colin.guthr.ie>
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008 by Andrea Diamantini <adjam7 at gmail dot com>
 *
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */


// local includes.
#include "gallerywindow.h"
#include "imagedialog.h"
#include "kpaboutdata.h"
#include "galleries.h"
#include "gallerytalker.h"
#include "galleryitem.h"
#include "galleryconfig.h"

// KIPI include files
#include <libkipi/interface.h>

// UI includes
#include "ui_galleryalbumwidget.h"

// Qt includes
#include <Qt>
#include <QDialog>
#include <QPushButton>
#include <QTimer>
#include <QPixmap>
#include <QCursor>
#include <QCheckBox>
#include <QListWidgetItem>
#include <QSpinBox>
#include <QGroupBox>
#include <QInputDialog>

// KDE includes
#include <KAboutData>
#include <KHelpMenu>
#include <KIcon>
#include <KMenu>
#include <KPushButton>
#include <KLocale>
#include <KMessageBox>
#include <KApplication>
#include <KRun>
#include <KDebug>
#include <KConfig>
#include <ktoolinvocation.h>



using namespace KIPIGalleryExportPlugin;

class GalleryWindow::Private
{
private:
    Private(GalleryWindow* parent);

    QWidget*        widget;
    QTreeWidget*    albumView;
    QPushButton*    newAlbumBtn;
    QPushButton*    addPhotoBtn;
    QPushButton*    helpButton;
    QCheckBox*      captTitleCheckBox;
    QCheckBox*      captDescrCheckBox;
    QCheckBox*      resizeCheckBox;
    QSpinBox*       dimensionSpinBox;


    QHash<QString, GAlbum> albumDict;
//    QString lastSelectedAlbum;
  
    friend class GalleryWindow;
};



GalleryWindow::Private::Private(GalleryWindow* parent)
{
    widget = new QWidget(parent);
    parent->setMainWidget(widget);
    parent->setModal(false);

    QHBoxLayout* galleryWidgetLayout = new QHBoxLayout(widget);
 
    // creating and setting objects

    // 1st. QListWidget albumView
    albumView = new QTreeWidget;
    albumView->setHeaderLabel(i18n("albums"));
    galleryWidgetLayout->addWidget(albumView);

    // 2nd. GroupBox optionBox
    QFrame* optionFrame = new QFrame; 

    QVBoxLayout* frameLayout = new QVBoxLayout();

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

    QGroupBox* optionsBox = new QGroupBox(i18n("Override Default Options"));
    QVBoxLayout* optionsBoxLayout = new QVBoxLayout(); 

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
    
    QLabel* resizeLabel = new QLabel(i18n("Maximum dimension:"));
    dimLayout->addWidget(resizeLabel);

    dimensionSpinBox  = new QSpinBox;
    dimensionSpinBox->setValue(600);
    dimensionSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
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
};


// --------------------------------------------

GalleryWindow::GalleryWindow(KIPI::Interface* interface, QWidget *parent, Gallery* pGallery)
        : KDialog(parent),
        m_interface(interface),
        mpGallery(pGallery),
        d(new Private(this))
{
    setWindowTitle( i18n("Gallery Export") );
    setButtons( KDialog::Close | KDialog::Help);
    setModal(false);
    
    // About data.
    m_about = new KIPIPlugins::KPAboutData(ki18n("Gallery Export"),
                                           0,
                                           KAboutData::License_GPL,
                                           ki18n("A Kipi plugin to export image collection to remote Gallery server."),
                                           ki18n("(c) 2003-2005, Renchi Raju"));

    m_about->addAuthor(ki18n("Renchi Raju"), ki18n("Author"),
                       "renchi at pooh dot tam dot uiuc dot edu");

    m_about->addAuthor(ki18n("Colin Guthrie"), ki18n("Maintainer"),
                       "kde at colin dot guthr dot ie");

    m_about->addAuthor(ki18n("Andrea Diamantini"), ki18n("Developer"),
                       "adjam7 at gmail dot com");

    // help button
    disconnect(this, SIGNAL(helpClicked()), this, SLOT(slotHelp()));

    KPushButton *helpButton = button( Help );
    KHelpMenu* helpMenu = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction *handbook = new QAction(i18n("Plugin Handbook"), this);
    connect(handbook, SIGNAL(triggered(bool)), this, SLOT(slotHelp()));
    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    helpButton->setDelayedMenu( helpMenu->menu() );

    // we need to let m_talker work..
    m_talker = new GalleryTalker(d->widget);

    // connect functions
    connectSignals();

    // read Settings
    readSettings();

    QTimer::singleShot(0, this,  SLOT(slotDoLogin()));
};


GalleryWindow::~GalleryWindow()
{
    // write config
    KConfig config("kipirc");
    KConfigGroup group = config.group("GallerySync Galleries");

    group.writeEntry("Resize", d->resizeCheckBox->isChecked());
    group.writeEntry("Set title", d->captTitleCheckBox->isChecked());
    group.writeEntry("Set description", d->captDescrCheckBox->isChecked());
    group.writeEntry("Maximum Width",  d->dimensionSpinBox->value());

    delete d;
    delete m_about;
};


void GalleryWindow::connectSignals()
{
    connect(d->albumView, SIGNAL(itemSelectionChanged()), 
        this , SLOT(slotAlbumSelected()));
    connect(d->newAlbumBtn, SIGNAL(clicked()), this, SLOT(slotNewAlbum()));
    connect(d->addPhotoBtn, SIGNAL(clicked()), this, SLOT(slotAddPhoto()));

    connect(m_talker, SIGNAL(signalError(const QString&)),
        this, SLOT(slotError(const QString&)));
    connect(m_talker, SIGNAL(signalBusy(bool)), this, SLOT(slotBusy(bool)));
    connect(m_talker, SIGNAL(signalLoginFailed(const QString&)), 
        this, SLOT(slotLoginFailed(const QString&)));
    connect(m_talker, SIGNAL(signalAlbums(const QList<GAlbum>&)), 
        this, SLOT(slotAlbums(const QList<GAlbum>&)));
    connect(m_talker, SIGNAL(signalPhotos(const QList<GPhoto>&)), 
        this, SLOT(slotPhotos(const QList<GPhoto>&)));
    connect(m_talker, SIGNAL(signalAddPhotoSucceeded()),
        this, SLOT(slotAddPhotoSucceeded()));
    connect(m_talker, SIGNAL(signalAddPhotoFailed(const QString&)), 
        this, SLOT(slotAddPhotoFailed(const QString&)));
};


void GalleryWindow::readSettings()
{
    // read Config
    KConfig config("kipirc");
    KConfigGroup group = config.group("GallerySync Galleries");

    if (group.readEntry("Resize", false)) {
        d->resizeCheckBox->setChecked(true);
        d->dimensionSpinBox->setEnabled(true);
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

    d->dimensionSpinBox->setValue(group.readEntry("Maximum Width", 1600));
};



void GalleryWindow::slotHelp()
{
    KToolInvocation::invokeHelp("galleryexport", "kipi-plugins");
    return;
};


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
};



void GalleryWindow::slotLoginFailed(const QString& msg)
{
    if (KMessageBox::warningYesNo(this,
                                  i18n("Failed to login into remote gallery. ")
                                  + msg
                                  + i18n("\nDo you want to try again?"))
            != KMessageBox::Yes) {
        close();
        return;
    }

    slotDoLogin();
};



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
};



void GalleryWindow::slotError(const QString& msg)
{
    KMessageBox::error(this, msg);
};



void GalleryWindow::slotAlbums(const QList<GAlbum>& albumList)
{
    d->albumDict.clear();
    d->albumView->clear();

    typedef QList<GAlbum> GAlbumList;
    GAlbumList::const_iterator iterator;
    for (iterator = albumList.begin(); iterator != albumList.end(); ++iterator) {
        if ( (*iterator).parent_ref_num != 0 ) {
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, (*iterator).title );
            item->setIcon(0, KIcon("inode-directory") );
            d->albumView->addTopLevelItem(item);

            const GAlbum& album = *iterator;
            d->albumDict.insert(album.title, album);
        }
    }
};



void GalleryWindow::slotPhotos(const QList<GPhoto>& photoList)
{
    QTreeWidgetItem* parentItem = d->albumView->currentItem();

    typedef QList<GPhoto> GPhotoList;
    GPhotoList::const_iterator iterator;
    for (iterator = photoList.begin(); iterator != photoList.end(); ++iterator) {
        QTreeWidgetItem *item = new QTreeWidgetItem(parentItem);
        item->setText(0, (*iterator).caption );
        item->setIcon(0, KIcon("image-x-generic") );
    }
};



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
                d->addPhotoBtn->setEnabled(true);
                QString title = item->text(column);
                const GAlbum& album = d->albumDict.value(title); 
                m_talker->listPhotos(album.name);
        }
    }
};


// FIXME reenable me! yeah..
// void GalleryWindow::slotOpenPhoto(const KUrl& url)
// {
//     new KRun(url, this);
// };



void GalleryWindow::slotNewAlbum()
{
    QString text = QInputDialog::getText(this, i18n("Create New album"), i18n("Insert new album name") );

    QString name    = text;
    QString title   = text;
    QString caption = text;

    // check for prohibited chars in the album name
    // \ / * ? " ' & < > | . + # ( ) or spaces
    // Todo: Change this to a QRegExp check.
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
        KMessageBox::error(this, i18nc("Sorry, these characters are not allowed in album name: %1" , "\\ / * ? \" \' & < > | . + # ( ) or spaces"));
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
};



void GalleryWindow::slotAddPhoto()
{
    QTreeWidgetItem* item = d->albumView->currentItem();
    int column = d->albumView->currentColumn();
    if (!item)
        return;     // NO item selected FIXME: do something

    // albumName
    QString albumTitle = item->text(column);
    if(!d->albumDict.contains(albumTitle))
        return;     // NO album selected: FIXME: do something

    const GAlbum& album = d->albumDict.value(albumTitle); 

    // photoPath
    KUrl::List urls = KIPIPlugins::ImageDialog::getImageURLs(this, m_interface);
    if (urls.isEmpty())
        return;

    QString photoPath = urls.at(0).path();
    bool res = m_talker->addPhoto(album.name, photoPath, QString(),
                                  d->captTitleCheckBox->isChecked(),
                                  d->captDescrCheckBox->isChecked(),
                                  d->resizeCheckBox->isChecked(),
                                  d->dimensionSpinBox->value());

    if (!res) {
        slotAddPhotoFailed("");
        return;
    }
};



void GalleryWindow::slotAddPhotoSucceeded()
{
    slotAddPhoto();         // FIXME temporary solution
};



void GalleryWindow::slotAddPhotoFailed(const QString& msg)
{
    if (KMessageBox::warningContinueCancel(this,
                                           i18n("Failed to upload photo into "
                                                "remote gallery. ")
                                           + msg
                                           + i18n("\nDo you want to continue?"))
            != KMessageBox::Continue) {
    } else {
        slotAddPhoto();
    }
};



void GalleryWindow::slotAddPhotoCancel()
{
    m_talker->cancel();
};


#include "gallerywindow.moc"
