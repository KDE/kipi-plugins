//////////////////////////////////////////////////////////////////////////////
//
//    ACQUIREIMAGEDIALOG.CPP
//
//    Copyright (C) 2003 Gilles CAULIER <caulier.gilles at free.fr>
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

// C Ansi includes

extern "C"
{
#include <tiffio.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
}

// Include files for Qt

#include <qvbox.h>
#include <qlayout.h>
#include <qdir.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qstring.h>
#include <qwhatsthis.h>
#include <qtextedit.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qcombobox.h>
#include <qlistbox.h>
#include <qfile.h>

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
#include <klistbox.h>
#include <klineeditdlg.h>
#include <ksqueezedtextlabel.h>
#include <kio/netaccess.h>
#include <kimageio.h>

#include <libkipi/thumbnailjob.h>

// Local includes

#include "acquireimagedialog.h"
#include <kdebug.h>
#include <kfiletreeview.h>
#include <qapplication.h>


class AlbumItem : public QListBoxText
{
public:
    AlbumItem(QListBox * parent, QString const & name, QString const & comments, QString const & path,
              QString const & collection, QString const & firstImage, QDate const & date, int const & items)
            : QListBoxText( parent), _name(name), _comments(comments), _path(path), _collection(collection),
              _firstImage(firstImage), _date (date), _items (items)
    {}

    QString comments()                   { return _comments;   }
    QString name()                       { return _name;       }
    QString path()                       { return _path;       }
    QString collection()                 { return _collection; }
    QString firstImage()                 { return _firstImage; }
    QDate   date()                       { return _date;       }
    int     items()                      { return _items;      }
    void setName(const QString &newName) { setText(newName);   }
    void incItem ()                      { ++_items;           }

private:
    QString _name;
    QString _comments;
    QString _path;
    QString _collection;
    QString _firstImage;
    QDate   _date;
    int     _items;
};


//////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////////

AcquireImageDialog::AcquireImageDialog( KIPI::Interface* interface, QWidget *parent, const QImage &img)
                  : KDialogBase( IconList, i18n("Save target image options"), Help|Ok|Cancel,
                    Ok, parent, "AcquireImageDialog", true, true ), m_interface( interface ), m_dialogOk( false )
{
    KImageIO::registerFormats();
    m_qimageScanned = img;

    setupImageOptions();
    setupAlbumsList();
    aboutPage();
    readSettings();
    setHelp("plugin-acquireimages.anchor", "digikam");
    setAlbumsList();
    slotImageFormatChanged(m_imagesFormat->currentText());
    page_setupImageOptions->setFocus();
    resize( 600, 400 );
}


//////////////////////////////////// DESTRUCTOR /////////////////////////////////////////////

AcquireImageDialog::~AcquireImageDialog()
{
    if (!m_thumbJob.isNull())
       delete m_thumbJob;
}


/////////////////////////////////////////////////////////////////////////////////////////////

void AcquireImageDialog::readSettings(void)
{
    // Read all settings from configuration file.

    m_config = new KConfig("digikamrc");
    m_config->setGroup("AcquireImages Settings");
    m_FileName->setText(m_config->readEntry("DefaultImageFileName", i18n("image")));
    m_imageCompression->setValue(m_config->readNumEntry("ImageCompression", 75));
    m_imagesFormat->setCurrentText(m_config->readEntry("ImageFormat", "TIFF"));

    // Read File Filter settings in digikamrc file.

    m_config->setGroup("Album Settings");
    QString Temp = m_config->readEntry("File Filter", "*.jpg *.jpeg *.tif *.tiff *.gif *.png *.bmp");
    m_ImagesFilesSort = Temp.lower() + " " + Temp.upper();

    delete m_config;
}


/////////////////////////////////////////////////////////////////////////////////////////////

void AcquireImageDialog::writeSettings(void)
{
    // Write all settings in configuration file.

    m_config = new KConfig("digikamrc");
    m_config->setGroup("AcquireImages Settings");
    m_config->writeEntry("DefaultImageFileName", m_FileName->text());
    m_config->writeEntry("ImageCompression", m_imageCompression->value());
    m_config->writeEntry("ImageFormat", m_imagesFormat->currentText());
    m_config->sync();
    delete m_config;
}


/////////////////////////////////////////////////////////////////////////////////////////////

void AcquireImageDialog::setupImageOptions(void)
{
    QString whatsThis;

    page_setupImageOptions = addPage( i18n("Target image"),
                                      i18n("Target image options"),
                                      BarIcon("image", KIcon::SizeMedium ) );

    QVBoxLayout *vlay = new QVBoxLayout( page_setupImageOptions, 0, spacingHint() );

    //---------------------------------------------

    QGroupBox * groupBox1 = new QGroupBox( page_setupImageOptions );
    groupBox1->setFlat(false);
    groupBox1->setTitle(i18n("File name and comments"));
    QWhatsThis::add( groupBox1, i18n("<p>The target image preview with the file name and comments.") );
    QGridLayout* grid2 = new QGridLayout( groupBox1, 3, 3 , 20, 10);

    m_ImageFileName = new QLabel( i18n("File name (without suffix):"), groupBox1);
    grid2->addMultiCellWidget(m_ImageFileName, 0, 0, 0, 3);

    m_FileName = new QLineEdit(i18n("acquired_image"), groupBox1);
    QWhatsThis::add( m_FileName, i18n("<p>Enter here the target image file name without suffix. "
                                      "It's will be automaticly added to the file name in according "
                                      "with the file format option.") );
    m_ImageFileName->setBuddy(m_FileName);
    grid2->addMultiCellWidget(m_FileName, 1, 1, 0, 3);

    m_ImageComments = new QLabel( i18n("Comments:"), groupBox1);
    grid2->addMultiCellWidget(m_ImageComments, 2, 2, 0, 3);

    m_CommentsEdit = new QTextEdit(groupBox1);
    m_CommentsEdit->setMaximumHeight( 200 );
    QWhatsThis::add( m_CommentsEdit, i18n("<p>Enter here the target image comments.") );
    grid2->addMultiCellWidget(m_CommentsEdit, 3, 3, 0, 2);

    m_preview = new QLabel( groupBox1, "preview" );
    m_preview->setFixedHeight( 120 );
    m_preview->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    m_preview->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
    QWhatsThis::add( m_preview, i18n( "<p>The preview of the target image." ) );
    m_preview->setScaledContents( false );
    QImage scanned = m_qimageScanned.smoothScale((m_qimageScanned.width() * 100) / m_qimageScanned.height(), 100);
    QPixmap pix;
    pix.convertFromImage(scanned);
    m_preview->setPixmap(pix);
    grid2->addMultiCellWidget(m_preview, 3, 3, 3, 3);

    vlay->addWidget( groupBox1 );

    //---------------------------------------------

    QGroupBox * groupBox2 = new QGroupBox( i18n("Saving options"), page_setupImageOptions );
    groupBox2->setColumnLayout(0, Qt::Vertical );
    groupBox2->layout()->setSpacing( 6 );
    groupBox2->layout()->setMargin( 11 );
    QWhatsThis::add( groupBox2, i18n("<p>The saving options of the target image.") );

    QVBoxLayout * groupBox2Layout = new QVBoxLayout( groupBox2->layout() );
    groupBox2Layout->setAlignment( Qt::AlignTop );

    m_imageCompression = new KIntNumInput(75, groupBox2);
    m_imageCompression->setRange(1, 100, 1, true );
    m_imageCompression->setLabel( i18n("Image compression:") );
    whatsThis = i18n("<p>The compression value of target image for JPEG and PNG formats:<p>");
    whatsThis = whatsThis + i18n("<b>1</b>: very high compression<p>"
                                 "<b>25</b>: high compression<p>"
                                 "<b>50</b>: medium compression<p>"
                                 "<b>75</b>: low compression (default value)<p>"
                                 "<b>100</b>: no compression");

    QWhatsThis::add( m_imageCompression, whatsThis);
    groupBox2Layout->addWidget( m_imageCompression );

    QHBoxLayout *hlay12  = new QHBoxLayout( );
    groupBox2Layout->addLayout( hlay12 );

    m_imagesFormat = new QComboBox(false, groupBox2);
    m_imagesFormat->insertItem("JPEG");
    m_imagesFormat->insertItem("PNG");
    m_imagesFormat->insertItem("TIFF");
    m_imagesFormat->insertItem("PPM");
    m_imagesFormat->insertItem("BMP");
    m_imagesFormat->setCurrentText ("TIFF");
    whatsThis = i18n("<p>Select here the target image file format.<p>");
    whatsThis = whatsThis + i18n("<b>JPEG</b>: The Joint Photographic Experts Group "
                "file format is a good Web file format but it use a compression with data lost.<p>"
                "<b>PNG</b>: the Portable Network Graphics format is an extensible file format for "
                "the lossless, portable, well-compressed storage of raster images. PNG provides a "
                "patent-free replacement for GIF and can also replace many common uses of TIFF. PNG "
                "is designed to work well in online viewing applications, such as the World Wide Web, "
                "so it is fully streamable with a progressive display option. Also, PNG can store gamma "
                "and chromaticity data for improved color matching on heterogeneous platforms.");
    whatsThis = whatsThis + i18n("<p><b>TIFF</b>: the Tag Image File Format is a rather old standard "
                "that is still very popular today. It is a highly flexible and platform-independent "
                "format which is supported by numerous image processing applications and virtually all "
                "prepress software on the market.");
    whatsThis = whatsThis + i18n("<p><b>PPM</b>: the Portable Pixel Map file format is used as an "
                "intermediate format for storing color bitmap informations. PPM files may be either "
                "binary or ASCII and store pixel values up to 24 bits in size. This format generate "
                "very biggest sized text files for encode images without losing quality");
    whatsThis = whatsThis + i18n("<p><b>BMP</b>: the BitMaP file format is a popular image format from "
                "Win32 environment. It efficiently stores mapped or unmapped RGB graphics data with pixels "
                "1-, 4-, 8-, or 24-bits in size. Data may be stored raw or compressed using a 4-bit or "
                "8-bit RLE data compression algorithm. BMP is an excellent choice for a simple bitmap "
                "format which supports a wide range of RGB image data.");
    QWhatsThis::add( m_imagesFormat, whatsThis );

    m_labelImageFormat = new QLabel( i18n("Image file format:"), groupBox2);
    hlay12->addWidget( m_labelImageFormat );
    m_labelImageFormat->setBuddy( m_imagesFormat );
    hlay12->addStretch( 1 );
    hlay12->addWidget( m_imagesFormat );

    vlay->addWidget( groupBox2 );
    vlay->addStretch(1);

    //---------------------------------------------

    connect(m_imagesFormat, SIGNAL(activated(const QString &)),
            this, SLOT(slotImageFormatChanged(const QString &)));
}


/////////////////////////////////////////////////////////////////////////////////////////////

void AcquireImageDialog::setupAlbumsList(void)
{
    QString whatsThis;

    page_setupAlbumsList = addPage( i18n("Selection"),
                                    i18n("Album selection"),
                                    BarIcon("endturn", KIcon::SizeMedium ) );

    QVBoxLayout *vlay = new QVBoxLayout( page_setupAlbumsList, 0, spacingHint() );

    //---------------------------------------------

    QGroupBox * groupBox1 = new QGroupBox( page_setupAlbumsList );
    groupBox1->setFlat(false);
    groupBox1->setTitle(i18n("Select Album for to save the target image"));
    QGridLayout* grid = new QGridLayout( groupBox1, 2, 2 , 20, 20);

    m_uploadPath = new KIPI::UploadWidget( m_interface, groupBox1, "m_uploadPath" );
    grid->addMultiCellWidget( m_uploadPath, 0, 2, 0, 1 );


    m_addNewAlbumButton = new QPushButton (groupBox1, "PushButton_AddNewAlbum");
    m_addNewAlbumButton->setText(i18n( "&Add new album") );
    m_addNewAlbumButton->setAutoRepeat( false );
    QWhatsThis::add( m_addNewAlbumButton, i18n( "<p>Add a new Album in the Digikam Albums library."));

    grid->addMultiCellWidget(m_addNewAlbumButton, 0, 1, 2, 2);

    m_albumPreview = new QLabel( groupBox1 );
    m_albumPreview->setFixedHeight( 120 );
    m_albumPreview->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    m_albumPreview->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    m_albumPreview->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
    QWhatsThis::add( m_albumPreview, i18n( "<p>Preview of the first image in the current selected Album." ) );
    grid->addMultiCellWidget(m_albumPreview, 2, 2, 2, 2);

    vlay->addWidget( groupBox1 );

    //---------------------------------------------

    QGroupBox * groupBox2 = new QGroupBox( i18n("Album description"), page_setupAlbumsList );
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

    vlay->addWidget( groupBox2 );
    if ( !m_interface->hasFeature( KIPI::AlbumsHaveDescriptions ) )
        groupBox2->hide();
    else
        vlay->addStretch(1);

    //---------------------------------------------

    connect(m_addNewAlbumButton, SIGNAL(clicked()),
            this, SLOT(slotAddNewAlbum()));

#ifdef TEMPORARILY_REMOVED
    connect(m_AlbumList, SIGNAL( currentChanged( QListBoxItem * ) ),
            this, SLOT( albumSelected( QListBoxItem * )));
#endif
    }


/////////////////////////////////////////////////////////////////////////////////////////////

void AcquireImageDialog::setAlbumsList(void)
{
#ifdef TEMPORARILY_REMOVED
    QString CurrentAlbum;

    for (Digikam::AlbumInfo *album=Digikam::AlbumManager::instance()->firstAlbum() ;
         album ; album = album->nextAlbum())
        {
        album->openDB();
        QDir imageDir( album->getPath(), m_ImagesFilesSort.latin1(),
                       QDir::Name|QDir::IgnoreCase, QDir::Files|QDir::Readable);

        int nbfiles = imageDir.count();

        if ( nbfiles < 0 ) nbfiles = 0;

        AlbumItem *item = new AlbumItem( m_AlbumList,
                                         album->getTitle(),
                                         album->getComments(),
                                         album->getPath(),
                                         album->getCollection(),
                                         imageDir.entryList().first(),
                                         album->getDate(),
                                         nbfiles
                                        );

        item->setName( album->getTitle() );

        if (album == Digikam::AlbumManager::instance()->currentAlbum())
           {
           CurrentAlbum = album->getTitle();
           albumSelected( item );
           }

        album->closeDB();
        }

    m_AlbumList->sort (true);
    m_AlbumList->setSelected( m_AlbumList->findItem(CurrentAlbum), true );
    m_AlbumList->setCurrentItem(m_AlbumList->findItem(CurrentAlbum));
    m_AlbumList->centerCurrentItem();
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////

void AcquireImageDialog::aboutPage(void)
{
    page_about = addPage( i18n("About"), i18n("About Digikam acquire images"),
                          BarIcon("digikam", KIcon::SizeMedium ) );

    QVBoxLayout *vlay = new QVBoxLayout( page_about, 0, spacingHint() );

    QLabel *label = new QLabel( i18n("A Digikam plugin for acquire images\n\n"
                                     "Author: Gilles Caulier\n\n"
                                     "Email: caulier dot gilles at free.fr\n\n"), page_about);

    vlay->addWidget(label);
    vlay->addStretch(1);
}


//////////////////////////////////////// SLOTS //////////////////////////////////////////////


void AcquireImageDialog::albumSelected( QListBoxItem * item )
{
    if ( !item ) return;

    AlbumItem *pitem = static_cast<AlbumItem*>( item );
    if ( pitem == NULL ) return;
    m_AlbumComments->setText( i18n("Comment: %1").arg(pitem->comments()) );
    m_AlbumCollection->setText( i18n("Collection: %1").arg(pitem->collection()) );
    m_AlbumDate->setText( i18n("Date: %1").arg(pitem->date().toString(( Qt::LocalDate ))) );
    m_AlbumItems->setText( i18n("Items: %1").arg( pitem->items() ) );

    checkNewFileName();

    m_albumPreview->clear();

    if (!m_thumbJob.isNull())
       delete m_thumbJob;

    QString IdemIndexed = "file:" + pitem->path() + "/" + pitem->firstImage();
    KURL url(IdemIndexed);

    m_thumbJob = new KIPI::ThumbnailJob( url, m_albumPreview->height(), false, true );

    connect(m_thumbJob, SIGNAL(signalThumbnail(const KURL&, const QPixmap&)),
            SLOT(slotGotPreview(const KURL&, const QPixmap&)));
}


/////////////////////////////////////////////////////////////////////////////////////////////

void AcquireImageDialog::slotGotPreview(const KURL &/*url*/, const QPixmap &pixmap)
{
    m_albumPreview->setPixmap(pixmap);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void AcquireImageDialog::slotAddNewAlbum( void )
{
    kdWarning( 51000 ) << "AcquireImageDialog::slotAddNewAlbum is commented out, what should we do?\n";

// PENDING(blackie) How do we handle this without a library path?
#ifdef TEMPORARILY_REMOVED
    QDir libraryDir( Digikam::AlbumManager::instance()->getLibraryPath() );
    if (!libraryDir.exists())
        {
        KMessageBox::error(0, i18n("Album Library has not been set correctly\n"
                                   "Please run Setup"));
        return;
        }

    bool ok;
    m_newDir = KLineEditDlg::getText(i18n("Enter New Album Name: "), "", &ok, this);
    if (!ok) return;

    KURL newAlbumURL(Digikam::AlbumManager::instance()->getLibraryPath());
    newAlbumURL.addPath(m_newDir);

    KIO::SimpleJob* job = KIO::mkdir(newAlbumURL);
    connect(job, SIGNAL(result(KIO::Job*)), this, SLOT(slot_onAlbumCreate(KIO::Job*)));
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////

void AcquireImageDialog::slot_onAlbumCreate(KIO::Job* job)
{
    if (job->error())
        job->showErrorDialog(this);
    else
        {
#ifdef TEMPORARILY_REMOVED
            AlbumItem *item = new AlbumItem( m_AlbumList, m_newDir, "", "", "", "", QDate::currentDate(), 0);
        item->setName( m_newDir );
        albumSelected( item );
        m_AlbumList->sort (true);
        m_AlbumList->setSelected( item, true );
        m_AlbumList->setCurrentItem( item );
        m_AlbumList->centerCurrentItem();
#endif
        }
}


/////////////////////////////////////////////////////////////////////////////////////////////

  void AcquireImageDialog::slotOk()
{
#ifdef TEMPORARILY_REMOVED
    if (m_AlbumList->currentText().isEmpty())
        {
        KMessageBox::error(0, i18n("You must select a target album for this image!"));
        return;
        }

    if (m_FileName->text().isEmpty())
        {
        KMessageBox::error(0, i18n("You must give a file name for this image!"));
        return;
        }

    writeSettings();

    // Get all scanned image informations.

    int albumSelectedId = m_AlbumList->currentItem();
    QString albumSelectedText = m_AlbumList->text(albumSelectedId);
    Digikam::AlbumInfo *album = Digikam::AlbumManager::instance()->findAlbum(albumSelectedText);
    QString targetAlbumPath = album->getPath();
    QString imageFileName = m_FileName->text();
    QString imageFormat = m_imagesFormat->currentText();
    int imageCompression = m_imageCompression->value();
    QString imagePath = targetAlbumPath + "/" + imageFileName + extension(imageFormat);
    QString Commentsimg = m_CommentsEdit->text();
    QFile Image(imagePath);

    // Saving the target image file.

    if ( Image.exists() == true )
        {
        if (KMessageBox::warningYesNo(0, i18n("The target image \n\"%1\"\nalready exist.\n"
                                              "Do you want overwrite it?").arg(imagePath)) == KMessageBox::No)
           return;
        }

    qDebug("Saving image: %s", imagePath.ascii());
    bool ValRet = false;

    if ( imageFormat == "JPEG" || imageFormat == "PNG" )
       ValRet = m_qimageScanned.save(imagePath, imageFormat.latin1(), imageCompression);

    if ( imageFormat == "PPM" || imageFormat == "BMP" )
       ValRet = m_qimageScanned.save(imagePath, imageFormat.latin1());

    if ( imageFormat == "TIFF" )
       ValRet = QImageToTiff(m_qimageScanned, imagePath);

    if ( ValRet == false )
       {
       KMessageBox::error(0, i18n("Cannot write image file \"%1\"!").arg(imagePath));
       return;
       }

    // Save the comments for this image.

    album->openDB();
    album->setItemComments(imageFileName + extension(imageFormat), Commentsimg);
    album->closeDB();

    // Update the items number for the selected Album
    // in the case if another image is scanned during the plugin session.

    AlbumItem *pitem = static_cast<AlbumItem*>( m_AlbumList->item( albumSelectedId ));
    pitem->incItem();
    Digikam::AlbumManager::instance()->refreshItemHandler(albumSelectedText);
    close();
    delete this;
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void AcquireImageDialog::slotImageFormatChanged(const QString &string)
{
    if ( string == "JPEG" || string == "PNG" )
       m_imageCompression->setEnabled(true);
    else
       m_imageCompression->setEnabled(false);

    checkNewFileName();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

QString AcquireImageDialog::extension(const QString& imageFormat)
{
    if (imageFormat == "PNG")
        return ".png";

    if (imageFormat == "JPEG")
        return ".jpg";

    if (imageFormat == "TIFF")
        return ".tif";

    if (imageFormat == "BMP")
        return ".bmp";

    if (imageFormat == "PPM")
        return ".ppm";

    Q_ASSERT(false);
    return "";
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void AcquireImageDialog::checkNewFileName(void)
{
#ifdef TEMPORARILY_REMOVED
    int albumSelectedId = m_AlbumList->currentItem();
    QString albumSelectedText = m_AlbumList->text(albumSelectedId);

    if ( albumSelectedText == QString::null)
       return;

    Digikam::AlbumInfo *album = Digikam::AlbumManager::instance()->findAlbum(albumSelectedText);
    if (!album) return;
    QString targetAlbumPath = album->getPath();
    QString imageFileName = m_FileName->text().left( m_FileName->text().findRev('_', -1));
    QString imageFormat = m_imagesFormat->currentText();
    QString imagePath = targetAlbumPath + "/" + imageFileName + extension(imageFormat);
    QFileInfo *Target = new QFileInfo(imagePath);
    QString Temp = RenameTargetImageFile(Target);
    QString newFileName = Temp.left( Temp.findRev('.', -1));

    if ( newFileName != QString::null )
       m_FileName->setText( newFileName );
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

QString AcquireImageDialog::RenameTargetImageFile(QFileInfo *fi)
{
    QString Temp;
    int Enumerator = 0;
    KURL NewDestUrl;

    do
       {
       ++Enumerator;
       Temp = Temp.setNum( Enumerator );
       NewDestUrl = fi->filePath().left( fi->filePath().findRev('.', -1)) + "_" + Temp +
                                         extension(m_imagesFormat->currentText());
       }
    while ( Enumerator < 100 && KIO::NetAccess::exists(NewDestUrl) == true );

    if (Enumerator == 100) return QString::null;

    QFileInfo newDestFile(NewDestUrl.path());

    return (newDestFile.fileName());
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

bool AcquireImageDialog::QImageToTiff(const QImage& image, const QString& dst)
{
    TIFF               *tif;
    unsigned char      *data;
    int                 x, y;
    QRgb                rgb;

    tif = TIFFOpen(QFile::encodeName(dst).data(), "w");
    if ( tif )
        {
        TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, image.width());
        TIFFSetField(tif, TIFFTAG_IMAGELENGTH, image.height());
        TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
        TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
        TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
        TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_ADOBE_DEFLATE);
            {
            TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3);
            TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
            TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(tif, 0));

            data = new unsigned char[image.width()*3];
            unsigned char *dptr = 0;

            for (y = 0 ; y < image.height() ; ++y)
                {
                dptr = data;

                for (x = 0 ; x < image.width() ; ++x)
                    {
                    rgb = *((uint *)image.scanLine(y) + x);
                    *(dptr++) = qRed(rgb);
                    *(dptr++) = qGreen(rgb);
                    *(dptr++) = qBlue(rgb);
                    }

                TIFFWriteScanline(tif, data, y, 0);
                }

            delete [] data;
            }

        TIFFClose(tif);
        return true;
        }

    return false;
}


#include "acquireimagedialog.moc"
