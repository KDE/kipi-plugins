//////////////////////////////////////////////////////////////////////////////
//
//    SENDIMAGESDIALOG.CPP
//
//    Copyright (C) 2003 Gilles CAULIER <caulier dot gilles at free.fr>
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
#include <qtimer.h>
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
#include <kprocess.h>
#include <kimageio.h>
#include <kbuttonbox.h>
#include <ksqueezedtextlabel.h>

// Include files for Digikam

#include <digikam/albummanager.h>
#include <digikam/albuminfo.h>

// Local include files

#include "sendimagesdialog.h"
#include "listimageserrordialog.h"

/////////////////////////////////////////////////////////////////////////////////////////////

class ImageItem : public QListBoxText
{
public:
    ImageItem(QListBox * parent, QString const & name, QString const & comments, QString const & path,
              QString const & album)
            : QListBoxText(parent), _name(name), _comments(comments), _path(path), _album(album)
    {}

    QString comments()                   { return _comments; }
    QString name()                       { return _name;     }
    QString path()                       { return _path;     }
    QString album()                      { return _album;    }
    void setName(const QString &newName) { setText(newName); }

private:
    QString _name;
    QString _comments;
    QString _path;
    QString _album;
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

SendImagesDialog::SendImagesDialog(QWidget *parent, QString TmpPath, QStringList imagesfileList)
                : KDialogBase( IconList, i18n("E-mail Images Options"), Help|Ok|Cancel,
                  Ok, parent, "SendImagesDialog", false, true )
{
    KImageIO::registerFormats();
    m_mozillaTimer = new QTimer(this);
    m_tempPath = TmpPath;

    setupImagesList();
    setupEmailOptions();
    aboutPage();
    readSettings();
    setHelp("plugin-sendimages.anchor", "digikam");
    setImagesList(imagesfileList);
    page_setupImagesList->setFocus();
    m_ImagesFilesListBox->setSelected(0, true);
    slotImageSelected(m_ImagesFilesListBox->item(0));
    setNbItems();
    resize( 600, 400 );
}


//////////////////////////////////// DESTRUCTOR /////////////////////////////////////////////

SendImagesDialog::~SendImagesDialog()
{
    if (!m_thumbJob.isNull())
       delete m_thumbJob;
}


/////////////////////////////////////////////////////////////////////////////////////////////

void SendImagesDialog::readSettings(void)
{
    // Read all settings from configuration file.

    m_config = new KConfig("digikamrc");
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

    // Read File Filter settings in digikamrc file.

    m_config->setGroup("Album Settings");
    m_ImagesFilesSort = m_config->readEntry("File Filter", "*.jpg *.jpeg *.tif *.tiff *.gif *.png *.bmp");

    delete m_config;
}


/////////////////////////////////////////////////////////////////////////////////////////////

void SendImagesDialog::writeSettings(void)
{
    // Write all settings in configuration file.

    m_config = new KConfig("digikamrc");
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
                                    i18n("Images list to e-mail"),
                                    BarIcon("image", KIcon::SizeMedium ) );

    QVBoxLayout *vlay = new QVBoxLayout( page_setupImagesList, 0, spacingHint() );

    //---------------------------------------------

    m_groupBoxImageList = new QGroupBox( page_setupImagesList );
    m_groupBoxImageList->setFlat(false);
    QGridLayout* grid = new QGridLayout( m_groupBoxImageList, 2, 2 , 20, 20);

    m_ImagesFilesListBox = new ListImageItems( m_groupBoxImageList, "ListImageItems" );
    QWhatsThis::add( m_ImagesFilesListBox, i18n( "<p>This is the images list to e-mail. "
                                                 "If you want to add some images click on 'Add images' "
                                                 "button or use the drag and drop."));
    grid->addMultiCellWidget(m_ImagesFilesListBox, 0, 2, 0, 1);

    KButtonBox* imagesListButtonBox = new KButtonBox( m_groupBoxImageList, Vertical );
    QPushButton* m_addImagesButton = imagesListButtonBox->addButton ( i18n( "&Add images" ) );
    QWhatsThis::add( m_addImagesButton, i18n("<p>Add images to the list.") );
    QPushButton* m_remImagesButton = imagesListButtonBox->addButton ( i18n( "&Remove images" ));
    QWhatsThis::add( m_remImagesButton, i18n("<p>Remove images from the list.") );
    imagesListButtonBox->layout();
    grid->addMultiCellWidget(imagesListButtonBox, 0, 1, 2, 2);

    m_imageLabel = new QLabel( m_groupBoxImageList );
    m_imageLabel->setFixedHeight( 120 );
    m_imageLabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    m_imageLabel->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    m_imageLabel->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
    QWhatsThis::add( m_imageLabel, i18n( "<p>Preview of the selected image in the list." ) );
    grid->addMultiCellWidget(m_imageLabel, 2, 2, 2, 2);

    vlay->addWidget( m_groupBoxImageList );

    //---------------------------------------------

    QGroupBox * groupBox2 = new QGroupBox( i18n("Image description"), page_setupImagesList );
    groupBox2->setColumnLayout(0, Qt::Vertical );
    groupBox2->layout()->setSpacing( 6 );
    groupBox2->layout()->setMargin( 11 );
    QWhatsThis::add( groupBox2, i18n("<p>The description of the current image in the list.") );

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

void SendImagesDialog::setImagesList(QStringList Files)
{
    if (Files.isEmpty()) return;

    for ( QStringList::Iterator it = Files.begin() ; it != Files.end() ; ++it )
      {
      QString currentFile = *it;
      QFileInfo fi(currentFile);
      QString Temp = fi.dirPath();
      QString albumName = Temp.section('/', -1);
      Digikam::AlbumInfo *Album = Digikam::AlbumManager::instance()->findAlbum( albumName );
      Album->openDB();
      QString comments = Album->getItemComments(fi.fileName());
      Album->closeDB();

      // Check if the new item already exist in the list.

      bool findItem = false;

      for (uint i = 0 ; i < m_ImagesFilesListBox->count() ; ++i)
          {
          ImageItem *pitem = static_cast<ImageItem*>( m_ImagesFilesListBox->item(i) );

          if (pitem->path() == currentFile.section('/', 0, -1))
             findItem = true;
          }

      if (findItem == false)
         {
         ImageItem *item = new ImageItem( m_ImagesFilesListBox,
                                          currentFile.section('/', -1 ),   // File name with extension.
                                          comments,                        // Image comments.
                                          currentFile.section('/', 0, -1), // Complete path with file name.
                                          albumName                        // Album name.
                                        );

         item->setName( currentFile.section('/', -1) );
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
    QWhatsThis::add( m_mailAgentName, i18n("<p>Select here your prefered external mail agent program."
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
                                         "will be added like an attached file.") );
    vlay->addWidget( m_addComments );

    //---------------------------------------------

    QGroupBox * groupBox2 = new QGroupBox( i18n("Images properties"), page_setupEmailOptions );
    groupBox2->setColumnLayout(0, Qt::Vertical );
    groupBox2->layout()->setSpacing( 6 );
    groupBox2->layout()->setMargin( 11 );
    QWhatsThis::add( groupBox2, i18n("<p>The properties of images to send.") );

    QVBoxLayout * groupBox2Layout = new QVBoxLayout( groupBox2->layout() );
    groupBox2Layout->setAlignment( Qt::AlignTop );

    m_changeImagesProp = new QCheckBox(i18n("Change images properties to send"), groupBox2);
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
                     "<b>%1</b>: used that if you have a very slow internet "
                     "connexion and if the target mailling box size is very limited.<p>"
                     "<b>%2</b>: used that if you have a slow internet connexion "
                     "and if the target mailling box size is limited.<p>"
                     "<b>%3</b>: this is the default value for a medium internet connexion "
                     "and a target mailling box size.<p>"
                     "<b>%4</b>: used that if you have a speed internet connexion "
                     "and if the target mailling box size isn't limited.<p>"
                     "<b>%5</b>: used that if you have none size and speed restriction.<p>")
                     .arg(i18n("very small (320 pixels)"))
                     .arg(i18n("small (640 pixels)"))
                     .arg(i18n("medium (800 pixels)"))
                     .arg(i18n("big (1024 pixels)"))
                     .arg(i18n("very big (1280 pixels)"));
    QWhatsThis::add( m_imagesResize, whatsThis );

    m_labelImageSize = new QLabel( i18n("New images size:"), groupBox2);
    hlay12->addWidget( m_labelImageSize );
    m_labelImageSize->setBuddy( m_imagesResize );
    hlay12->addStretch( 1 );
    hlay12->addWidget(m_imagesResize);

    //---------------------------------------------

    m_imageCompression = new KIntNumInput(75, groupBox2);
    m_imageCompression->setRange(1, 100, 1, true );
    m_imageCompression->setLabel( i18n("New images compression:") );
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
    whatsThis = whatsThis + i18n("<b>JPEG</b>: The Joint Photographic Experts Group file format "
                "is a good Web file format but it use a compression with data lost.<p>"
                "<b>PNG</b>: the Portable Network Graphics format is an extensible file format for "
                "the lossless, portable, well-compressed storage of raster images. PNG provides a "
                "patent-free replacement for GIF and can also replace many common uses of TIFF. "
                "PNG is designed to work well in online viewing applications, such as the World Wide Web, "
                "so it is fully streamable with a progressive display option. Also, PNG can store gamma "
                "and chromaticity data for improved color matching on heterogeneous platforms.");
    QWhatsThis::add( m_imagesFormat, whatsThis );

    m_labelImageFormat = new QLabel( i18n("Images file format:"), groupBox2);
    hlay13->addWidget( m_labelImageFormat );
    m_labelImageFormat->setBuddy( m_imagesFormat );
    hlay13->addStretch( 1 );
    hlay13->addWidget(m_imagesFormat);

    vlay->addWidget( groupBox2 );
    vlay->addStretch(1);

    //---------------------------------------------

    connect(m_mozillaTimer, SIGNAL(timeout()),
            this, SLOT(slotMozillaTimeout()));

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
    page_about = addPage( i18n("About"), i18n("About Digikam E-mail Images"),
                          BarIcon("digikam", KIcon::SizeMedium ) );

    QVBoxLayout *vlay = new QVBoxLayout( page_about, 0, spacingHint() );

    QLabel *label = new QLabel( i18n("A Digikam plugin for e-mail images\n\n"
                                     "Author: Gilles Caulier\n\n"
                                     "Email: caulier dot gilles at free.fr\n\n"), page_about);

    vlay->addWidget(label);
    vlay->addStretch(1);
}


//////////////////////////////////////// SLOTS //////////////////////////////////////////////

void SendImagesDialog::slotAddDropItems(QStringList filesPath)
{
    setImagesList(filesPath);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void SendImagesDialog::slotImagesFilesButtonAdd( void )
{
    QStringList ImageFilesList;

    ImageFilesList = KFileDialog::getOpenFileNames( Digikam::AlbumManager::instance()->getLibraryPath(),
                                                    m_ImagesFilesSort,
                                                    this );

    if ( ImageFilesList.isEmpty() )
       return;

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
    if (!m_thumbJob.isNull())
       delete m_thumbJob;

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

    QString IdemIndexed = "file:" + pitem->path();
    KURL url(IdemIndexed);

    m_thumbJob = new Digikam::ThumbnailJob( url, m_imageLabel->height(), false, true );

    connect(m_thumbJob, SIGNAL(signalThumbnail(const KURL&, const QPixmap&)),
            SLOT(slotGotPreview(const KURL&, const QPixmap&)));
}


/////////////////////////////////////////////////////////////////////////////////////////////

void SendImagesDialog::slotGotPreview(const KURL &url, const QPixmap &pixmap)
{
    m_imageLabel->setPixmap(pixmap);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void SendImagesDialog::slotCancelled()
{
    m_cancelled = true;
    removeTmpFiles();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void SendImagesDialog::slotOk()
{
    if ( m_ImagesFilesListBox->count() == 0 )
       {
       KMessageBox::error(0, i18n("You must add some images to send!"));
       return;
       }

    close();
    writeSettings();

    // Prepare data to send.

    QStringList imagesfileList;
    QString ImageCommentsText = "";
    QStringList imagesResizedWithError;
    m_cancelled = false;

    m_progressDlg = new QProgressDialog(0, "progressDlg", true );
    m_progressDlg->setCaption( i18n("Creating images"));
    m_progressDlg->setCancelButtonText(i18n("&Cancel"));
    m_progressDlg->show();

    connect(m_progressDlg, SIGNAL( cancelled() ),
            this, SLOT( slotCancelled() ) );

    m_progressDlg->setTotalSteps( m_ImagesFilesListBox->count() );
    int imgIndex = 0;

    // Main loop for data send creation.

    for (uint i = 0 ; !m_cancelled && i < m_ImagesFilesListBox->count() ; ++i)
        {
        ImageItem *pitem = static_cast<ImageItem*>( m_ImagesFilesListBox->item(i) );
        QString imageName = pitem->path();
        imagesfileList.append (imageName);

        QString ItemName = imageName.section( '/', -1 );

        m_progressDlg->setLabelText( i18n("Creating image \"%1\" to send").arg(ItemName) );
        m_progressDlg->setProgress( imgIndex );
        kapp->processEvents();
        ++imgIndex;

        // Prepare resized target images to send.

        if ( m_changeImagesProp->isChecked() == true )
           {
           // Prepare resizing images and items comments.

           QString imageFormat = m_imagesFormat->currentText();
           QString imageFileName = ItemName;
           QString imageNameFormat = pitem->album() + "-" + imageFileName + extension(imageFormat);
           int sizeFactor = getSize( m_imagesResize->currentItem() );
           int imageCompression = m_imageCompression->value();

           qDebug("Resizing '%s' -> '%s%s' (%s)", imageName.ascii(),
                  m_tempPath.ascii(), imageNameFormat.ascii(), imageFormat.ascii());

           if ( ResizeImage( imageName, m_tempPath, imageFormat, imageNameFormat,
                             sizeFactor, imageCompression) == false )
               {
               // Resized images failed...

               m_progressDlg->setLabelText( i18n("Creating image \"%1\" failed").arg(ItemName) );
               kapp->processEvents();
               imagesResizedWithError.append(imageName);
               }
           else          // Resized images OK...
               {
               // List of files to send.

               m_imagesSendList.append (m_tempPath + imageNameFormat);

               // Prepare items comments.

               if ( m_addComments->isChecked() == true )
                  {
                  QString commentItem;

                  if ( pitem->comments() != "" )
                     commentItem = pitem->comments();
                  else
                     commentItem = i18n("no comment");

                  ImageCommentsText = ImageCommentsText
                                      + i18n("Comments for image \"%1\" from Album \"%2\": %3\n")
                                      .arg(imageNameFormat).arg(pitem->album()).arg(commentItem);
                  }
               }
           }
        else     // No resize images operations...
           {
           // List of files to send.

           m_imagesSendList.append (imageName);

           // Prepare items comments.

           if ( m_addComments->isChecked() == true )
              {
              QString ItemName2 = imageName;
              ItemName2 = ItemName2.section( '/', -1 );
              QString commentItem;

              if ( pitem->comments() != "" )
                 pitem->comments();
              else
                 commentItem = i18n("no comment");

              ImageCommentsText = ImageCommentsText +
                                  i18n("Comments for image \"%1\" from Album \"%2\": %3\n")
                                  .arg(ItemName2).arg(pitem->album()).arg(commentItem);
              }
           }
        }

    // End of the main loop.

    delete m_progressDlg;

    if ( m_cancelled == true ) // Resizing images process canceled by user
       {                       // -> bye without removed tmp folder (already done!)...
       delete this;
       return;
       }

    if ( imagesResizedWithError.isEmpty() == false )
       {
       // If we have some errors during resized images process...

       listImagesErrorDialog *ErrorImagesDialog = new listImagesErrorDialog(0,
                                                  i18n("Error during resize images process"),
                                                  i18n("Cannot resize this images files :"),
                                                  i18n("Do you want added this images files like\nattachments "
                                                       "(not resizing)?"),
                                                  imagesResizedWithError);
       int ValRet = ErrorImagesDialog->exec();

       switch (ValRet)
         {
         case KDialogBase::Yes :         // Added source image files instead resized images...

            for ( QStringList::Iterator it = imagesResizedWithError.begin();
                                        it != imagesResizedWithError.end(); ++it )
                {
                m_imagesSendList.append (*it);
                kapp->processEvents();
                if ( m_addComments->isChecked() == true )
                   {
                   QString ItemName = *it;
                   ItemName = ItemName.section( '/', -1 );
                   QString commentItem;

                   ImageItem *pitem = static_cast<ImageItem*>( m_ImagesFilesListBox->findItem(*it) );

                   if (pitem)
                      {
                      if ( pitem->comments() != "" )
                         commentItem = pitem->comments();
                      else
                         commentItem = i18n("no comment");

                      ImageCommentsText = ImageCommentsText
                                          + i18n("Comments for image \"%1\" from Album \"%2\": %3\n")
                                          .arg(ItemName).arg(pitem->album()).arg(commentItem);
                      }
                   }
                }

            break;

         case KDialogBase::No :         // Do nothing...
            break;

         case KDialogBase::Cancel :     // Stop process...
            removeTmpFiles();
            delete this;
            return;
            break;
         }
       }

    // Create a text file with the comments.

    if ( m_addComments->isChecked() == true )
       {
       QFile commentsFile( m_tempPath + i18n("comments.txt") );
       QTextStream stream( &commentsFile );
       commentsFile.open( IO_ReadWrite );
       stream << ImageCommentsText << "\n";
       commentsFile.close();
       m_imagesSendList.append( m_tempPath + i18n("comments.txt") );
       }

    // Invoke mailer agent.

    if ( m_mailAgentName->currentText() == "Kmail" )      // Kmail agent call.
       {
       KApplication::kApplication()->invokeMailer(
                       QString::null,                     // Destination address.
                       QString::null,                     // Carbon Copy address.
                       QString::null,                     // Blind Carbon Copy address
                       QString::null,                     // Message Subject.
                       QString::null,                     // Message Body.
                       QString::null,                     // Message Body File.
                       m_imagesSendList);                 // Images attachments (+ comments).
       }

    // Sylpheed mail agent call.

    if ( m_mailAgentName->currentText() == "Sylpheed" )
       {
       m_mailAgentProc = new KProcess;
       *m_mailAgentProc << "sylpheed" << "--compose";
       *m_mailAgentProc << "--attach";

       for ( QStringList::Iterator it = m_imagesSendList.begin() ; it != m_imagesSendList.end() ; ++it )
           *m_mailAgentProc << *it;

       if (m_mailAgentProc->start() == false)
          KMessageBox::error(0, i18n("Cannot start '%1' program.\nPlease, "
                                     "check your installation!").arg(m_mailAgentName->currentText()));
       }

    // Balsa mail agent call.

    if ( m_mailAgentName->currentText() == "Balsa" )
       {
       m_mailAgentProc = new KProcess;
       *m_mailAgentProc << "balsa" << "-m" << "mailto:";

       for ( QStringList::Iterator it = m_imagesSendList.begin() ; it != m_imagesSendList.end() ; ++it )
           {
           *m_mailAgentProc << "-a";
           *m_mailAgentProc << *it;
           }

       if (m_mailAgentProc->start() == false)
          KMessageBox::error(0, i18n("Cannot start '%1' program.\nPlease, "
                                     "check your installation!").arg(m_mailAgentName->currentText()));
       }

    if ( m_mailAgentName->currentText() == "Evolution" )    // Evolution mail agent call.
       {
       m_mailAgentProc = new KProcess;
       *m_mailAgentProc << "evolution";

       QString Temp = "mailto:?subject=";

       for ( QStringList::Iterator it = m_imagesSendList.begin() ; it != m_imagesSendList.end() ; ++it )
           {
           Temp.append("&attach=");
           Temp.append( *it );
           }

       *m_mailAgentProc << Temp;

       if (m_mailAgentProc->start() == false)
          KMessageBox::error(0, i18n("Cannot start '%1' program.\nPlease, "
                                     "check your installation!").arg(m_mailAgentName->currentText()));
       }

    // Mozilla | Netscape | Thunderbird mail agent call.

    if ( m_mailAgentName->currentText() == "Mozilla" || m_mailAgentName->currentText() == "Netscape" ||
         m_mailAgentName->currentText() == "Thunderbird" )
       {
       m_mailAgentProc = new KProcess;

       if (m_mailAgentName->currentText() == "Mozilla")
          *m_mailAgentProc << "mozilla" << "-remote";
       else
          if (m_mailAgentName->currentText() == "Thunderbird")
             *m_mailAgentProc << "thunderbird" << "-remote";
          else
             *m_mailAgentProc << "netscape" << "-remote";

       QString Temp = "xfeDoCommand(composeMessage,attachment='";

       for ( QStringList::Iterator it = m_imagesSendList.begin() ; it != m_imagesSendList.end() ; ++it )
           {
           Temp.append( "file://" );
           Temp.append( *it );
           Temp.append( "," );
           }

       Temp.append("')");

       *m_mailAgentProc << Temp;

       connect(m_mailAgentProc, SIGNAL(processExited(KProcess *)), this, SLOT(slotMozillaExited(KProcess*)));

       connect(m_mailAgentProc, SIGNAL(receivedStderr(KProcess *, char*, int)),
               this, SLOT(slotMozillaReadStderr(KProcess*, char*, int)));

       qDebug ("%s", Temp.ascii());

       if (m_mailAgentProc->start(KProcess::NotifyOnExit , KProcess::All) == false)
          KMessageBox::error(0, i18n("Cannot start '%1' program.\nPlease, "
                                     "check your installation!").arg(m_mailAgentName->currentText()));
       else return;
       }

    // Close this dialog.

    delete this;
}


////////////////////////////////////////// FONCTIONS ////////////////////////////////////////////////

int SendImagesDialog::getSize ( int choice )
{
    switch (choice)
       {
       case 0:
          return (320);
          break;
       case 1:
          return (640);
          break;
       case 2:
          return (800);
          break;
       case 3:
          return (1024);
          break;
       case 4:
          return (1280);
          break;
       default:
          return (800); // Default value...
          break;
       }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void SendImagesDialog::slotMozillaExited(KProcess* proc)
{
    if ( m_mozillaStdErr.contains("No running window found.") == true )   // No remote Mozilla | Netscape |
       {                                                                  // Thunderbird env. loaded !
       m_mailAgentProc2 = new KProcess;                                   // Init a new env.

       if (m_mailAgentName->currentText() == "Mozilla")
          *m_mailAgentProc2 << "mozilla" << "-mail";
       else
          if (m_mailAgentName->currentText() == "Thunderbird")
             *m_mailAgentProc2 << "thunderbird" << "-mail";
          else
             *m_mailAgentProc2 << "netscape" << "-mail";

       if (m_mailAgentProc2->start() == false)     // Start an instance of mozilla mail agent before a remote call.
          {
          KMessageBox::error(0, i18n("Cannot start '%1' program.\nPlease, "
                                     "check your installation!").arg(m_mailAgentName->currentText()));
          delete this;
          }
       else
          {
          // Mozilla | Netscape | Thunderbird mail agent started correctly
          // -> start a remote mail agent with multiple attachments after the env. is loaded !

          m_mozillaTimer->start(5000, true);
          return; // Don't close this dialog -> will be closed by the end timer slot.
          }
       }
    else
       delete this;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void SendImagesDialog::slotMozillaTimeout(void)
{
    m_mailAgentProc3 = new KProcess;

    if (m_mailAgentName->currentText() == "Mozilla")
       *m_mailAgentProc3 << "mozilla" << "-remote";
    else
       if (m_mailAgentName->currentText() == "Thunderbird")
          *m_mailAgentProc3 << "thunderbird" << "-remote";
       else
          *m_mailAgentProc3 << "netscape" << "-remote";

    QString Temp = "xfeDoCommand(composeMessage,attachment='";

    for ( QStringList::Iterator it = m_imagesSendList.begin() ; it != m_imagesSendList.end() ; ++it )
        {
        Temp.append( "file://" );
        Temp.append( *it );
        Temp.append( "," );
        }

    Temp.append("')");

    *m_mailAgentProc3 << Temp;

    if (m_mailAgentProc3->start() == false)
       KMessageBox::error(0, i18n("Cannot start '%1' program.\nPlease, "
                                  "check your installation!").arg(m_mailAgentName->currentText()));

    // Close this dialog.

    delete this;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void SendImagesDialog::slotMozillaReadStderr(KProcess* proc, char *buffer, int buflen)
{
    m_mozillaStdErr = QString::fromLocal8Bit(buffer, buflen);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

bool SendImagesDialog::ResizeImage( const QString &SourcePath, const QString &DestPath,
                                    const QString &ImageFormat, const QString &ImageName,
                                    int SizeFactor, int ImageCompression)
{
    QImage img;

    if ( img.load(SourcePath) == true )
       {
       int w = img.width();
       int h = img.height();

       if( w > SizeFactor || h > SizeFactor )
           {
           if( w > h )
               {
               h = (int)( (double)( h * SizeFactor ) / w );

               if ( h == 0 ) h = 1;

               w = SizeFactor;
               Q_ASSERT( h <= SizeFactor );
               }
           else
               {
               w = (int)( (double)( w * SizeFactor ) / h );

               if ( w == 0 ) w = 1;

               h = SizeFactor;
               Q_ASSERT( w <= SizeFactor );
               }

           const QImage scaleImg(img.smoothScale( w, h ));

           if ( scaleImg.width() != w || scaleImg.height() != h )
               {
               qDebug("Resizing failed. Aborting.");
               return false;
               }

           img = scaleImg;
           }

        if ( !img.save(DestPath + ImageName, ImageFormat.latin1(), ImageCompression) )
           {
           qDebug("Saving failed with specific compression value. Aborting.");
           return false;
           }

        return true;
        }

    return false;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

QString SendImagesDialog::extension(const QString& imageFileFormat)
{
    if (imageFileFormat == "PNG")
        return ".png";

    if (imageFileFormat == "JPEG")
        return ".jpg";

    Q_ASSERT(false);
    return "";
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void SendImagesDialog::removeTmpFiles(void)
{
    if (DeleteDir(m_tempPath) == false)
       KMessageBox::error(0, i18n("Cannot remove temporary folder %1!").arg(m_tempPath));
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void SendImagesDialog::setNbItems(void)
{
    m_groupBoxImageList->setTitle(i18n("Images list (%1 items)")
                                  .arg(m_ImagesFilesListBox->count()));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

bool SendImagesDialog::DeleteDir(QString dirname)
{
if (dirname != "")
    {
    QDir dir;

    if (dir.exists ( dirname ) == true)
       {
       if (deldir(dirname) == false)
           return false;

       if (dir.rmdir( dirname ) == false )
           return false;
       }
    else
       return false;
    }
else
    return false;

return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

bool SendImagesDialog::deldir(QString dirname)
{
QDir *dir = new QDir(dirname);
dir->setFilter ( QDir::Dirs | QDir::Files | QDir::NoSymLinks );

const QFileInfoList* fileinfolist = dir->entryInfoList();
QFileInfoListIterator it(*fileinfolist);
QFileInfo* fi;

while( (fi = it.current() ) )
     {
     if(fi->fileName() == "." || fi->fileName() == ".." )
          {
          ++it;
          continue;
          }

     if( fi->isDir() )
          {
          if (deldir( fi->absFilePath() ) == false)
              return false;
          if (dir->rmdir( fi->absFilePath() ) == false)
              return false;
          }
     else
          if( fi->isFile() )
               if (dir->remove(fi->absFilePath() ) == false)
                   return false;

     kapp->processEvents();
     ++it;
     }

return true;
}

