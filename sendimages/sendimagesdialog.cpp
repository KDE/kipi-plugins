//////////////////////////////////////////////////////////////////////////////
//
//    SENDIMAGESDIALOG.CPP
//
//    Copyright (C) 2003 Gilles Caulier <caulier dot gilles at free.fr>
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

#include <qvbox.h>
#include <qlayout.h>
#include <qdir.h>
#include <qwidget.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qwhatsthis.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qprogressdialog.h>
#include <qimage.h>
#include <qevent.h>
#include <qdragobject.h>
#include <qstrlist.h>
#include <qfileinfo.h>

// Include files for KDE

#include <klocale.h>
#include <klineedit.h>
#include <kiconloader.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <kinstance.h>
#include <kconfig.h>
#include <kapplication.h>
#include <knuminput.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kdialogbase.h>
#include <kbuttonbox.h>
#include <ksqueezedtextlabel.h>

// libKipi includes.

#include <libkipi/imagecollectiondialog.h>

// Local include files

#include "sendimagesdialog.h"
#include "listimageserrordialog.h"

namespace KIPISendimagesPlugin
{

/////////////////////////////////////////////////////////////////////////////////////////////

class ImageItem : public QListBoxText
{
public:
    ImageItem(QListBox * parent, QString const & comments, KURL const & url)
            : QListBoxText(parent), _comments(comments), _url(url)
    {}

    QString comments()                   { return _comments;                         }
    QString name()                       { return _url.fileName();                   }
    KURL    url()                        { return _url;                              }
    QString album()                      { return _url.directory().section('/', -1); }
    void setName(const QString &newName) { setText(newName);                         }

private:
    QString _comments;
    KURL    _url;    
};


/////////////////////////////////////////////////////////////////////////////////////////////

ListImageItems::ListImageItems(QWidget *parent, const char *name)
              : KListBox(parent, name)
{
    setSelectionMode (QListBox::Extended);
    setAcceptDrops(true);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void ListImageItems::dragEnterEvent(QDragEnterEvent *e)
{
    e->accept(QUriDrag::canDecode(e));
}


/////////////////////////////////////////////////////////////////////////////////////////////

void ListImageItems::dropEvent(QDropEvent *e)
{
    QStrList strList;
    QStringList FilesPath;

    if ( !QUriDrag::decode(e, strList) ) return;

    QStrList stringList;
    QStrListIterator it(strList);
    char *str;

    while ( (str = it.current()) != 0 )
       {
       QString filePath = QUriDrag::uriToLocalFile(str);
       QFileInfo fileInfo(filePath);

       if (fileInfo.isFile() && fileInfo.exists())
          FilesPath.append(fileInfo.filePath());

       ++it;
       }

    if (FilesPath.isEmpty() == false)
       emit addedDropItems(FilesPath);
}

//////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////////

SendImagesDialog::SendImagesDialog(QWidget *parent, KIPI::Interface* interface, 
                                   const KIPI::ImageCollection& images )
                : KDialogBase( IconList, i18n("E-mail Images Options"), Help|Ok|Cancel,
                  Ok, parent, "SendImagesDialog", false, true )
{
    m_interface = interface;
    m_thumbJob = 0L;

    setupImagesList();
    setupEmailOptions();
    aboutPage();
    readSettings();
    setHelp("sendimages", "kipi-plugins");
    setImagesList( images.images() );
    page_setupImagesList->setFocus();
    m_ImagesFilesListBox->setSelected(0, true);
    slotImageSelected(m_ImagesFilesListBox->item(0));
    setNbItems();
    resize( 600, 400 );
}


//////////////////////////////////// DESTRUCTOR /////////////////////////////////////////////

SendImagesDialog::~SendImagesDialog()
{
    if ( m_thumbJob ) delete m_thumbJob; 
}


/////////////////////////////////////////////////////////////////////////////////////////////

void SendImagesDialog::readSettings(void)
{
    // Read all settings from configuration file.

    m_config = new KConfig("kipirc");
    m_config->setGroup("SendImages Settings");

    m_mailAgentName->setCurrentText(m_config->readEntry("MailAgentName", "Kmail"));

    if (m_config->readEntry("ImagesChangeProp", "true") == "true")
        m_changeImagesProp->setChecked( true );
    else
        m_changeImagesProp->setChecked( false );

    m_imagesResize->setCurrentItem(m_config->readNumEntry("ImageResize", 2));    // Medium size used by default...
    m_imageCompression->setValue(m_config->readNumEntry("ImageCompression", 75));
    m_imagesFormat->setCurrentText(m_config->readEntry("ImageFormat", "JPEG"));

    if (m_config->readEntry("AddComments", "true") == "true")
        m_addComments->setChecked( true );
    else
        m_addComments->setChecked( false );

    delete m_config;
}


/////////////////////////////////////////////////////////////////////////////////////////////

void SendImagesDialog::writeSettings(void)
{
    // Write all settings in configuration file.

    m_config = new KConfig("kipirc");
    m_config->setGroup("SendImages Settings");
    m_config->writeEntry("MailAgentName", m_mailAgentName->currentText());
    m_config->writeEntry("AddComments", m_addComments->isChecked());
    m_config->writeEntry("ImagesChangeProp", m_changeImagesProp->isChecked());
    m_config->writeEntry("ImageResize", m_imagesResize->currentItem());
    m_config->writeEntry("ImageCompression", m_imageCompression->value());
    m_config->writeEntry("ImageFormat", m_imagesFormat->currentText());
    m_config->sync();
    delete m_config;
}


/////////////////////////////////////////////////////////////////////////////////////////////

void SendImagesDialog::setupImagesList(void)
{
    QString whatsThis;

    page_setupImagesList = addPage( i18n("Images"),
                                    i18n("Image list to e-mail"),
                                    BarIcon("image", KIcon::SizeMedium ) );

    QVBoxLayout *vlay = new QVBoxLayout( page_setupImagesList, 0, spacingHint() );

    //---------------------------------------------

    m_groupBoxImageList = new QGroupBox( page_setupImagesList );
    m_groupBoxImageList->setFlat(false);
    QGridLayout* grid = new QGridLayout( m_groupBoxImageList, 2, 2 , 20, 20);

    m_ImagesFilesListBox = new ListImageItems( m_groupBoxImageList, "ListImageItems" );
    QWhatsThis::add( m_ImagesFilesListBox, i18n( "<p>This is the list of images  to e-mail. "
                                                 "If you want to add some images click on the 'Add images' "
                                                 "button or use the drag-and-drop."));
    grid->addMultiCellWidget(m_ImagesFilesListBox, 0, 2, 0, 1);

    KButtonBox* imagesListButtonBox = new KButtonBox( m_groupBoxImageList, Vertical );
    QPushButton* m_addImagesButton = imagesListButtonBox->addButton ( i18n( "&Add images" ) );
    QWhatsThis::add( m_addImagesButton, i18n("<p>Add images to the list.") );
    QPushButton* m_remImagesButton = imagesListButtonBox->addButton ( i18n( "&Remove images" ));
    QWhatsThis::add( m_remImagesButton, i18n("<p>Remove selected images from the list.") );
    imagesListButtonBox->layout();
    grid->addMultiCellWidget(imagesListButtonBox, 0, 1, 2, 2);

    m_imageLabel = new QLabel( m_groupBoxImageList );
    m_imageLabel->setFixedHeight( 120 );
    m_imageLabel->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    m_imageLabel->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
    QWhatsThis::add( m_imageLabel, i18n( "<p>Preview of the currently selected image on the list." ) );
    grid->addMultiCellWidget(m_imageLabel, 2, 2, 2, 2);

    vlay->addWidget( m_groupBoxImageList );

    //---------------------------------------------

    QGroupBox * groupBox2 = new QGroupBox( i18n("Image description"), page_setupImagesList );
    groupBox2->setColumnLayout(0, Qt::Vertical );
    groupBox2->layout()->setSpacing( 6 );
    groupBox2->layout()->setMargin( 11 );
    QWhatsThis::add( groupBox2, i18n("<p>The description of the currently selected image on the list.") );

    QVBoxLayout * groupBox2Layout = new QVBoxLayout( groupBox2->layout() );
    groupBox2Layout->setAlignment( Qt::AlignTop );

    m_ImageComments = new KSqueezedTextLabel( groupBox2 );
    m_ImageComments->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );
    groupBox2Layout->addWidget( m_ImageComments );

    m_ImageAlbum = new KSqueezedTextLabel( groupBox2 );
    m_ImageAlbum->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter ) );
    groupBox2Layout->addWidget( m_ImageAlbum );

    vlay->addWidget( groupBox2 );
    vlay->addStretch(1);

    //---------------------------------------------

    connect(m_addImagesButton, SIGNAL(clicked()),
            this, SLOT(slotImagesFilesButtonAdd()));

    connect(m_remImagesButton, SIGNAL(clicked()),
            this, SLOT(slotImagesFilesButtonRem()));

    connect(m_ImagesFilesListBox, SIGNAL( currentChanged( QListBoxItem * ) ),
            this, SLOT( slotImageSelected( QListBoxItem * )));

    connect(m_ImagesFilesListBox, SIGNAL( addedDropItems(QStringList) ),
            this, SLOT( slotAddDropItems(QStringList)));
}


/////////////////////////////////////////////////////////////////////////////////////////////

void SendImagesDialog::setImagesList( const KURL::List& Files )
{
    if ( Files.count() == 0 ) return;

    for( KURL::List::ConstIterator it = Files.begin(); it != Files.end(); ++it ) 
      {
      KIPI::ImageInfo imageInfo = m_interface->info( *it );
      QString comments = imageInfo.description();

      // Check if the new item already exist in the list.

      bool findItem = false;

      for (uint i = 0 ; i < m_ImagesFilesListBox->count() ; ++i)
          {
          ImageItem *pitem = static_cast<ImageItem*>( m_ImagesFilesListBox->item(i) );

          if (pitem->url() == (*it))
             findItem = true;
          }

      if (findItem == false)
         {
         ImageItem *item = new ImageItem( m_ImagesFilesListBox,
                                          comments,                   // Image comments.
                                          *it                         // Complete url (path & file name).
                                        );

         item->setName( (*it).fileName() );
         }
      }

    m_ImagesFilesListBox->setCurrentItem( m_ImagesFilesListBox->count()-1) ;
    slotImageSelected(m_ImagesFilesListBox->item(m_ImagesFilesListBox->currentItem()));
    m_ImagesFilesListBox->centerCurrentItem();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void SendImagesDialog::setupEmailOptions(void)
{
    QString whatsThis;

    page_setupEmailOptions = addPage( i18n("E-mail"),
                                      i18n("E-mail options"),
                                      BarIcon("mail_generic", KIcon::SizeMedium ) );

    QVBoxLayout *vlay = new QVBoxLayout( page_setupEmailOptions, 0, spacingHint() );

    //---------------------------------------------

    QHBoxLayout *hlay10  = new QHBoxLayout( );
    vlay->addLayout( hlay10 );

    QLabel *m_mailAgentLabel = new QLabel( i18n("Mail agent:"), page_setupEmailOptions);

    m_mailAgentName = new QComboBox( false, page_setupEmailOptions );
    m_mailAgentName->insertItem( "Balsa" );
    m_mailAgentName->insertItem( "Evolution" );
    m_mailAgentName->insertItem( "Kmail" );
    m_mailAgentName->insertItem( "Mozilla" );
    m_mailAgentName->insertItem( "Netscape" );
    m_mailAgentName->insertItem( "Sylpheed" );
    m_mailAgentName->insertItem( "Thunderbird" );
    m_mailAgentName->setCurrentText( "Kmail" );
    QWhatsThis::add( m_mailAgentName, i18n("<p>Select here your preferred external mail agent program."
                                           "These mail agents version are supported:<p>"
                                           "<b>Balsa</b>: >= 2.x<p>"
                                           "<b>Evolution</b>: >= 1.4<p>"
                                           "<b>Kmail</b>: >= 1.3<p>"
                                           "<b>Mozilla</b>: >= 1.4<p>"
                                           "<b>Netscape</b>: >= 7.x<p>"
                                           "<b>Sylpheed</b>: >= 0.9<p>"
                                           "<b>Thunderbird</b>: >= 0.4<p>") );

    hlay10->addWidget( m_mailAgentLabel );
    hlay10->addStretch( 1 );
    hlay10->addWidget( m_mailAgentName );

    //---------------------------------------------

    m_addComments = new QCheckBox( i18n("Add comments in attached file"), page_setupEmailOptions);
    QWhatsThis::add( m_addComments, i18n("<p>If you enable this option, all images comments "
                                         "will be added as an attached file.") );
    vlay->addWidget( m_addComments );

    //---------------------------------------------

    QGroupBox * groupBox2 = new QGroupBox( i18n("Images properties"), page_setupEmailOptions );
    groupBox2->setColumnLayout(0, Qt::Vertical );
    groupBox2->layout()->setSpacing( 6 );
    groupBox2->layout()->setMargin( 11 );
    QWhatsThis::add( groupBox2, i18n("<p>The properties of images to send.") );

    QVBoxLayout * groupBox2Layout = new QVBoxLayout( groupBox2->layout() );
    groupBox2Layout->setAlignment( Qt::AlignTop );

    m_changeImagesProp = new QCheckBox(i18n("Change properties of images to send"), groupBox2);
    QWhatsThis::add( m_changeImagesProp, i18n("<p>If you enable this option, "
                     "all images to send can be resized and recompressed.") );
    m_changeImagesProp->setChecked( true );
    groupBox2Layout->addWidget( m_changeImagesProp );

    QHBoxLayout *hlay12  = new QHBoxLayout();
    groupBox2Layout->addLayout( hlay12 );

    m_imagesResize = new QComboBox(false, groupBox2);
    m_imagesResize->insertItem(i18n("very small (320 pixels)"));
    m_imagesResize->insertItem(i18n("small (640 pixels)"));
    m_imagesResize->insertItem(i18n("medium (800 pixels)"));
    m_imagesResize->insertItem(i18n("big (1024 pixels)"));
    m_imagesResize->insertItem(i18n("very big (1280 pixels)"));
    m_imagesResize->setCurrentText (i18n("medium (800 pixels)"));
    whatsThis = i18n("<p>Select here the images size to send:<p>"
                     "<b>%1</b>: use this if you have a very slow internet "
                     "connection or if the target mailbox size is very limited.<p>"
                     "<b>%2</b>: use this if you have a slow internet connection "
                     "and if the target mailbox size is limited.<p>"
                     "<b>%3</b>: this is the default value for a medium internet connection "
                     "and a target mailbox size.<p>"
                     "<b>%4</b>: use this if you have a high-speed internet connection "
                     "and if the target mailbox size is not limited.<p>"
                     "<b>%5</b>: use this if you have no size or speed restrictions.<p>")
                     .arg(i18n("very small (320 pixels)"))
                     .arg(i18n("small (640 pixels)"))
                     .arg(i18n("medium (800 pixels)"))
                     .arg(i18n("big (1024 pixels)"))
                     .arg(i18n("very big (1280 pixels)"));
    QWhatsThis::add( m_imagesResize, whatsThis );

    m_labelImageSize = new QLabel( i18n("New images' size:"), groupBox2);
    hlay12->addWidget( m_labelImageSize );
    m_labelImageSize->setBuddy( m_imagesResize );
    hlay12->addStretch( 1 );
    hlay12->addWidget(m_imagesResize);

    //---------------------------------------------

    m_imageCompression = new KIntNumInput(75, groupBox2);
    m_imageCompression->setRange(1, 100, 1, true );
    m_imageCompression->setLabel( i18n("New images' compression:") );
    groupBox2Layout->addWidget( m_imageCompression );
    whatsThis = i18n("<p>The new compression value of images to send:<p>");
    whatsThis = whatsThis + i18n("<b>1</b>: very high compression<p>"
                                 "<b>25</b>: high compression<p>"
                                 "<b>50</b>: medium compression<p>"
                                 "<b>75</b>: low compression (default value)<p>"
                                 "<b>100</b>: no compression");

    QWhatsThis::add( m_imageCompression, whatsThis);

    //---------------------------------------------

    QHBoxLayout *hlay13  = new QHBoxLayout( );
    groupBox2Layout->addLayout( hlay13 );

    m_imagesFormat = new QComboBox(false, groupBox2);
    m_imagesFormat->insertItem("JPEG");
    m_imagesFormat->insertItem("PNG");
    m_imagesFormat->setCurrentText ("JPEG");
    whatsThis = i18n("<p>Select here the images files format to send.<p>");
    whatsThis = whatsThis + i18n("<b>JPEG</b>: The Joint Photographic Experts Group's file format "
                "is a good Web file format but it uses lossy compression.<p>"
                "<b>PNG</b>: the Portable Network Graphics format is an extensible file format for "
                "the lossless, portable, well-compressed storage of raster images. PNG provides a "
                "patent-free replacement for GIF and can also replace many common uses of TIFF. "
                "PNG is designed to work well in online viewing applications, such as the World Wide Web, "
                "so it is fully streamable with a progressive display option. Also, PNG can store gamma "
                "and chromaticity data for improved color matching on heterogeneous platforms.");
    QWhatsThis::add( m_imagesFormat, whatsThis );

    m_labelImageFormat = new QLabel( i18n("Images' file format:"), groupBox2);
    hlay13->addWidget( m_labelImageFormat );
    m_labelImageFormat->setBuddy( m_imagesFormat );
    hlay13->addStretch( 1 );
    hlay13->addWidget(m_imagesFormat);

    vlay->addWidget( groupBox2 );
    vlay->addStretch(1);

    //---------------------------------------------

    connect(m_changeImagesProp, SIGNAL(toggled(bool)),
            m_labelImageSize, SLOT(setEnabled(bool)));

    connect(m_changeImagesProp, SIGNAL(toggled(bool)),
            m_imagesResize, SLOT(setEnabled(bool)));

    connect(m_changeImagesProp, SIGNAL(toggled(bool)),
            m_imageCompression, SLOT(setEnabled(bool)));

    connect(m_changeImagesProp, SIGNAL(toggled(bool)),
            m_labelImageFormat, SLOT(setEnabled(bool)));

    connect(m_changeImagesProp, SIGNAL(toggled(bool)),
            m_imagesFormat, SLOT(setEnabled(bool)));
}


/////////////////////////////////////////////////////////////////////////////////////////////

void SendImagesDialog::aboutPage(void)
{
    page_about = addPage( i18n("About"), i18n("About KIPI's 'E-mail Images' Plugin"),
                          BarIcon("kipi", KIcon::SizeMedium ) );

    QVBoxLayout *vlay = new QVBoxLayout( page_about, 0, spacingHint() );

    QLabel *label = new QLabel( i18n("A KIPI plugin for e-mailing images\n\n"
                                     "Author: Gilles Caulier\n\n"
                                     "Email: caulier dot gilles at free.fr\n\n"), page_about);

    vlay->addWidget(label);
    vlay->addStretch(1);
}


//////////////////////////////////////// SLOTS //////////////////////////////////////////////

void SendImagesDialog::slotAddDropItems(QStringList filesPath)
{
    setImagesList( KURL::List( filesPath) );
}


/////////////////////////////////////////////////////////////////////////////////////////////

void SendImagesDialog::slotImagesFilesButtonAdd( void )
{
    KURL::List ImageFilesList;
    KURL url = KIPI::ImageCollectionDialog::getImageURL( this, m_interface );

    if ( !url.isValid() ) return;
  
    ImageFilesList << url.path();
    setImagesList(ImageFilesList);
    setNbItems();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void SendImagesDialog::slotImagesFilesButtonRem( void )
{
    for (uint i = 0 ; i < m_ImagesFilesListBox->count() ; ++i)
       {
       if (m_ImagesFilesListBox->isSelected(i))
           {
           m_ImagesFilesListBox->removeItem(i);
           m_ImagesFilesListBox->setCurrentItem(i);
           --i;
           }
       }

    m_ImagesFilesListBox->setSelected(m_ImagesFilesListBox->item(m_ImagesFilesListBox->currentItem()), true);
    slotImageSelected(m_ImagesFilesListBox->item(m_ImagesFilesListBox->currentItem()));
    setNbItems();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void SendImagesDialog::slotImageSelected( QListBoxItem * item )
{
    if ( !item || m_ImagesFilesListBox->count() == 0 )
       {
       m_imageLabel->clear();
       return;
       }

    ImageItem *pitem = static_cast<ImageItem*>( item );
    
    if ( !pitem ) return;

    m_ImageComments->setText( i18n("Comments: %1").arg(pitem->comments()) );
    m_ImageAlbum->setText( i18n("Album: %1").arg(pitem->album()) );
    m_imageLabel->clear();
    
    if ( m_thumbJob ) delete m_thumbJob; 
  
    m_thumbJob = KIO::filePreview( pitem->url(), m_imageLabel->height() );

    connect(m_thumbJob, SIGNAL(gotPreview(const KFileItem*, const QPixmap&)),
            SLOT(slotGotPreview(const KFileItem*, const QPixmap&)));
            
    connect(m_thumbJob, SIGNAL(failed(const KFileItem*)),
            SLOT(slotFailedPreview(const KFileItem*)));            
}


/////////////////////////////////////////////////////////////////////////////////////////////

void SendImagesDialog::slotGotPreview(const KFileItem* /*url*/, const QPixmap &pixmap)
{
    m_imageLabel->setPixmap(pixmap);
    m_thumbJob = 0L;
}


/////////////////////////////////////////////////////////////////////////////////////////////

void SendImagesDialog::slotFailedPreview(const KFileItem*)
{
    m_thumbJob = 0L;
}


/////////////////////////////////////////////////////////////////////////////////////////////

void SendImagesDialog::slotOk()
{
    if ( m_ImagesFilesListBox->count() == 0 )
       {
       KMessageBox::error(0, i18n("You must add some images to send."));
       return;
       }

    writeSettings();
    
    for (uint i = 0 ; i < m_ImagesFilesListBox->count() ; ++i)
        {
        ImageItem *pitem = static_cast<ImageItem*>( m_ImagesFilesListBox->item(i) );
        m_images2send << pitem->url();
        }
    
    accept();
}


////////////////////////////////////////// FONCTIONS ////////////////////////////////////////////////

void SendImagesDialog::setNbItems(void)
{
    if ( m_ImagesFilesListBox->count() == 0 ) m_groupBoxImageList->setTitle(i18n("Image list"));
    else
       m_groupBoxImageList->setTitle(i18n("Image list (1 item)", "Image list (%n items)",
                                     m_ImagesFilesListBox->count() ));
}

}  // NameSpace KIPISendimagesPlugin

#include "sendimagesdialog.moc"
