/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-10-01
 * Description : a plugin to export to a remote Gallery server.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006      by Colin Guthrie <kde at colin dot guthr dot ie>
 * Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008      by Andrea Diamantini <adjam7 at gmail dot com>
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

#include "gallerywindow.moc"

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
#include <QSpacerItem>

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
#include <libkipi/imagecollection.h>

// Local includes

#include "albumdlg.h"
#include "galleries.h"
#include "galleryconfig.h"
#include "galleryitem.h"
#include "gallerytalker.h"
#include "kpimagedialog.h"
#include "kpaboutdata.h"
#include "kpimageinfo.h"

namespace KIPIGalleryExportPlugin
{

class GalleryWindow::Private
{
public:

    Private(GalleryWindow* const parent);

    QWidget*               widget;
    QTreeWidget*           albumView;
    QPushButton*           newAlbumBtn;
    QPushButton*           addPhotoBtn;
    QCheckBox*             resizeCheckBox;
    QSpinBox*              dimensionSpinBox;
    QHash<QString, GAlbum> albumDict;
    KUrlLabel*             logo;
    Gallery*               gallery;
    GalleryTalker*         talker;
    QProgressDialog*       progressDlg;
    unsigned int           uploadCount;
    unsigned int           uploadTotal;
    QStringList*           uploadList;
    QString                firstAlbumName;
};

GalleryWindow::Private::Private(GalleryWindow* const parent)
{
    gallery     = 0;
    talker      = 0;
    progressDlg = 0;
    uploadList  = 0;
    uploadCount = 0;
    uploadTotal = 0;
    widget      = new QWidget(parent);

    parent->setMainWidget(widget);
    parent->setModal(false);

    QHBoxLayout* const hlay = new QHBoxLayout(widget);

    // ---------------------------------------------------------------------------

    logo = new KUrlLabel;
    logo->setText(QString());
    logo->setUrl("http://gallery.menalto.com");
    logo->setPixmap(QPixmap(KStandardDirs::locate("data", "kipiplugin_galleryexport/pics/gallery_logo.png")));
    logo->setAlignment(Qt::AlignLeft);

    // ---------------------------------------------------------------------------

    albumView = new QTreeWidget;
    QStringList labels;
    labels << i18n("Albums"); // << i18n("ID");
    albumView->setHeaderLabels(labels);

    // ---------------------------------------------------------------------------

    QFrame* const optionFrame = new QFrame;
    QVBoxLayout* const vlay   = new QVBoxLayout();

    newAlbumBtn = new QPushButton;
    newAlbumBtn->setText(i18n("&New Album"));
    newAlbumBtn->setIcon(KIcon("folder-new"));
    newAlbumBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    newAlbumBtn->setEnabled(false);

    addPhotoBtn = new QPushButton;
    addPhotoBtn->setText(i18n("&Add Selected Photos"));
    addPhotoBtn->setIcon(KIcon("list-add"));
    addPhotoBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    addPhotoBtn->setEnabled(false);

    QGroupBox* const optionsBox = new QGroupBox(i18n("Override Default Options"));
    QVBoxLayout* const vlay2    = new QVBoxLayout();

    resizeCheckBox = new QCheckBox(optionsBox);
    resizeCheckBox->setText(i18n("Resize photos before uploading"));

    QHBoxLayout* const hlay2    = new QHBoxLayout;
    QLabel* const resizeLabel   = new QLabel(i18n("Maximum dimension:"));
    dimensionSpinBox            = new QSpinBox;
    dimensionSpinBox->setRange(1,3200);
    dimensionSpinBox->setValue(600);

    resizeCheckBox->setChecked(false);
    dimensionSpinBox->setEnabled(false);

    // ---------------------------------------------------------------------------

    hlay2->addWidget(resizeLabel);
    hlay2->addWidget(dimensionSpinBox);
    hlay2->setSpacing(KDialog::spacingHint());
    hlay2->setMargin(KDialog::spacingHint());

    // ---------------------------------------------------------------------------

    vlay2->addWidget(resizeCheckBox);
    vlay2->addLayout(hlay2);
    vlay2->addItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
    vlay2->setSpacing(KDialog::spacingHint());
    vlay2->setMargin(KDialog::spacingHint());

    optionsBox->setLayout(vlay2);

    // ---------------------------------------------------------------------------

    vlay->addWidget(newAlbumBtn);
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

GalleryWindow::GalleryWindow(QWidget* const parent, Gallery* const pGallery)
    : KPToolDialog(parent),
      d(new Private(this))
{
    d->gallery = pGallery;

    setWindowTitle( i18n("Gallery Export") );
    setButtons( KDialog::Close | KDialog::User1 | KDialog::Help);
    setModal(false);

    KPAboutData* const about = new KPAboutData(ki18n("Gallery Export"),
                                               0,
                                               KAboutData::License_GPL,
                                               ki18n("A Kipi plugin to export image collections to a remote Gallery server."),
                                               ki18n("(c) 2003-2005, Renchi Raju\n"
                                                     "(c) 2006-2007, Colin Guthrie\n"
                                                     "(c) 2006-2013, Gilles Caulier\n"
                                                     "(c) 2008, Andrea Diamantini\n"));

    about->addAuthor(ki18n("Renchi Raju"), ki18n("Author"),
                     "renchi dot raju at gmail dot com");

    about->addAuthor(ki18n("Colin Guthrie"), ki18n("Maintainer"),
                     "kde at colin dot guthr dot ie");

    about->addAuthor(ki18n("Andrea Diamantini"), ki18n("Developer"),
                     "adjam7 at gmail dot com");

    about->addAuthor(ki18n("Gilles Caulier"), ki18n("Developer"),
                     "caulier dot gilles at gmail dot com");

    about->setHandbookEntry("galleryexport");
    setAboutData(about);

    // User1 Button : to conf gallery settings
    KPushButton* const confButton = button( User1 );
    confButton->setText( i18n("Settings") );
    confButton->setIcon( KIcon("configure") );

    connect(confButton, SIGNAL(clicked()),
            this, SLOT(slotSettings()) );

    // we need to let d->talker work..
    d->talker      = new GalleryTalker(d->widget);

    // setting progressDlg and its numeric hints
    d->progressDlg = new QProgressDialog(this);
    d->progressDlg->setModal(true);
    d->progressDlg->setAutoReset(true);
    d->progressDlg->setAutoClose(true);
    d->uploadList  = new QStringList;

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

    group.writeEntry("Resize",          d->resizeCheckBox->isChecked());
    group.writeEntry("Maximum Width",   d->dimensionSpinBox->value());

    delete d->uploadList;

    delete d;
}

void GalleryWindow::connectSignals()
{
    connect(d->albumView, SIGNAL(itemSelectionChanged()),
            this , SLOT(slotAlbumSelected()) );

    connect(d->newAlbumBtn, SIGNAL(clicked()),
            this, SLOT(slotNewAlbum()));

    connect(d->addPhotoBtn, SIGNAL(clicked()),
            this, SLOT(slotAddPhoto()));

    connect(d->resizeCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(slotEnableSpinBox(int)));

    connect(d->logo, SIGNAL(leftClickedUrl(QString)),
            this, SLOT(slotProcessUrl(QString)));

    connect(d->progressDlg, SIGNAL(canceled()),
            this, SLOT(slotAddPhotoCancel()));

    connect(d->talker, SIGNAL(signalError(QString)),
            this, SLOT(slotError(QString)));

    connect(d->talker, SIGNAL(signalBusy(bool)),
            this, SLOT(slotBusy(bool)));

    connect(d->talker, SIGNAL(signalLoginFailed(QString)),
            this, SLOT(slotLoginFailed(QString)));

    connect(d->talker, SIGNAL(signalAlbums(QList<GAlbum>)),
            this, SLOT(slotAlbums(QList<GAlbum>)));

    connect(d->talker, SIGNAL(signalPhotos(QList<GPhoto>)),
            this, SLOT(slotPhotos(QList<GPhoto>)));

    connect(d->talker, SIGNAL(signalAddPhotoSucceeded()),
            this, SLOT(slotAddPhotoSucceeded()));

    connect(d->talker, SIGNAL(signalAddPhotoFailed(QString)),
            this, SLOT(slotAddPhotoFailed(QString)));
}

void GalleryWindow::slotProcessUrl(const QString& url)
{
    KToolInvocation::self()->invokeBrowser(url);
}

void GalleryWindow::readSettings()
{
    // read Config
    KConfig config("kipirc");
    KConfigGroup group = config.group("GallerySync Galleries");

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
}

void GalleryWindow::slotDoLogin()
{
    GalleryTalker::setGallery2((2 == d->gallery->version()));
    KUrl url(d->gallery->url());

    if (url.protocol().isEmpty())
    {
        url.setProtocol("http");
        url.setHost(d->gallery->url());
    }

    if (!url.url().endsWith(QLatin1String(".php")))
    {
        if (GalleryTalker::isGallery2())
            url.addPath("main.php");
        else
            url.addPath("gallery_remote2.php");
    }

    // If we've done something clever, save it back to the gallery.
    if (d->gallery->url() != url.url())
    {
        d->gallery->setUrl(url.url());
        d->gallery->save();
    }

    d->talker->login(url.url(), d->gallery->username(), d->gallery->password());
}

void GalleryWindow::slotLoginFailed(const QString& msg)
{
    if (KMessageBox::warningYesNo(this,
                                  i18n("Failed to login into remote gallery. ")
                                  + msg
                                  + i18n("\nDo you want to check your settings and try again?"))
            != KMessageBox::Yes)
    {
        close();
        return;
    }

    QPointer<GalleryEdit> configDlg = new GalleryEdit(kapp->activeWindow(), d->gallery, i18n("Edit Gallery Data") );

    if ( configDlg->exec() != QDialog::Accepted )
    {
        delete configDlg;
        return;
    }

    slotDoLogin();
    delete configDlg;
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
        bool loggedIn = d->talker->loggedIn();
        d->newAlbumBtn->setEnabled(loggedIn);
        d->addPhotoBtn->setEnabled(loggedIn && d->albumView->currentItem());
    }
}

void GalleryWindow::slotError(const QString& msg)
{
    d->progressDlg->hide();
    KMessageBox::error(this, msg);
}

// FIXME
void GalleryWindow::slotAlbums(const QList<GAlbum>& albumList)
{
    d->albumDict.clear();
    d->albumView->clear();

    // album work list
    QList<GAlbum> workList(albumList);
    QList<QTreeWidgetItem *> parentItemList;

    // fill QTreeWidget
    while( !workList.isEmpty() )
    {
        // the album to work on
        GAlbum album     = workList.takeFirst();
        int parentRefNum = album.parent_ref_num;

        if ( parentRefNum == 0 )
        {
            QTreeWidgetItem* const item = new QTreeWidgetItem();
            item->setText(0, cleanName(album.title) );
            item->setIcon(0, KIcon("inode-directory") );
            item->setText(1, album.name );
            d->firstAlbumName = album.name;
            item->setText(2, i18n("Album") );
            item->setText(3, QString::number(album.ref_num) );

            d->albumView->addTopLevelItem(item);
            d->albumDict.insert(album.title, album);
            parentItemList << item;
        }
        else
        {
            QTreeWidgetItem* parentItem = 0;
            bool found                  = false;
            int i                       = 0;

            while( !found && i < parentItemList.size() )
            {
                parentItem = parentItemList.at(i);

                if(parentItem && (parentItem->text(3) == QString::number(parentRefNum)))
                {
                    QTreeWidgetItem* const item = new QTreeWidgetItem(parentItem);
                    item->setText(0, cleanName(album.title) );
                    item->setIcon(0, KIcon("inode-directory") );
                    item->setText(1, album.name );
                    item->setText(2, i18n("Album") );
                    item->setText(3, QString::number(album.ref_num) );

                    d->albumDict.insert(album.title, album);
                    parentItemList << item;
                    found = true;
                }
                i++;
            }

            if ( i == parentItemList.size() )
            {
                workList.append(album);
            }
        }
    }
}

// FIXME: avoid duplications
void GalleryWindow::slotPhotos(const QList<GPhoto>& photoList)
{
    QTreeWidgetItem* const parentItem = d->albumView->currentItem();
    typedef QList<GPhoto> GPhotoList;
    GPhotoList::const_iterator iterator;

    for (iterator = photoList.begin(); iterator != photoList.end(); ++iterator)
    {
        QString plain               = (*iterator).caption;
        QTreeWidgetItem* const item = new QTreeWidgetItem(parentItem);
        item->setText(0, cleanName(plain) );
        item->setIcon(0, KIcon("image-x-generic") );
        item->setText(1, (*iterator).name);
        item->setText(2, i18n("Image") );
    }
}

void GalleryWindow::slotAlbumSelected()
{
    QTreeWidgetItem* const item = d->albumView->currentItem();

    if (!item)
    {
        d->addPhotoBtn->setEnabled(false);
        return;
    }

    // stop loading if user clicked an image
    if( item->text(2) == i18n("Image") )
    {
        return;
    }

    QString albumName = item->text(1);

    if (d->talker->loggedIn() && !albumName.isEmpty() )
    {
        d->addPhotoBtn->setEnabled(true);
        d->talker->listPhotos(albumName);
    }
    else
    {
        d->addPhotoBtn->setEnabled(false);
    }
}

void GalleryWindow::slotNewAlbum()
{
    QPointer<AlbumDlg> dlg = new AlbumDlg(d->widget);
    dlg->titleEdit->setFocus();

    if ( dlg->exec() != QDialog::Accepted )
    {
        delete dlg;
        return;
    }

    QString name    = dlg->nameEdit->text();
    QString title   = dlg->titleEdit->text();
    QString caption = dlg->captionEdit->text();

    delete dlg;

    // check for prohibited chars in the album name
    // \ / * ? " ' & < > | . + # ( ) or spaces
    // TODO: Change this to a QRegExp check.
    QChar ch;
    bool  clean = true;

    for (int i = 0; i < name.length(); ++i)
    {
        ch = name[i];
        if (ch == '\\')
        {
            clean = false;
            break;
        }
        else if (ch == '/')
        {
            clean = false;
            break;
        }
        else if (ch == '*')
        {
            clean = false;
            break;
        }
        else if (ch == '?')
        {
            clean = false;
            break;
        }
        else if (ch == '"')
        {
            clean = false;
            break;
        }
        else if (ch == '\'')
        {
            clean = false;
            break;
        }
        else if (ch == '&')
        {
            clean = false;
            break;
        }
        else if (ch == '<')
        {
            clean = false;
            break;
        }
        else if (ch == '>')
        {
            clean = false;
            break;
        }
        else if (ch == '|')
        {
            clean = false;
            break;
        }
        else if (ch == '.')
        {
            clean = false;
            break;
        }
        else if (ch == '+')
        {
            clean = false;
            break;
        }
        else if (ch == '#')
        {
            clean = false;
            break;
        }
        else if (ch == '(')
        {
            clean = false;
            break;
        }
        else if (ch == ')')
        {
            clean = false;
            break;
        }
/*
        else if (ch == ' ')
        {
            clean = false;
            break;
        }
*/
    }

    if (!clean)
    {
        KMessageBox::error(this, i18n("Sorry, these characters are not allowed in album name: \\ / * ? \" \' & &lt; &gt; | . + # ( ) or spaces"));
        return;
    }

    QString parentAlbumName;

    QTreeWidgetItem* const item = d->albumView->currentItem();
    int column                  = d->albumView->currentColumn();

    if (item)
    {
        const GAlbum& album = d->albumDict.value( item->text(column) );
        parentAlbumName     = album.name;
        d->talker->createAlbum( parentAlbumName, name, title, caption);
    }
    else
    {
        d->talker->createAlbum( d->firstAlbumName, name, title, caption );
    }
}

void GalleryWindow::slotAddPhoto()
{
    QTreeWidgetItem* const item = d->albumView->currentItem();
    int column                  = d->albumView->currentColumn();

    if (!item)
        return;     // NO album selected: FIXME: do something

    // albumName
    QString albumTitle = item->text(column);

    if(!d->albumDict.contains(albumTitle))
        return;     // NO album name found: FIXME: do something

    // photoPath
    const KUrl::List urls(iface()->currentSelection().images());

    if (urls.isEmpty())
        return; // NO photo selected: FIXME: do something

    for (KUrl::List::ConstIterator it = urls.constBegin(); it != urls.constEnd(); ++it)
    {
        d->uploadList->append( (*it).path() );
    }

    d->uploadTotal = d->uploadList->count();
    d->progressDlg->reset();
    d->progressDlg->setMaximum(d->uploadTotal);
    d->uploadCount = 0;
    slotAddPhotoNext();
}

void GalleryWindow::slotAddPhotoNext()
{
    if ( d->uploadList->isEmpty() )
    {
        d->progressDlg->reset();
        d->progressDlg->hide();
        slotAlbumSelected();        // ?
        return;
    }

    QTreeWidgetItem* const item = d->albumView->currentItem();
    int column                  = d->albumView->currentColumn();
    QString albumTitle          = item->text(column);
    const GAlbum& album         = d->albumDict.value(albumTitle);
    QString photoPath           = d->uploadList->takeFirst();
    KPImageInfo info(photoPath);
    QString title               = info.title();
    QString description         = info.description();
    bool res                    = d->talker->addPhoto(album.name, photoPath, title, description,
                                                      d->resizeCheckBox->isChecked(),
                                                      d->dimensionSpinBox->value() );

    if (!res)
    {
        slotAddPhotoFailed( "" );
        return;
    }

    d->progressDlg->setLabelText( i18n("Uploading file %1", KUrl(photoPath).fileName()) );

    if (d->progressDlg->isHidden())
        d->progressDlg->show();
}

void GalleryWindow::slotAddPhotoSucceeded()
{
    d->uploadCount++;
    d->progressDlg->setValue(d->uploadCount);
    slotAddPhotoNext();
}

void GalleryWindow::slotAddPhotoFailed(const QString& msg)
{
    d->progressDlg->reset();
    d->progressDlg->hide();

    if (KMessageBox::warningContinueCancel(this,
                                           i18n("Failed to upload photo into "
                                                "remote gallery. ")
                                           + msg
                                           + i18n("\nDo you want to continue?"))
            != KMessageBox::Continue)
    {
    }
    else
    {
        slotAddPhotoNext();
    }
}

void GalleryWindow::slotAddPhotoCancel()
{
    d->progressDlg->reset();
    d->progressDlg->hide();
    d->talker->cancel();
}

void GalleryWindow::slotEnableSpinBox(int n)
{
    bool b;

    switch(n)
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

    d->dimensionSpinBox->setEnabled(b);
}

void GalleryWindow::slotSettings()
{
    // TODO: reload albumlist if OK slot used.
    QPointer<GalleryEdit> dlg = new GalleryEdit(kapp->activeWindow(), d->gallery, i18n("Edit Gallery Data") );

    if( dlg->exec() == QDialog::Accepted )
    {
        slotDoLogin();
    }

    delete dlg;
}

QString GalleryWindow::cleanName(const QString& str) const
{
    QString plain = str;
    plain.replace("&lt;", "<");
    plain.replace("&gt;", ">");
    plain.replace("&quot;", "\"");
    plain.replace("&amp;", "&");

    return plain;
}

} // namespace KIPIGalleryExportPlugin
