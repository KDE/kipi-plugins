//////////////////////////////////////////////////////////////////////////////
//
//    DISPLAYCOMPARE.CPP
//
//    Copyright (C) 2001 Richard Groult <rgroult at jalix.org> (from ShowImg project)
//    Copyright (C) 2004 Gilles Caulier <caulier dot gilles at free.fr>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//////////////////////////////////////////////////////////////////////////////

// Include files for Qt

#include <qptrvector.h>
#include <qdict.h>
#include <qfile.h>
#include <qlistbox.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qlistview.h>
#include <qcheckbox.h>
#include <qheader.h>
#include <qlayout.h>
#include <qfileinfo.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qgroupbox.h>
#include <qwhatsthis.h>
#include <qpushbutton.h>

// Include files for KDE

#include <klocale.h>
#include <kmessagebox.h>
#include <kiconloader.h>
#include <klistview.h>
#include <ksqueezedtextlabel.h>
#include <kapplication.h>
#include <kurl.h>
#include <kio/job.h>
#include <kio/jobclasses.h>
#include <kio/netaccess.h>
#include <kio/global.h>
#include <kimageio.h>
#include <kio/previewjob.h>
#include <kapplication.h>
#include <kaboutdata.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kpopupmenu.h>

// Include files for KIPI

#include <libkipi/version.h>

// Local include files

#include "displaycompare.h"

namespace KIPIFindDupplicateImagesPlugin
{

/////////////////////////////////////////////////////////////////////////////////////////////

class FindOriginalItem : public QCheckListItem
{
public:
    FindOriginalItem(QListView * parent, QString const & name, QString const & fullpath,
                       QString const & album, QString const & comments)
                     : QCheckListItem( parent, name, QCheckListItem::CheckBox), _name(name),
                       _fullpath(fullpath), _album (album), _comments (comments)
    {}

    QString name()         { return _name;     }
    QString fullpath()     { return _fullpath; }
    QString album()        { return _album;    }
    QString comments()     { return _comments; }

private:
    QString _name;
    QString _fullpath;
    QString _album;
    QString _comments;
};


/////////////////////////////////////////////////////////////////////////////////////////////

class FindDuplicateItem : public QCheckListItem
{
public:
    FindDuplicateItem(QListView * parent, QString const & name, QString const & fullpath,
                       QString const & album, QString const & comments)
                     : QCheckListItem( parent, name, QCheckListItem::CheckBox), _name(name),
                       _fullpath(fullpath), _album (album), _comments (comments)
    {}

    QString name()         { return _name;     }
    QString fullpath()     { return _fullpath; }
    QString album()        { return _album;    }
    QString comments()     { return _comments; }

private:
    QString _name;
    QString _fullpath;
    QString _album;
    QString _comments;
};


//////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////////

DisplayCompare::DisplayCompare(QWidget* parent, KIPI::Interface* interface, 
                               QDict < QPtrVector < QFile > >* cmp )
              : KDialogBase( parent, "DisplayCompare", true, 0,
                Help|User1|Close, Close, true, i18n("Delete")),
                m_interface( interface )
{
    KImageIO::registerFormats();
    this->cmp = cmp;
    
    // About data and help button.
        
    KAboutData* about = new KAboutData("kipiplugins",
                                       I18N_NOOP("Find Duplicate Images"), 
                                       kipi_version,
                                       I18N_NOOP("A Kipi plugin to find duplicate images\n"
                                                 "This plugin is based on ShowImg implementation algorithm"),
                                       KAboutData::License_GPL,
                                       "(c) 2003-2004, Gilles Caulier", 
                                       0,
                                       "http://extragear.kde.org/apps/kipi.php");
    
    about->addAuthor("Gilles Caulier", I18N_NOOP("Author and maintainer"),
                     "caulier dot gilles at free.fr");

    about->addAuthor("Richard Groult", I18N_NOOP("Find duplicate images algorithm"),
                     "rgroult at jalix.org");
                         
    m_helpButton = actionButton( Help );
    KHelpMenu* helpMenu = new KHelpMenu(this, about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Find Duplicate Images handbook"), this, SLOT(slotHelp()), 0, -1, 0);
    m_helpButton->setPopup( helpMenu->menu() );

    // ----------------------------------------------------
        
    QWidget* box = new QWidget( this );
    setMainWidget(box);

    QVBoxLayout* ml = new QVBoxLayout( box, 10 );
    QHBoxLayout* h1 = new QHBoxLayout( ml );
    QVBoxLayout* v1 = new QVBoxLayout( h1 );
    h1->addSpacing( 5 );
    QGridLayout* g1 = new QGridLayout( v1, 1, 2 );

    //---------------------------------------------

    GroupBox1 = new QGroupBox( 1, Qt::Horizontal, i18n("Original Files"), box );
    GroupBox1->layout()->setSpacing( 6 );
    GroupBox1->layout()->setMargin( 11 );

    OriginalNameLabel = new KSqueezedTextLabel( GroupBox1, "OriginalNameLabel" );
    OriginalNameLabel->setFrameShape( QLabel::Box );
    OriginalNameLabel->setAlignment( int( QLabel::AlignCenter ) );

    preview1 = new QLabel( GroupBox1, "preview1" );
    preview1->setFixedHeight( 120 );
    preview1->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    preview1->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
    QWhatsThis::add( preview1, i18n( "<p>The preview of files with duplicates." ) );

    originalInfoLabel1 = new KSqueezedTextLabel( GroupBox1, "originalInfoLabel1" );
    originalInfoLabel1->setAlignment( int( QLabel::AlignCenter ) );
    originalInfoLabel2 = new KSqueezedTextLabel( GroupBox1, "originalInfoLabel2" );
    originalInfoLabel2->setAlignment( int( QLabel::AlignCenter ) );
    originalInfoLabel3 = new KSqueezedTextLabel( GroupBox1, "originalInfoLabel3" );
    originalInfoLabel3->setAlignment( int( QLabel::AlignCenter ) );
    originalInfoLabel4 = new KSqueezedTextLabel( GroupBox1, "originalInfoLabel4" );
    originalInfoLabel4->setAlignment( int( QLabel::AlignCenter ) );
    originalInfoLabel5 = new KSqueezedTextLabel( GroupBox1, "originalInfoLabel5" );
    originalInfoLabel5->setAlignment( int( QLabel::AlignCenter ) );

    listName = new KListView( GroupBox1, "listName" );
    listName->header()->setLabel( 0, i18n( "Files" ) );
    QWhatsThis::add( listName, i18n( "<p>This list contains all files with many duplicates." ) );
    listName->addColumn( i18n( "Files" ) );
    listName->setResizeMode(QListView::AllColumns);

    g1->addWidget( GroupBox1 , 0, 0);

    //---------------------------------------------

    GroupBox2 = new QGroupBox( 1, Qt::Horizontal, i18n("Similar Files"), box );
    GroupBox2->layout()->setSpacing( 6 );
    GroupBox2->layout()->setMargin( 11 );

    similarNameLabel = new KSqueezedTextLabel( GroupBox2, "similarNameLabel" );
    similarNameLabel->setFrameShape( QLabel::Box );
    similarNameLabel->setAlignment( int( QLabel::AlignCenter ) );

    preview2 = new QLabel( GroupBox2, "preview2" );
    preview2->setFixedHeight( 120 );
    preview2->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    preview2->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
    QWhatsThis::add( preview2, i18n( "<p>The preview of duplicate files." ) );

    similarInfoLabel1 = new KSqueezedTextLabel( GroupBox2, "similarInfoLabel1" );
    similarInfoLabel1->setAlignment( int( QLabel::AlignCenter ) );
    similarInfoLabel2 = new KSqueezedTextLabel( GroupBox2, "similarInfoLabel2" );
    similarInfoLabel2->setAlignment( int( QLabel::AlignCenter ) );
    similarInfoLabel3 = new KSqueezedTextLabel( GroupBox2, "similarInfoLabel3" );
    similarInfoLabel3->setAlignment( int( QLabel::AlignCenter ) );
    similarInfoLabel4 = new KSqueezedTextLabel( GroupBox2, "similarInfoLabel4" );
    similarInfoLabel4->setAlignment( int( QLabel::AlignCenter ) );
    similarInfoLabel5 = new KSqueezedTextLabel( GroupBox2, "similarInfoLabel5" );
    similarInfoLabel5->setAlignment( int( QLabel::AlignCenter ) );

    listEq = new KListView( GroupBox2, "listEq" );
    listEq->header()->setLabel( 0, i18n( "Files" ) );
    QWhatsThis::add( listEq, i18n( "<p>This list contains all duplicates files." ) );
    listEq->addColumn( i18n( "Identical To" ) );
    listEq->setResizeMode(QListView::AllColumns);

    g1->addWidget( GroupBox2 , 0, 1);

    setTabOrder( listName, listEq );

    //---------------------------------------------

    QDictIterator < QPtrVector < QFile > >itres (*cmp);    // iterator for res
    cmp->setAutoDelete(true);
    int n_id = 0;

    while (itres.current ())
        {
        QFileInfo fi(itres.currentKey());
        QString Temp = fi.dirPath();
        QString albumName = Temp.section('/', -1);

        KURL url;
        url.setPath(fi.fileName());
        KIPI::ImageInfo info = m_interface->info(url);
        QString comments = info.description();

        new FindOriginalItem( listName, fi.fileName(), itres.currentKey(), albumName, comments );
        ++itres;
        ++n_id;
        }

    setCaption(i18n("1 Original Image with Duplicate Images Has Been Found",
                    "%n Original Images with Duplicate Images Have Been Found", n_id));

    // signals and slots connections

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(slotDelete()));

    connect(listName, SIGNAL(selectionChanged ( QListViewItem * )),
            this, SLOT(slotDisplayLeft(QListViewItem *)));

    connect(listEq, SIGNAL(selectionChanged ( QListViewItem * )),
            this, SLOT(slotDisplayRight(QListViewItem *)));

    resize( 700, 600 );
    listName->setSelected(listName->firstChild(), true);
}


//////////////////////////////////// DESTRUCTOR /////////////////////////////////////////////

DisplayCompare::~DisplayCompare()
{
}


/////////////////////////////////////////////////////////////////////////////////////////////

void DisplayCompare::slotHelp()
{
    KApplication::kApplication()->invokeHelp("findduplicateimages",
                                             "kipi-plugins");
}


/////////////////////////////////////////////////////////////////////////////////////////////

void DisplayCompare::slotDelete( void )
{
    FindDuplicateItem *item = (FindDuplicateItem*)listEq->firstChild ();
    FindDuplicateItem *itemTmp;

    while(item)    // Remove duplicate file selected.
       {
       if( item->isOn() )
           {
           itemTmp = (FindDuplicateItem*)item->nextSibling();
           KURL deleteImage(item->fullpath());

           if ( KIO::NetAccess::del(deleteImage) == false )
              KMessageBox::error(0, i18n("Cannot remove duplicate file:\n%1").arg(item->fullpath()));
           else
              m_interface->delImage( deleteImage );

           listEq->takeItem (item);
           item = itemTmp;
           }
        else
           item = (FindDuplicateItem*)item->nextSibling();
        }

    // Remove originals files selected.

    for (FindOriginalItem* item = (FindOriginalItem*)listName->firstChild(); item;
         item = (FindOriginalItem*)item->nextSibling())
        {
        if ( item->isOn() )
           {
           KURL deleteImage(item->fullpath());

           if ( KIO::NetAccess::del(deleteImage) == false )
              KMessageBox::error(0, i18n("Cannot remove original file:\n%1").arg(item->fullpath()));

           item->setOn( false );
           }
        }
}


/////////////////////////////////////////////////////////////////////////////////////////////

void DisplayCompare::slotDisplayLeft(QListViewItem * item)
{
    KApplication::setOverrideCursor( waitCursor );
    listEq->clear();
    FindOriginalItem *pitem = static_cast<FindOriginalItem*>( item );
    QPtrVector < QFile > *list = (QPtrVector < QFile > *)cmp->find(pitem->fullpath());
    QImage im = QImage(pitem->fullpath());

    if ( !im.isNull() )
       {
       OriginalNameLabel->setText(pitem->name());
       originalInfoLabel1->setText(i18n("Image size: %1x%2 pixels").arg(im.width()).arg(im.height()));
       originalInfoLabel2->setText(i18n("File size: 1 byte",
                                        "File size: %n bytes",QFileInfo(pitem->fullpath()).size()));
       originalInfoLabel3->setText(i18n("Modified: %1").arg(KLocale(NULL)
                                   .formatDateTime(QFileInfo(pitem->fullpath())
                                   .lastModified())));
       originalInfoLabel4->setText(i18n("Album: %1").arg(pitem->album()));
       originalInfoLabel5->setText(i18n("Comments: %1").arg(pitem->comments()));
       }

    preview1->clear();

    QString IdemIndexed = "file:" + pitem->fullpath();
    KURL url(IdemIndexed);

    KIO::PreviewJob* thumbJob1 = KIO::filePreview( url, preview1->height() );

    connect(thumbJob1, SIGNAL(gotPreview(const KFileItem*, const QPixmap&)),
            SLOT(slotGotPreview1(const KFileItem*, const QPixmap&)));

    FindDuplicateItem *last = NULL;
    QFile *f = NULL;
    QFileInfo *fi = new QFileInfo();
    QString fn;

    for (unsigned int i = 0 ; i < list->size () ; ++i)
       {
       f = (QFile*)list->at(i);
       fi->setFile(*f);
       fn =  fi->absFilePath();

       if (fi->exists ())
          {
          QString Temp = fi->dirPath();
          QString albumName = Temp.section('/', -1);

          KURL url;
          url.setPath(fi->fileName());
          KIPI::ImageInfo info = m_interface->info(url);
          QString comments = info.description();

          FindDuplicateItem *item = new FindDuplicateItem( listEq,
                                                           fi->fileName(),
                                                           fn,
                                                           albumName,
                                                           comments
                                                           );
          if (!last) 
             last = item;
          }
       }

    preview2->setPixmap(QPixmap());
    listEq->setSelected(last, true);
    KApplication::restoreOverrideCursor();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void DisplayCompare::slotGotPreview1(const KFileItem* /*url*/, const QPixmap &pixmap)
{
    preview1->setPixmap(pixmap);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void DisplayCompare::slotDisplayRight(QListViewItem * item)
{
    KApplication::setOverrideCursor( waitCursor );
    FindDuplicateItem *pitem = static_cast<FindDuplicateItem*>( item );
    QImage im = QImage(pitem->fullpath());

    if ( !im.isNull() )
       {
       similarNameLabel->setText(pitem->name());
       similarInfoLabel1->setText(i18n("Image size: %1x%2 pixels").arg(im.width()).arg(im.height()));
       similarInfoLabel2->setText(i18n("File size: 1 byte",
                                       "File size: %n bytes", QFileInfo(pitem->fullpath()).size()));
       similarInfoLabel3->setText(i18n("Modified: %1").arg(KLocale(NULL)
                                                      .formatDateTime(QFileInfo(pitem->fullpath())
                                                      .lastModified())));
       similarInfoLabel4->setText(i18n("Album: %1").arg(pitem->album()));
       similarInfoLabel5->setText(i18n("Comments: %1").arg(pitem->comments()));
       }

    preview2->clear();

    QString IdemIndexed = "file:" + pitem->fullpath();
    KURL url(IdemIndexed);

    KIO::PreviewJob* thumbJob2 = KIO::filePreview( url, preview2->height() );

    connect(thumbJob2, SIGNAL(gotPreview(const KFileItem*, const QPixmap&)),
            SLOT(slotGotPreview2(const KFileItem*, const QPixmap&)));

    KApplication::restoreOverrideCursor();
    }


/////////////////////////////////////////////////////////////////////////////////////////////

void DisplayCompare::slotGotPreview2(const KFileItem* /*url*/, const QPixmap &pixmap)
{
    preview2->setPixmap(pixmap);
}

}  // NameSpace KIPIFindDupplicateImagesPlugin

#include "displaycompare.moc"
