/* ============================================================
 * File  : cdarchivingdialog.cpp
 * Author: Gilles Caulier <caulier dot gilles at free.fr>
 * Date  : 2003-09-05
 * Description : CD archiving dialog
 *
 * Copyright 2003-2004 by Gregory Kokanosky <gregory dot kokanosky at free.fr>
 * for images navigation mode patchs.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Include files for Qt

#include <qlabel.h>
#include <qvbox.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qwhatsthis.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qlistview.h>
#include <qheader.h>
#include <qpushbutton.h>
#include <qfileinfo.h>

// Include files for KDE

#include <klocale.h>
#include <kconfig.h>
#include <kfontdialog.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <kurlrequester.h>
#include <kurl.h>
#include <kdirsize.h>
#include <klineedit.h>
#include <knuminput.h>
#include <kcolorbutton.h>
#include <kglobalsettings.h>
#include <kmessagebox.h>
#include <kbuttonbox.h>
#include <kapplication.h>
#include <ksqueezedtextlabel.h>
#include <kio/previewjob.h>

// Include files for KIPI

#include <libkipi/imagecollection.h>
#include <libkipi/imageinfo.h>

// Local include files

#include "cdarchivingdialog.h"

namespace KIPICDArchivingPlugin
{

KIO::filesize_t TargetMediaSize;

class AlbumItem : public QCheckListItem
{
public:
    AlbumItem(QListView * parent, KIPI::ImageCollection album,
              QString const & collection, QDate const & date,
              KIO::filesize_t const & size)
            : QCheckListItem( parent, album.name(), QCheckListItem::CheckBox), _album(album),
              _collection(collection), _date (date), _size (size)
    {}
    KIPI::ImageCollection album() { return _album;                  }
    QString comments()            { return _album.comment();        }
    QString name()                { return _album.name();           }
    KURL    path()                { return _album.path();           }
    QString collection()          { return _album.category();       }
    KURL    firstImage()          { return _album.images().first(); }
    QDate   date()                { return _date;                   }
    KIO::filesize_t size()        { return _size;                   }
    int     items()               { return _album.images().count(); }

    void stateChange ( bool )
       {
       if ( AlbumItem::isOn() == true ) TargetMediaSize = TargetMediaSize + _size;
       else TargetMediaSize = TargetMediaSize - _size;
       }

private:
    KIPI::ImageCollection _album;
    QString               _collection;
    QDate                 _date;
    KIO::filesize_t       _size;
};


/////////////////////////////////////////////////////////////////////////////////////////////

CDArchivingDialog::CDArchivingDialog( KIPI::Interface* interface, QWidget *parent)
                 : KDialogBase( IconList, i18n("Configure"), Help|Ok|Cancel, Ok, parent,
                                "CDArchivingDialog", true, true ), m_interface( interface )
{
    setCaption(i18n("Albums CD archiving"));
    setupSelection();
    setupLookPage();
    setupCDInfos();
    setupBurning();
    aboutPage();
    page_setupSelection->setFocus();
    setHelp("cdarchiving", "kipi-plugins");
    setAlbumsList();
}


/////////////////////////////////////////////////////////////////////////////////////////////

CDArchivingDialog::~CDArchivingDialog()
{
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::setupSelection(void)
{
    page_setupSelection = addPage(i18n("Selection"), i18n("Albums selection"),
                                  BarIcon("folder_image", KIcon::SizeMedium));

    QVBoxLayout *layout = new QVBoxLayout(page_setupSelection, 0, spacingHint() );
    TargetMediaSize = 0L;

    //---------------------------------------------

    QGroupBox * groupBox1 = new QGroupBox( page_setupSelection );
    groupBox1->setFlat(false);
    groupBox1->setTitle(i18n("Select Albums to archive"));
    QGridLayout* grid = new QGridLayout( groupBox1, 2, 2 , 20, 20);

    m_AlbumsList = new QListView( groupBox1 );
    m_AlbumsList->setResizeMode( QListView::LastColumn );
    m_AlbumsList->addColumn("");
    m_AlbumsList->header()->hide();
    QWhatsThis::add( m_AlbumsList, i18n("<p>Selected here the Albums to archive in CD.") );

    grid->addMultiCellWidget(m_AlbumsList, 0, 2, 0, 1);

    KButtonBox* albumSelectionButtonBox = new KButtonBox( groupBox1, Vertical );
    QPushButton* buttonSelectAll = albumSelectionButtonBox->addButton ( i18n( "&Select all" ) );
    QWhatsThis::add( buttonSelectAll, i18n("<p>Select all Albums in the list.") );
    QPushButton* buttonInvertSelection = albumSelectionButtonBox->addButton ( i18n( "&Invert selection" ));
    QWhatsThis::add( buttonInvertSelection, i18n("<p>Invert the Albums selection in the list.") );
    QPushButton* buttonSelectNone = albumSelectionButtonBox->addButton ( i18n( "Select &none" ) );
    QWhatsThis::add( buttonSelectNone, i18n("<p>Deselect all Albums in the list.") );
    albumSelectionButtonBox->layout();
    grid->addMultiCellWidget(albumSelectionButtonBox, 0, 1, 2, 2);

    m_albumPreview = new QLabel( groupBox1 );
    m_albumPreview->setFixedHeight( 120 );
    m_albumPreview->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    m_albumPreview->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
    QWhatsThis::add( m_albumPreview, i18n( "Preview of the first image in the current selected Album." ) );
    grid->addMultiCellWidget(m_albumPreview, 2, 2, 2, 2);

    layout->addWidget( groupBox1 );

    //---------------------------------------------

    QGroupBox * groupBox2 = new QGroupBox( i18n("Album description"), page_setupSelection );
    groupBox2->setColumnLayout(0, Qt::Vertical );
    groupBox2->layout()->setSpacing( 6 );
    groupBox2->layout()->setMargin( 11 );
    QWhatsThis::add( groupBox2, i18n("<p>The description of the current Album in the selection list.") );

    QVBoxLayout * groupBox2Layout = new QVBoxLayout( groupBox2->layout() );
    groupBox2Layout->setAlignment( Qt::AlignTop );

    m_AlbumComments = new KSqueezedTextLabel( groupBox2 );
    m_AlbumComments->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );
    groupBox2Layout->addWidget( m_AlbumComments );

    m_AlbumCollection = new KSqueezedTextLabel( groupBox2 );
    m_AlbumCollection->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );
    groupBox2Layout->addWidget( m_AlbumCollection );

    m_AlbumDate = new KSqueezedTextLabel( groupBox2 );
    m_AlbumDate->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );
    groupBox2Layout->addWidget( m_AlbumDate );

    m_AlbumItems = new KSqueezedTextLabel( groupBox2 );
    m_AlbumItems->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );
    groupBox2Layout->addWidget( m_AlbumItems );

    m_AlbumSize = new KSqueezedTextLabel( groupBox2 );
    m_AlbumSize->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );
    groupBox2Layout->addWidget( m_AlbumSize );

    layout->addWidget( groupBox2 );

    //---------------------------------------------

    QGroupBox * groupBox3 = new QGroupBox( 2, Qt::Horizontal, i18n("Target media informations"),
                                           page_setupSelection );
    groupBox3->layout()->setSpacing( 6 );
    groupBox3->layout()->setMargin( 11 );
    QWhatsThis::add( groupBox3, i18n("<p>The informations about the backup media.") );

    m_mediaSize = new QLabel( groupBox3 );
    m_mediaSize->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );

    m_mediaFormat = new QComboBox(false, groupBox3);
    m_mediaFormat->insertItem(i18n("CD (650Mb)"));
    m_mediaFormat->insertItem(i18n("CD (700Mb)"));
    m_mediaFormat->insertItem(i18n("CD (880Mb)"));
    m_mediaFormat->insertItem(i18n("DVD (4,7Gb)"));
    m_mediaFormat->setCurrentText (i18n("CD (650Mb)"));
    mediaFormatActived(m_mediaFormat->currentText());
    QWhatsThis::add( m_mediaFormat, i18n("<p>Select here the backup media format."));

    layout->addWidget( groupBox3 );
    layout->addStretch(1);

    //---------------------------------------------

    connect( buttonSelectAll, SIGNAL( clicked() ),
             this, SLOT( slotbuttonSelectAll() ) );

    connect( buttonInvertSelection, SIGNAL( clicked() ),
             this, SLOT( slotbuttonInvertSelection() ) );

    connect( buttonSelectNone, SIGNAL( clicked() ),
             this, SLOT( slotbuttonSelectNone() ) );

    connect( m_mediaFormat, SIGNAL( highlighted( const QString & ) ),
             this, SLOT( mediaFormatActived( const QString & ) ) );

    connect( m_AlbumsList, SIGNAL( currentChanged( QListViewItem * ) ),
             this, SLOT( albumSelected( QListViewItem * ) ) );
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::setAlbumsList(void)
{
    AlbumItem *currentAlbum = 0; 

    QValueList<KIPI::ImageCollection> albums = m_interface->allAlbums();

    for( QValueList<KIPI::ImageCollection>::Iterator albumIt = albums.begin() ;
         albumIt != albums.end() ; ++albumIt ) 
        {
        KURL::List images = (*albumIt).images();
        int size = 0;
        QDateTime newestDate;
        
        for( KURL::List::Iterator urlIt = images.begin() ; urlIt != images.end() ; ++urlIt ) 
            {
            KIPI::ImageInfo info = m_interface->info( *urlIt );
            size += info.size();
            
            if ( info.time() > newestDate )
                newestDate = info.time();
            }

        AlbumItem *item = new AlbumItem( m_AlbumsList, 
                                         *albumIt,
                                         (*albumIt).category(),
                                         newestDate.date(),
                                         size/1024 );

        if ( m_interface->currentAlbum().isValid() )
           {
           if ( (*albumIt).name() == m_interface->currentAlbum().name() )
              {
              item->setOn(true);
              item->setSelected(true);
              albumSelected( item );
              currentAlbum = item;
              }
           else
              item->setOn(false);
           }
        }

    if (currentAlbum != 0)
        m_AlbumsList->ensureItemVisible(currentAlbum);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::setupLookPage(void)
{
    QString whatsThis;
    page_setupLook = addPage( i18n("HTML interface"), i18n("HTML interface Look"),
                              BarIcon("html", KIcon::SizeMedium ) );

    QVBoxLayout *vlay = new QVBoxLayout( page_setupLook, 0, spacingHint() );

    //---------------------------------------------

    m_useHTMLInterface = new QCheckBox( i18n("Build CD HTML interface"), page_setupLook);
    m_useHTMLInterface->setChecked( true );
    vlay->addWidget( m_useHTMLInterface );
    QWhatsThis::add( m_useHTMLInterface,
                     i18n("<p>This option adding an HTML interface for browse the CD content.") );

    //---------------------------------------------

    m_useAutoRunWin32 = new QCheckBox( i18n("Add \"autorun\" fonctionality"), page_setupLook);
    m_useAutoRunWin32->setChecked( true );
    vlay->addWidget( m_useAutoRunWin32 );
    QWhatsThis::add( m_useAutoRunWin32,
                     i18n("<p>This option adding the Ms Windows(tm) autoruning capability in the CD.") );

    //---------------------------------------------

    m_labelTitle = new QLabel( i18n("Archiving title:"), page_setupLook);
    vlay->addWidget( m_labelTitle );

    m_title = new QLineEdit(i18n("Albums Archiving"), page_setupLook);
    vlay->addWidget( m_title );
    m_labelTitle->setBuddy(m_title);
    QWhatsThis::add( m_title, i18n("<p>Enter here the title of the CD archiving.") );

    //---------------------------------------------

    m_imagesPerRow = new KIntNumInput(4, page_setupLook);
    m_imagesPerRow->setRange(1, 8, 1, true );
    m_imagesPerRow->setLabel( i18n("I&mages per row:") );
    QWhatsThis::add( m_imagesPerRow, i18n("<p>Enter here the number of images per row in the album page. "
                                          "A good value is '4'.") );
    vlay->addWidget( m_imagesPerRow );

    QGridLayout *grid = new QGridLayout( 2, 2 );
    vlay->addLayout( grid );

    //---------------------------------------------

    m_thumbnailsSize = new KIntNumInput(140, page_setupLook);
    m_thumbnailsSize->setRange(10, 1000, 1, true );
    m_thumbnailsSize->setLabel( i18n("Thumbnails size:") );
    vlay->addWidget( m_thumbnailsSize );
    QWhatsThis::add( m_thumbnailsSize, i18n("<p>The new size of thumbnails in pixels") );

    //---------------------------------------------

    QHBoxLayout *hlay3 = new QHBoxLayout( spacingHint() );
    vlay->addLayout( hlay3 );
    m_imageFormat = new QComboBox(false, page_setupLook);
    m_imageFormat->insertItem("JPEG");
    m_imageFormat->insertItem("PNG");
    m_imageFormat->setCurrentText ("JPEG");
    whatsThis = i18n("<p>Select here the image file format for thumbnails.<p>");
    whatsThis = whatsThis + i18n("<b>JPEG</b>: The Joint Photographic Experts Group file format is a "
                "good Web file format but it use a compression with data lost.<p>"
                "<b>PNG</b>: the Portable Network Graphics format is an extensible file format for "
                "the lossless, portable, well-compressed storage of raster images. PNG provides a "
                "patent-free replacement for GIF and can also replace many common uses of TIFF. "
                "PNG is designed to work well in online viewing applications, such as the World "
                "Wide Web, so it is fully streamable with a progressive display option. Also, "
                "PNG can store gamma and chromaticity data for improved color matching on "
                "heterogeneous platforms.");
    QWhatsThis::add( m_imageFormat, whatsThis );

    m_labelThumbsFileFormat = new QLabel( i18n("Thumbnails file format:"), page_setupLook);
    hlay3->addWidget( m_labelThumbsFileFormat );
    m_labelThumbsFileFormat->setBuddy( m_imageFormat );
    hlay3->addStretch( 1 );
    hlay3->addWidget( m_imageFormat );

    //---------------------------------------------

    QHBoxLayout *hlay11  = new QHBoxLayout( );
    vlay->addLayout( hlay11 );

    m_fontName = new QComboBox( false, page_setupLook );
    QStringList standardFonts;
    KFontChooser::getFontList(standardFonts, 0);
    m_fontName->insertStringList( standardFonts );
    m_fontName->setCurrentText( KGlobalSettings::generalFont().family());
    QWhatsThis::add( m_fontName, i18n("<p>Select here the font name used for the pages.") );

    m_labelFontName = new QLabel( i18n("Fon&t name:"), page_setupLook );
    m_labelFontName->setBuddy( m_fontName );
    hlay11->addWidget( m_labelFontName );
    hlay11->addStretch( 1 );
    hlay11->addWidget( m_fontName );

    //---------------------------------------------

    QHBoxLayout *hlay12  = new QHBoxLayout( );
    vlay->addLayout( hlay12 );

    m_fontSize = new QSpinBox( 6, 15, 1, page_setupLook );
    m_fontSize->setValue( 14 );
    QWhatsThis::add( m_fontSize, i18n("<p>Select here the font size used for the pages.") );

    m_labelFontSize = new QLabel( i18n("Font si&ze:"), page_setupLook );
    m_labelFontSize->setBuddy( m_fontSize );
    hlay12->addWidget( m_labelFontSize );
    hlay12->addStretch( 1 );
    hlay12->addWidget( m_fontSize );

    //---------------------------------------------

    QHBoxLayout *hlay1  = new QHBoxLayout( spacingHint() );
    vlay->addLayout( hlay1 );

    m_foregroundColor = new KColorButton(page_setupLook);
    m_foregroundColor->setColor(QColor("#d0ffd0"));
    QWhatsThis::add( m_foregroundColor, i18n("<p>Select here the foreground color used for the pages.") );

    m_labelForegroundColor = new QLabel( i18n("&Foreground color:"), page_setupLook);
    m_labelForegroundColor->setBuddy( m_foregroundColor );
    hlay1->addWidget( m_labelForegroundColor );
    hlay1->addStretch( 1 );
    hlay1->addWidget(m_foregroundColor);

    //---------------------------------------------

    QHBoxLayout *hlay2 = new QHBoxLayout( spacingHint() );
    vlay->addLayout( hlay2 );

    m_backgroundColor = new KColorButton(page_setupLook);
    m_backgroundColor->setColor(QColor("#333333"));
    QWhatsThis::add( m_backgroundColor, i18n("<p>Select here the background color used for the pages.") );

    m_labelBackgroundColor = new QLabel( i18n("&Background color:"), page_setupLook);
    hlay2->addWidget( m_labelBackgroundColor );
    m_labelBackgroundColor->setBuddy( m_backgroundColor );
    hlay2->addStretch( 1 );
    hlay2->addWidget(m_backgroundColor);
   //---------------------------------------------

    QHBoxLayout *hlay13  = new QHBoxLayout( );
    vlay->addLayout( hlay13 );

    m_bordersImagesSize = new QSpinBox( 1, 20, 1, page_setupLook );
    m_bordersImagesSize->setValue( 1 );
    QWhatsThis::add( m_bordersImagesSize, i18n("<p>Select here the images borders size in pixels.") );

    QLabel *label = new QLabel( i18n("Images borders s&ize:"), page_setupLook );
    label->setBuddy( m_bordersImagesSize );
    hlay13->addWidget( label );
    hlay13->addStretch( 1 );
    hlay13->addWidget( m_bordersImagesSize );

    //---------------------------------------------

    QHBoxLayout *hlay4 = new QHBoxLayout( spacingHint() );
    vlay->addLayout( hlay4 );

    m_bordersImagesColor = new KColorButton(page_setupLook);
    m_bordersImagesColor->setColor(QColor("#d0ffd0"));
    QWhatsThis::add( m_bordersImagesColor, i18n("<p>Select here the color used "
                                                "for the images borders.") );

    label = new QLabel( i18n("Images bo&rders color:"), page_setupLook);
    hlay4->addWidget( label );
    label->setBuddy( m_bordersImagesColor );
    hlay4->addStretch( 1 );
    hlay4->addWidget(m_bordersImagesColor);

    //---------------------------------------------

    vlay->addStretch(1);

    connect(m_useHTMLInterface, SIGNAL(toggled(bool)),
            m_useAutoRunWin32, SLOT(setEnabled(bool)));

    connect(m_useHTMLInterface, SIGNAL(toggled(bool)),
            m_labelTitle, SLOT(setEnabled(bool)));

    connect(m_useHTMLInterface, SIGNAL(toggled(bool)),
            m_labelThumbsFileFormat, SLOT(setEnabled(bool)));

    connect(m_useHTMLInterface, SIGNAL(toggled(bool)),
            m_labelFontName, SLOT(setEnabled(bool)));

    connect(m_useHTMLInterface, SIGNAL(toggled(bool)),
            m_labelFontSize, SLOT(setEnabled(bool)));

    connect(m_useHTMLInterface, SIGNAL(toggled(bool)),
            m_labelForegroundColor, SLOT(setEnabled(bool)));

    connect(m_useHTMLInterface, SIGNAL(toggled(bool)),
            m_labelBackgroundColor, SLOT(setEnabled(bool)));

    connect(m_useHTMLInterface, SIGNAL(toggled(bool)),
            m_title, SLOT(setEnabled(bool)));

    connect(m_useHTMLInterface, SIGNAL(toggled(bool)),
            m_imagesPerRow, SLOT(setEnabled(bool)));

    connect(m_useHTMLInterface, SIGNAL(toggled(bool)),
            m_fontSize, SLOT(setEnabled(bool)));

    connect(m_useHTMLInterface, SIGNAL(toggled(bool)),
            m_fontName, SLOT(setEnabled(bool)));

    connect(m_useHTMLInterface, SIGNAL(toggled(bool)),
            m_imageFormat, SLOT(setEnabled(bool)));

    connect(m_useHTMLInterface, SIGNAL(toggled(bool)),
            m_thumbnailsSize, SLOT(setEnabled(bool)));

    connect(m_useHTMLInterface, SIGNAL(toggled(bool)),
            m_foregroundColor, SLOT(setEnabled(bool)));

    connect(m_useHTMLInterface, SIGNAL(toggled(bool)),
            m_backgroundColor, SLOT(setEnabled(bool)));
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::setupCDInfos(void)
{
    page_CDInfos = addPage( i18n("Volume descriptor"), i18n("Media Volume descriptor"),
                            BarIcon("cd", KIcon::SizeMedium ) );

    QVBoxLayout *vlay = new QVBoxLayout( page_CDInfos, 0, spacingHint() );

    QLabel *label;

    //---------------------------------------------

    label = new QLabel( i18n("Volume name:"), page_CDInfos);
    vlay->addWidget( label );

    m_volume_id = new QLineEdit(i18n("CD Albums"), page_CDInfos);
    vlay->addWidget( m_volume_id );
    m_labelTitle->setBuddy(m_volume_id);
    m_volume_id->setMaxLength(32);
    QWhatsThis::add( m_volume_id, i18n("<p>Enter here the media volume name (32 characters max.)") );

    //---------------------------------------------

    label = new QLabel( i18n("Volume set name:"), page_CDInfos);
    vlay->addWidget( label );

    m_volume_set_id = new QLineEdit(i18n("Albums CD archiving"), page_CDInfos);
    vlay->addWidget( m_volume_set_id );
    m_labelTitle->setBuddy(m_volume_set_id);
    m_volume_set_id->setMaxLength(128);
    QWhatsThis::add( m_volume_set_id,
                     i18n("<p>Enter here the media volume global name (128 characters max.)") );

    //---------------------------------------------

    label = new QLabel( i18n("System:"), page_CDInfos);
    vlay->addWidget( label );

    m_system_id = new QLineEdit(i18n("LINUX"), page_CDInfos);
    vlay->addWidget( m_system_id );
    m_labelTitle->setBuddy(m_system_id);
    m_system_id->setMaxLength(32);
    QWhatsThis::add( m_system_id,
                     i18n("<p>Enter here the media burning system name (32 characters max.)") );

    //---------------------------------------------

    label = new QLabel( i18n("Application:"), page_CDInfos);
    vlay->addWidget( label );

    m_application_id = new QLineEdit(i18n("K3b CD-DVD Burning application"), page_CDInfos);
    vlay->addWidget( m_application_id );
    m_labelTitle->setBuddy(m_application_id);
    m_application_id->setMaxLength(128);
    QWhatsThis::add( m_application_id,
                     i18n("<p>Enter here the media burning application name (128 characters max.).") );

    //---------------------------------------------

    label = new QLabel( i18n("Publisher:"), page_CDInfos);
    vlay->addWidget( label );

    m_publisher = new QLineEdit(i18n("KIPI [KDE Images Program Interface]"), page_CDInfos);
    vlay->addWidget( m_publisher );
    m_labelTitle->setBuddy(m_publisher);
    m_publisher->setMaxLength(128);
    QWhatsThis::add( m_publisher,
                     i18n("<p>Enter here the media publisher name (128 characters max.).") );

    //---------------------------------------------

    label = new QLabel( i18n("Preparer:"), page_CDInfos);
    vlay->addWidget( label );

    m_preparer = new QLineEdit(i18n("KIPI CD Archiving plugin"), page_CDInfos);
    vlay->addWidget( m_preparer );
    m_labelTitle->setBuddy(m_preparer);
    m_preparer->setMaxLength(128);
    QWhatsThis::add( m_preparer, i18n("<p>Enter here the media preparer name (128 characters max.).") );

    vlay->addStretch(1);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::setupBurning(void)
{
    page_burning = addPage( i18n("Media Burning"),
                            i18n("CD/DVD burning Setup"),
                            BarIcon("cdwriter_unmount", KIcon::SizeMedium ) );

    QVBoxLayout *vlay = new QVBoxLayout( page_burning, 0, spacingHint() );

    QLabel *label;

    //---------------------------------------------

    label = new QLabel(i18n("&K3b Binary path:"), page_burning);
    vlay->addWidget( label );

    m_K3bBinPath = new KURLRequester( "/usr/bin/k3b", page_burning);
    label->setBuddy( m_K3bBinPath );
    vlay->addWidget(m_K3bBinPath);

    connect( m_K3bBinPath, SIGNAL(textChanged(const QString&)),
             this, SLOT(UrlChanged(const QString&)));

    QWhatsThis::add( m_K3bBinPath, i18n("<p>The path name to the K3b binary program.") );

    //---------------------------------------------

    QGroupBox * groupBoxAdvancedOptions = new QGroupBox( i18n("Advanced burning options"), page_burning );
    groupBoxAdvancedOptions->setColumnLayout(0, Qt::Vertical );
    groupBoxAdvancedOptions->layout()->setSpacing( 6 );
    groupBoxAdvancedOptions->layout()->setMargin( 11 );

    QVBoxLayout * groupBoxAOLayout = new QVBoxLayout( groupBoxAdvancedOptions->layout() );
    groupBoxAOLayout->setAlignment( Qt::AlignTop );

    m_burnOnTheFly = new QCheckBox( i18n("Media burning On The Fly"), groupBoxAdvancedOptions);
    m_burnOnTheFly->setChecked( false );
    QWhatsThis::add( m_burnOnTheFly, i18n("<p>This option use the \"On The Fly\" "
                     "media burning capability. This burning mode do not use a media image.") );
    groupBoxAOLayout->addWidget( m_burnOnTheFly );

    m_checkCDBurn = new QCheckBox( i18n("Check media"), groupBoxAdvancedOptions);
    m_checkCDBurn->setChecked( false );
    QWhatsThis::add( m_checkCDBurn, i18n("<p>This option check the media after the burning process. "
                                         "You must use K3b release >= 0.10.0") );
    groupBoxAOLayout->addWidget( m_checkCDBurn );

    m_startBurningProcess = new QCheckBox( i18n("Start burning process automaticly"), groupBoxAdvancedOptions);
    m_startBurningProcess->setChecked( false );
    m_startBurningProcess->hide();
    QWhatsThis::add( m_startBurningProcess, i18n("<p>This option start automaticly the burning process "
                                         "when K3b is loaded.") );
    groupBoxAOLayout->addWidget( m_startBurningProcess );

    vlay->addWidget( groupBoxAdvancedOptions );

    vlay->addStretch(1);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::aboutPage(void)
{
    page_about = addPage( i18n("About"), i18n("About KIPI Albums CD Archiving"),
                          BarIcon("kipi", KIcon::SizeMedium ) );

    QVBoxLayout *vlay = new QVBoxLayout( page_about, 0, spacingHint() );

    QLabel *label = new QLabel( i18n("A KIPI plugin for Albums CD archiving\n\n"
                             "Author: Gilles Caulier\n\n"
                             "Email: caulier dot gilles at free.fr\n\n"
                             "Thanks to Gregory Kokanosky <gregory dot kokanosky at free.fr> "
                             "for images navigation mode patchs\n\n"
                             "This plugin use K3b CD/DVD burning software avialable at this URL:\n\n"
                             "http://www.k3b.org."), page_about);

    vlay->addWidget(label);
    vlay->addStretch(1);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::albumSelected( QListViewItem * item )
{
    if ( !item ) return;

    AlbumItem *pitem = static_cast<AlbumItem*>( item );
    if ( pitem == NULL ) return;
    m_AlbumComments->setText( i18n("Comment: %1").arg(pitem->comments()) );
    m_AlbumCollection->setText( i18n("Collection: %1").arg(pitem->collection()) );
    m_AlbumDate->setText( i18n("Date: %1").arg( pitem->date().toString(( Qt::LocalDate ) ) ) );
    m_AlbumItems->setText( i18n("Items: %1").arg( pitem->items() ) );
    m_AlbumSize->setText( i18n("Size: %1").arg( KIO::convertSizeFromKB (pitem->size()) ) );

    ShowMediaCapacity();

    m_albumPreview->clear();

    KIO::PreviewJob* thumbJob = KIO::filePreview( pitem->firstImage(), m_albumPreview->height() );
    
    connect(thumbJob, SIGNAL(gotPreview(const KFileItem*, const QPixmap&)),
            this, SLOT(slotGotPreview(const KFileItem*, const QPixmap&)));
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::slotGotPreview(const KFileItem*, const QPixmap &pixmap)
{
    m_albumPreview->setPixmap(pixmap);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::mediaFormatActived (const QString & item )
{
    QString Color;

    if (item == i18n("CD (650Mb)"))
       MaxMediaSize = 665600;

    if (item == i18n("CD (700Mb)"))
       MaxMediaSize = 716800;

    if (item == i18n("CD (880Mb)"))
       MaxMediaSize = 901120;

    if (item == i18n("DVD (4,7Gb)"))
       MaxMediaSize = 4928307;

    ShowMediaCapacity();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::ShowMediaCapacity(void)
{
   QString Color = "<font color=\"blue\">";;

   if (TargetMediaSize >= MaxMediaSize - (MaxMediaSize*0.1))
       Color = "<font color=\"orange\">";

   if (TargetMediaSize >= MaxMediaSize)
       Color = "<font color=\"red\">";

   m_mediaSize->setText( i18n("Total size: ") + Color +
                         i18n("<b>%1</b></font> / <b>%2</b>").arg(KIO::convertSizeFromKB(TargetMediaSize))
                              .arg(KIO::convertSizeFromKB (MaxMediaSize)) );
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::slotOk()
{
    if (getAlbumsSelection().isEmpty() == true)
       {
       KMessageBox::sorry(this, i18n("You must selected at least an Album to archive!"));
       return;
       }

    QFile fileK3b(getK3bBinPathName());

    if (fileK3b.exists() == false)
       {
       KMessageBox::sorry(this, i18n("K3b binary path is not valid ! Please check it..."));
       return;
       }

    if (TargetMediaSize >= MaxMediaSize)
       {
       KMessageBox::sorry(this, i18n("Target media size is too big. Please, change your Albums selection!"));
       return;
       }

    accept();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::slotbuttonSelectAll(void)
{
    QListViewItemIterator it( m_AlbumsList );

    while ( it.current() )
        {
        AlbumItem *item = static_cast<AlbumItem*>( it.current() );

        if (!item->isOn())
            item->setOn(true);

        ++it;
        }

    albumSelected( m_AlbumsList->currentItem() );
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::slotbuttonInvertSelection(void)
{
    QListViewItemIterator it( m_AlbumsList );

    while ( it.current() )
        {
        AlbumItem *item = static_cast<AlbumItem*>( it.current() );

        if (!item->isOn())
            item->setOn(true);
        else
            item->setOn(false);

        ++it;
        }

    albumSelected( m_AlbumsList->currentItem() );
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::slotbuttonSelectNone(void)
{
    QListViewItemIterator it( m_AlbumsList );

    while ( it.current() )
        {
        AlbumItem *item = static_cast<AlbumItem*>( it.current() );

        if (item->isOn())
            item->setOn(false);

        ++it;
        }

    albumSelected( m_AlbumsList->currentItem() );
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::UrlChanged(const QString &url )
{
    enableButtonOK( !url.isEmpty());
}


/////////////////////////////////////////////////////////////////////////////////////////////

const QString CDArchivingDialog::getK3bBinPathName() const
{
    return m_K3bBinPath->url();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::setK3bBinPathName(QString Value)
{
    m_K3bBinPath->setURL( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

int CDArchivingDialog::getImagesPerRow() const
{
    return m_imagesPerRow->value();
}

/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::setImagesPerRow(int Value)
{
    m_imagesPerRow->setValue(Value);
}

/////////////////////////////////////////////////////////////////////////////////////////////

int CDArchivingDialog::getThumbnailsSize() const
{
    return m_thumbnailsSize->value();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::setThumbnailsSize(int Value)
{
    m_thumbnailsSize->setValue( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

const QString CDArchivingDialog::getFontName() const
{
    return m_fontName->currentText();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::setFontName(QString Value)
{
    m_fontName->setCurrentText (Value);
}


/////////////////////////////////////////////////////////////////////////////////////////////

const QString CDArchivingDialog::getFontSize() const
{
    return m_fontSize->text();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::setFontSize(int Value)
{
    m_fontSize->setValue( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

const QColor CDArchivingDialog::getBackgroundColor() const
{
    return m_backgroundColor->color();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::setBackgroundColor(QColor Value)
{
    m_backgroundColor->setColor( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

const QColor CDArchivingDialog::getForegroundColor() const
{
    return m_foregroundColor->color();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::setForegroundColor(QColor Value)
{
    m_foregroundColor->setColor( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

const QString CDArchivingDialog::getImageFormat() const
{
    return m_imageFormat->currentText();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::setImageFormat(QString Value)
{
    return m_imageFormat->setCurrentText( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

const QString CDArchivingDialog::getMainTitle() const
{
    return m_title->text();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::setMainTitle(QString Value)
{
    return m_title->setText( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

bool CDArchivingDialog::getUseHTMLInterface() const
{
    return m_useHTMLInterface->isChecked();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::setUseHTMLInterface(bool Value)
{
    m_useHTMLInterface->setChecked(Value);
}


/////////////////////////////////////////////////////////////////////////////////////////////

bool CDArchivingDialog::getUseAutoRunWin32() const
{
    return m_useAutoRunWin32->isChecked();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::setUseAutoRunWin32(bool Value)
{
    m_useAutoRunWin32->setChecked(Value);
}


/////////////////////////////////////////////////////////////////////////////////////////////

QValueList<KIPI::ImageCollection> CDArchivingDialog::getAlbumsSelection(void)
{
    QValueList<KIPI::ImageCollection> AlbumsListSelected;
    QListViewItemIterator it( m_AlbumsList );

    while ( it.current() )
        {
        AlbumItem *item = static_cast<AlbumItem*>( it.current() );

        if (item->isOn())
            AlbumsListSelected.append( item->album() );

        ++it;
        }

    return (AlbumsListSelected);
}


/////////////////////////////////////////////////////////////////////////////////////////////

const QString CDArchivingDialog::getVolumeID() const
{
    return m_volume_id->text();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::setVolumeID(QString Value)
{
    return m_volume_id->setText( Value );
}

/////////////////////////////////////////////////////////////////////////////////////////////

const QString CDArchivingDialog::getVolumeSetID() const
{
    return m_volume_set_id->text();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::setVolumeSetID(QString Value)
{
    return m_volume_set_id->setText( Value );
}

/////////////////////////////////////////////////////////////////////////////////////////////

const QString CDArchivingDialog::getSystemID() const
{
    return m_system_id->text();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::setSystemID(QString Value)
{
    return m_system_id->setText( Value );
}

/////////////////////////////////////////////////////////////////////////////////////////////

const QString CDArchivingDialog::getApplicationID() const
{
    return m_application_id->text();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::setApplicationID(QString Value)
{
    return m_application_id->setText( Value );
}

/////////////////////////////////////////////////////////////////////////////////////////////

const QString CDArchivingDialog::getPublisher() const
{
    return m_publisher->text();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::setPublisher(QString Value)
{
    return m_publisher->setText( Value );
}

/////////////////////////////////////////////////////////////////////////////////////////////

const QString CDArchivingDialog::getPreparer() const
{
    return m_preparer->text();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::setPreparer(QString Value)
{
    return m_preparer->setText( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

const QString CDArchivingDialog::getMediaFormat() const
{
    return m_mediaFormat->currentText();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::setMediaFormat(QString Value)
{
    return m_mediaFormat->setCurrentText( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

bool CDArchivingDialog::getUseOnTheFly() const
{
    return m_burnOnTheFly->isChecked();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::setUseUseOnTheFly(bool Value)
{
    m_burnOnTheFly->setChecked(Value);
}


/////////////////////////////////////////////////////////////////////////////////////////////

bool CDArchivingDialog::getUseCheckCD() const
{
    return m_checkCDBurn->isChecked();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::setUseCheckCD(bool Value)
{
    m_checkCDBurn->setChecked(Value);
}


/////////////////////////////////////////////////////////////////////////////////////////////

bool CDArchivingDialog::getUseStartBurningProcess() const
{
    return m_startBurningProcess->isChecked();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::setUseStartBurningProcess(bool Value)
{
    m_startBurningProcess->setChecked(Value);
}


/////////////////////////////////////////////////////////////////////////////////////////////

const QColor CDArchivingDialog::getBordersImagesColor() const
{
    return m_bordersImagesColor->color();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::setBordersImagesColor(QColor Value)
{
    m_bordersImagesColor->setColor( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

const QString CDArchivingDialog::getBordersImagesSize() const
{
    return m_bordersImagesSize->text();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void CDArchivingDialog::setBordersImagesSize(int Value)
{
    m_bordersImagesSize->setValue( Value );
}

}  // NameSpace KIPICDArchivingPlugin

#include "cdarchivingdialog.moc"
