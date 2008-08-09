/* ============================================================
 * File  : gallerywindow.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2004-11-30
 * Description :
 *
 * Copyright 2004-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *
 *
 * Modified by : Andrea Diamantini <adjam7@gmail.com>
 * Date        : 2008-07-11
 * Copyright 2008 by Andrea Diamantini <adjam7@gmail.com>
 *
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Qt includes

#include <Qt>
#include <QPushButton>
#include <QTimer>
#include <QPixmap>
#include <QCursor>
#include <QCheckBox>
#include <QProgressDialog>
#include <QListWidgetItem>
#include <QSpinBox>
#include <QGroupBox>

// Include files for KDE
#include <KAboutData>
#include <KHelpMenu>
#include <KLocale>
#include <KMessageBox>
#include <KApplication>
//#include <KIconLoader>
#include <khtml_part.h>
#include <khtmlview.h>
#include <KRun>
#include <KDebug>
#include <KConfig>
#include <ktoolinvocation.h>

// KIPI include files
#include <libkipi/interface.h>

// Local includes.
#include "imagedialog.h"
#include "kpaboutdata.h"

// local includes
#include "galleries.h"
#include "gallerytalker.h"
#include "galleryitem.h"
#include "galleryviewitem.h"
#include "gallerywindow.h"

// UI includes
#include "ui_galleryalbumdialog.h"


using namespace KIPIGalleryExportPlugin;


class GalleryWindow::Private
{
private:
    Private(GalleryWindow* parent);

    QWidget*        widget;
    QTreeWidget*    albumView;
    KHTMLPart*      photoView;
    QPushButton*    newAlbumBtn;
    QPushButton*    addPhotoBtn;
    QPushButton*    helpButton;
    QCheckBox*      captTitleCheckBox;
    QCheckBox*      captDescrCheckBox;
    QCheckBox*      resizeCheckBox;
    QSpinBox*       dimensionSpinBox;


    QHash<int, GAlbumViewItem> albumDict;
    QString lastSelectedAlbum;
    QProgressDialog* progressDlg;
    unsigned int uploadCount;
    unsigned int uploadTotal;
    QList< QPair<QString, QString> >  uploadQueue;

    friend class GalleryWindow;
};



GalleryWindow::Private::Private(GalleryWindow* parent)
{
    widget = new QWidget(parent);
    parent->setMainWidget(widget);

    QHBoxLayout* galleryWidgetLayout = new QHBoxLayout(widget);
 
    // creating and setting objects

    // 1st. QListWidget albumView
    albumView = new QTreeWidget;
    albumView->setHeaderLabel(i18n("albums"));
    galleryWidgetLayout->addWidget(albumView);

    // 2nd. KHTMLPart photoView
//    photoView = new KHTMLPart;
//    galleryWidgetLayout->addWidget(photoView);
    // ------------------------------------------------------------------------

    // 3rd. GroupBox optionBox
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

// ----------------------------------------------

    progressDlg = new QProgressDialog( QString("ciao"), QString("ohi ohi"), 0, 0, widget); // FIXME
    progressDlg->setAutoReset(true);
    progressDlg->setAutoClose(true);

    widget->setLayout(galleryWidgetLayout);
}


// --------------------------------------------

GalleryWindow::GalleryWindow(KIPI::Interface* interface, QWidget *parent, Gallery* pGallery)
        : KDialog(parent),
        m_interface(interface),
        mpGallery(pGallery),
        d(new Private(this))
{
    setWindowTitle( i18n("Gallery Export") );
    setButtons( KDialog::Ok | KDialog::Cancel );

    // perhaps we need for m_talker to work..
    m_talker = new GalleryTalker(d->widget);

    d->uploadCount = 0;
    d->uploadTotal = 0;

    // About data and help button.
    m_about = new KIPIPlugins::KPAboutData(ki18n("Gallery Export"),
                                           0,
                                           KAboutData::License_GPL,
                                           ki18n("A Kipi plugin to export image collection to remote Gallery server."),
                                           ki18n("(c) 2003-2005, Renchi Raju"));

    m_about->addAuthor(ki18n("Renchi Raju"), ki18n("Author"),
                       "renchi at pooh dot tam dot uiuc dot edu");

    m_about->addAuthor(ki18n("Colin Guthrie"), ki18n("Maintainer"),
                       "kde at colin dot guthr dot ie");

    // TODO add /me

    // FIXME let apps crash..
    connectSignals();

    // read Config
    KConfig config("kipirc");
    KConfigGroup group = config.group("GallerySync Galleries");

    if (group.readEntry("Resize", false))
    {
        d->resizeCheckBox->setChecked(true);
        d->dimensionSpinBox->setEnabled(true);
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

    d->dimensionSpinBox->setValue(group.readEntry("Maximum Width", 1600));

    //QTimer::singleShot(0, this,  SLOT(slotDoLogin()));

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

    delete d;
    delete m_about;
}

void GalleryWindow::connectSignals()
{
    connect(d->progressDlg, SIGNAL(canceled()), this , SLOT(slotAddPhotoCancel()));
    connect(d->albumView, SIGNAL(itemSelectionChanged()), this , SLOT(slotAlbumSelected()));
    connect(d->newAlbumBtn, SIGNAL(clicked()), this, SLOT(slotNewAlbum()));
    connect(d->newAlbumBtn, SIGNAL(clicked()), this, SLOT(slotDoLogin()));
    connect(d->addPhotoBtn, SIGNAL(clicked()), this, SLOT(slotAddPhotos()));

// FIXME let app crashs..
//  connect(d->photoView->browserExtension(), SIGNAL(openURLRequest(const KUrl&, const KParts::URLArgs&)),
//             this, SLOT(slotOpenPhoto(const KUrl&)));

    connect(m_talker, SIGNAL(signalError(const QString&)), this, SLOT(slotError(const QString&)));
    connect(m_talker, SIGNAL(signalBusy(bool)), this, SLOT(slotBusy(bool)));
    connect(m_talker, SIGNAL(signalLoginFailed(const QString&)), 
        this, SLOT(slotLoginFailed(const QString&)));
    connect(m_talker, SIGNAL(signalAlbums(const QList<GAlbum>&)), 
        this, SLOT(slotAlbums(const QList<GAlbum>&)));
    connect(m_talker, SIGNAL(signalPhotos(const QList<GPhoto>&)), 
        this, SLOT(slotPhotos(const QList<GPhoto>&)));
    connect(m_talker, SIGNAL(signalAddPhotoSucceeded()), this, SLOT(slotAddPhotoSucceeded()));
    connect(m_talker, SIGNAL(signalAddPhotoFailed(const QString&)), 
        this, SLOT(slotAddPhotoFailed(const QString&)));
}


void GalleryWindow::slotHelp()
{
    // FIXME KToolInvocation::invokeHelp("galleryexport", "kipi-plugins");
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
                                  + i18n("\nDo you want to try again?"))
            != KMessageBox::Yes) {
        close();
        return;
    }

    slotDoLogin();
}

void GalleryWindow::slotBusy(bool val)
{
    if (val) {
        QCursor(3); // FIXME Qt::WaitCursor);
        d->newAlbumBtn->setEnabled(false);
        d->addPhotoBtn->setEnabled(false);
    } else {
        QCursor(0); // FIXME Qt::ArrowCursor);
        bool loggedIn = m_talker->loggedIn();
        d->newAlbumBtn->setEnabled(loggedIn);
        d->addPhotoBtn->setEnabled(loggedIn && d->albumView->currentItem());
    }
}

void GalleryWindow::slotError(const QString& msg)
{
    KMessageBox::error(this, msg);
}

void GalleryWindow::slotAlbums(const QList<GAlbum>& albumList)
{
    d->albumDict.clear();
    d->albumView->clear();
    d->photoView->begin();
    d->photoView->write("<html></html>");
    d->photoView->end();

    // FIXME
    // KIconLoader* iconLoader = KApplication::kApplication()->iconLoader();
    QPixmap pix = QPixmap();// QPixmap pix = iconLoader->loadIcon( "folder", KIconLoader::NoGroup, 32 );

    typedef QList<GAlbum> GAlbumList;
    GAlbumList::const_iterator iter;
    for (iter = albumList.begin(); iter != albumList.end(); ++iter) {
        const GAlbum& album = *iter;

        if (album.parent_ref_num == 0) {
            GAlbumViewItem item = GAlbumViewItem(d->albumView, album.title, album);
            //FIXME item->setPixmap( 0, pix );
            d->albumDict.insert(album.ref_num, item);
        } else {
            QTreeWidgetItem *parent = new QTreeWidgetItem(d->albumDict.take(album.parent_ref_num)); 
            // FIXME s, find, take
            if (parent) {
                GAlbumViewItem item = GAlbumViewItem(parent, album.title, album);
                // FIXME item->setPixmap( 0, pix );
                d->albumDict.insert(album.ref_num, item);
            } else {
                kWarning() << "Failed to find parent for album "
                << album.name
                << " with id " << album.ref_num << "\n";
            }
        }
    }


    // find and select the last selected album
    int lastSelectedID = 0;
    for (iter = albumList.begin(); iter != albumList.end(); ++iter) {
        if ((*iter).name == d->lastSelectedAlbum) {
            lastSelectedID = (*iter).ref_num;
            break;
        }
    }

    if (lastSelectedID > 0) {
        QTreeWidgetItem *lastSelectedItem = new QTreeWidgetItem(d->albumDict.take(lastSelectedID));
        // FIXME s, find, take
        if (lastSelectedItem) {
            d->albumView->setCurrentItem(lastSelectedItem);    // true
// FIXME perhaps to be removed           m_albumView->ensureItemVisible( lastSelectedItem );
        }
    }
}

void GalleryWindow::slotPhotos(const QList<GPhoto>& photoList)
{
    int pxSize = fontMetrics().height() - 2;
    QString styleSheet =
        QString("body { margin: 8px; font-size: %1px; "
                " color: %2; background-color: %3;}")
        .arg(pxSize , Qt::black , Qt::white );

    styleSheet += QString("a { font-size: %1px; color: %2; "
                          "text-decoration: none;}")
                  .arg(pxSize , Qt::black );
    styleSheet += QString("i { font-size: %1px; color: %2; "
                          "text-decoration: none;}")
                  .arg(pxSize - 2 , Qt::blue );

    d->photoView->begin();
    d->photoView->setUserStyleSheet(styleSheet);
    d->photoView->write("<html>");

    d->photoView->write("<table class='box-body' width='100%' "
                       "border='0' cellspacing='1' cellpadding='1'>");

    typedef QList<GPhoto> GPhotoList;
    GPhotoList::const_iterator iter;
    for (iter = photoList.begin(); iter != photoList.end(); ++iter)
    {
        const GPhoto& photo = *iter;
        KUrl imageurl(photo.albumURL + photo.name);
        KUrl thumburl(photo.albumURL + photo.thumbName);

        d->photoView->write("<tr><td class='photo'>"
                           + QString("<a href='%1'>")
                           .arg(imageurl.url())
                           + QString("<img border=1 src=\"%1\"><br>")
                           .arg(thumburl.url())
                           + photo.name
                           + (photo.caption.isEmpty() ? QString() :
                              QString("<br><i>%1</i>")
                              .arg(photo.caption))
                           + "</a></td></tr>");
    }

    d->photoView->write("</table>");
    d->photoView->write("</html>");
    d->photoView->end();
}

void GalleryWindow::slotAlbumSelected()
{
    QTreeWidgetItem* item = d->albumView->currentItem();
    if (!item) {
        d->addPhotoBtn->setEnabled(false);
    } else {
        if (m_talker->loggedIn()) {
            d->addPhotoBtn->setEnabled(true);

            d->photoView->begin();
            d->photoView->write("<html></html>");
            d->photoView->end();

            GAlbumViewItem* viewItem = static_cast<GAlbumViewItem*>(item);
            m_talker->listPhotos(viewItem->album.name);
            d->lastSelectedAlbum = viewItem->album.name;
        }
    }
}

void GalleryWindow::slotOpenPhoto(const KUrl& url)
{
    new KRun(url, this);
}

void GalleryWindow::slotNewAlbum()
{
    QDialog *dialog = new QDialog(this);
    Ui::GalleryAlbumDialog dlg;
    dlg.setupUi(dialog);
    dlg.titleEdit->setFocus();

// FIXME
//     if ( dlg.show() != QDialog::Accepted )
//     {
//         return;
//     }

    QString name    = dlg.nameEdit->text();
    QString title   = dlg.titleEdit->text();
    QString caption = dlg.captionEdit->text();

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
        KMessageBox::error(this, i18n("Sorry, these characters are not allowed in album name: %1")
                           .arg("\\ / * ? \" \' & < > | . + # ( ) or spaces"));
        return;
    }

    QString parentAlbumName;

    QTreeWidgetItem* item = d->albumView->currentItem();
    if (item) {
        GAlbumViewItem* viewItem = static_cast<GAlbumViewItem*>(item);
        parentAlbumName = viewItem->album.name;
    } else {
        parentAlbumName = "0";
    }

    m_talker->createAlbum(parentAlbumName, name, title, caption);
}

void GalleryWindow::slotAddPhotos()
{
    QTreeWidgetItem* item = d->albumView->currentItem();
    if (!item)
        return;

    KUrl::List urls = KIPIPlugins::ImageDialog::getImageURLs(this, m_interface);
    if (urls.isEmpty())
        return;

    typedef QPair<QString, QString> Pair;

    d->uploadQueue.clear();
    for (KUrl::List::iterator it = urls.begin(); it != urls.end(); ++it)
    {
        KIPI::ImageInfo info = m_interface->info(*it);
        d->uploadQueue.append(Pair((*it).path(), info.description()));
    }

    d->uploadTotal = d->uploadQueue.count();
    d->uploadCount = 0;
    d->progressDlg->reset();
    slotAddPhotoNext();
}

void GalleryWindow::slotAddPhotoNext()
{
    if (d->uploadQueue.isEmpty()) {
        d->progressDlg->reset();
        d->progressDlg->hide();
        slotAlbumSelected();
        return;
    }

    typedef QPair<QString, QString> Pair;
    Pair pathComments = d->uploadQueue.first();
    d->uploadQueue.pop_front();

    bool res = m_talker->addPhoto(d->lastSelectedAlbum, pathComments.first,
                                  pathComments.second,
                                  d->captTitleCheckBox->isChecked(),
                                  d->captDescrCheckBox->isChecked(),
                                  d->resizeCheckBox->isChecked(),
                                  d->dimensionSpinBox->value());
    if (!res) {
        slotAddPhotoFailed("");
        return;
    }

    d->progressDlg->setLabelText(i18n("Uploading file %1 ")
                                .arg(KUrl(pathComments.first).fileName()));

    if (d->progressDlg->isHidden())
        d->progressDlg->show();
}

void GalleryWindow::slotAddPhotoSucceeded()
{
    d->uploadCount++;
    d->progressDlg->setValue(d->uploadCount);   //, m_uploadTotal );
    slotAddPhotoNext();
}

void GalleryWindow::slotAddPhotoFailed(const QString& msg)
{
    if (KMessageBox::warningContinueCancel(this,
                                           i18n("Failed to upload photo into "
                                                "remote gallery. ")
                                           + msg
                                           + i18n("\nDo you want to continue?"))
            != KMessageBox::Continue) {
        d->uploadQueue.clear();
        d->progressDlg->reset();
        d->progressDlg->hide();

        // refresh the thumbnails
        slotAlbumSelected();
    } else {
        d->uploadTotal--;
        d->progressDlg->setValue(d->uploadCount);   //, m_uploadTotal );
        slotAddPhotoNext();
    }
}

void GalleryWindow::slotAddPhotoCancel()
{
    d->uploadQueue.clear();
    d->progressDlg->reset();
    d->progressDlg->hide();

    m_talker->cancel();

    // refresh the thumbnails
    slotAlbumSelected();
}


#include "gallerywindow.moc"
