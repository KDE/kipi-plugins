//////////////////////////////////////////////////////////////////////////////
//
//    ACQUIREIMAGEDIALOG.CPP
//
//    Copyright (C) 2003 Gilles Caulier <caulier.gilles at free.fr>
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
//    Foundation, Inc., 51 Franklin Steet, Fifth Floor, Cambridge, MA 02110-1301, USA.
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

#undef Unsorted // x headers suck - make qdir.h work with --enable-final
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
#include <qapplication.h>
#include <qvgroupbox.h>

// Include files for KDE

#include <klocale.h>
#include <klineedit.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <kinstance.h>
#include <kconfig.h>
#include <klistbox.h>
#include <klineeditdlg.h>
#include <ksqueezedtextlabel.h>
#include <kio/netaccess.h>
#include <kimageio.h>
#include <ktempfile.h>
#include <kdeversion.h>
#include <kdebug.h>
#include <kfiletreeview.h>
#include <kapplication.h>
#include <kaboutdata.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kpopupmenu.h>

// Include files for libKipi.

#include <libkipi/imageinfo.h>

// Local includes

#include "pluginsversion.h"
#include "acquireimagedialog.h"
#include "acquireimagedialog.moc"

namespace KIPIAcquireImagesPlugin
{

// Used by slotOK() method.

#undef NETACCESS_WIDGET
#if KDE_VERSION >= 0x30200
#define NETACCESS_WIDGET , this
#else
#define NETACCESS_WIDGET
#endif

//////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////////

AcquireImageDialog::AcquireImageDialog( KIPI::Interface* interface, QWidget *parent, const QImage &img)
                  : KDialogBase( IconList, i18n("Save Target Image Options"), Help|Ok|Cancel,
                    Ok, parent, "AcquireImageDialog", true, true ), m_interface( interface )
{
    KImageIO::registerFormats();
    m_qimageScanned = img;

    setupImageOptions();
    setupAlbumsList();
    readSettings();
    slotImageFormatChanged(m_imagesFormat->currentText());
    page_setupImageOptions->setFocus();
    resize( 600, 400 );
    
    // About data and help button.
    
    KAboutData* about = new KAboutData("kipiplugins",
                                       I18N_NOOP("Acquire images"), 
                                       kipiplugins_version,
                                       I18N_NOOP("A Kipi plugin to acquire images"),
                                       KAboutData::License_GPL,
                                       "(c) 2003-2004, Gilles Caulier", 
                                       0,
                                       "http://extragear.kde.org/apps/kipi");
    
    about->addAuthor("Gilles Caulier", I18N_NOOP("Author and maintainer"),
                     "caulier dot gilles at free.fr");

    m_helpButton = actionButton( Help );
    KHelpMenu* helpMenu = new KHelpMenu(this, about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Acquire Images Handbook"), this, SLOT(slotHelp()), 0, -1, 0);
    m_helpButton->setPopup( helpMenu->menu() );
}


//////////////////////////////////// DESTRUCTOR /////////////////////////////////////////////

AcquireImageDialog::~AcquireImageDialog()
{
}


/////////////////////////////////////////////////////////////////////////////////////////////

void AcquireImageDialog::slotHelp()
{
    KApplication::kApplication()->invokeHelp("acquireimages",
                                             "kipi-plugins");
}    


/////////////////////////////////////////////////////////////////////////////////////////////

void AcquireImageDialog::readSettings(void)
{
    // Read all settings from configuration file.

    m_config = new KConfig("kipirc");
    m_config->setGroup("AcquireImages Settings");
    m_FileName->setText(m_config->readPathEntry("DefaultImageFileName", i18n("image")));
    m_imageCompression->setValue(m_config->readNumEntry("ImageCompression", 75));
    m_imagesFormat->setCurrentText(m_config->readEntry("ImageFormat", "TIFF"));

    delete m_config;
    
    // Get the image files filters from the hosts app.
     
    m_ImagesFilesSort = m_interface->fileExtensions();    
}


/////////////////////////////////////////////////////////////////////////////////////////////

void AcquireImageDialog::writeSettings(void)
{
    // Write all settings in configuration file.

    m_config = new KConfig("kipirc");
    m_config->setGroup("AcquireImages Settings");
    m_config->writePathEntry("DefaultImageFileName", m_FileName->text());
    m_config->writeEntry("ImageCompression", m_imageCompression->value());
    m_config->writeEntry("ImageFormat", m_imagesFormat->currentText());
    m_config->sync();
    delete m_config;
}


/////////////////////////////////////////////////////////////////////////////////////////////

void AcquireImageDialog::setupImageOptions(void)
{
    QString whatsThis;

    page_setupImageOptions = addPage( i18n("Target Image"),
                                      i18n("Target Image Options"),
                                      BarIcon("image", KIcon::SizeMedium ) );

    QVBoxLayout *vlay = new QVBoxLayout( page_setupImageOptions, 0, spacingHint() );

    //---------------------------------------------

    QGroupBox * groupBox1 = new QGroupBox( page_setupImageOptions );
    groupBox1->setFlat(false);
    groupBox1->setTitle(i18n("File Name && Comments"));
    QWhatsThis::add( groupBox1, i18n("<p>The target image preview with the file name and comments.") );
    QGridLayout* grid2 = new QGridLayout( groupBox1, 3, 3 , 20, 10);

    m_ImageFileName = new QLabel( i18n("File name (without suffix):"), groupBox1);
    grid2->addMultiCellWidget(m_ImageFileName, 0, 0, 0, 3);

    m_FileName = new QLineEdit(i18n("acquired_image"), groupBox1);
    QWhatsThis::add( m_FileName, i18n("<p>Enter here the target image file name without suffix "
                                      "(that will be automatically added to the file name according "
                                      "to the file-format option.)") );
    m_ImageFileName->setBuddy(m_FileName);
    grid2->addMultiCellWidget(m_FileName, 1, 1, 0, 3);

    m_ImageComments = new QLabel( i18n("Comments:"), groupBox1);
    grid2->addMultiCellWidget(m_ImageComments, 2, 2, 0, 3);

    m_CommentsEdit = new QTextEdit(groupBox1);
    m_CommentsEdit->setMaximumHeight( 200 );
    QWhatsThis::add( m_CommentsEdit, i18n("<p>Enter here the target image's comments.") );
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

    QGroupBox * groupBox2 = new QGroupBox( i18n("Saving Options"), page_setupImageOptions );
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
    whatsThis = i18n("<p>Select here the target image's file format.<p>");
    whatsThis = whatsThis + i18n("<b>JPEG</b>: The Joint Photographic Experts' Group "
                "file format is a good Web file format but it uses lossy data compression.<p>"
                "<b>PNG</b>: the Portable Network Graphics format is an extensible file format for "
                "the lossless, portable, well-compressed storage of raster images. PNG provides a "
                "patent-free replacement for GIF and can also replace many common uses of TIFF. PNG "
                "is designed to work well in online viewing applications, such as the World Wide Web, "
                "so it is fully streamable with a progressive display option. Also, PNG can store gamma "
                "and chromaticity data for improved color matching on heterogeneous platforms.");
    whatsThis = whatsThis + i18n("<p><b>TIFF</b>: the Tagged Image File Format is a rather old standard "
                "that is still very popular today. It is a highly flexible and platform-independent "
                "format which is supported by numerous image processing applications and by virtually all "
                "prepress software on the market.");
    whatsThis = whatsThis + i18n("<p><b>PPM</b>: the Portable Pixel Map file format is used as an "
                "intermediate format for storing color bitmap information. PPM files may be either "
                "binary or ASCII and store pixel values up to 24 bits in size. This format generates "
                "the biggest-sized text files for encoding images without losing quality.");
    whatsThis = whatsThis + i18n("<p><b>BMP</b>: the BitMaP file format is a popular image format from the "
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
                                    BarIcon("folder_image", KIcon::SizeMedium ) );

    QVBoxLayout *vlay = new QVBoxLayout( page_setupAlbumsList, 0, spacingHint() );

    //---------------------------------------------

    QVGroupBox * groupBox1 = new QVGroupBox( i18n("Select Folder in Which to Save Target Image"),
                                             page_setupAlbumsList );
    
    m_uploadPath = new KIPI::UploadWidget( m_interface, groupBox1, "m_uploadPath" );

    QWidget* w = new QWidget( groupBox1 );
    QHBoxLayout* hlay = new QHBoxLayout( w, 6 );
    hlay->addStretch( 1 );

    m_addNewAlbumButton = new QPushButton (i18n( "&Add New Folder"), w, "PushButton_AddNewAlbum");
    hlay->addWidget( m_addNewAlbumButton );
    QWhatsThis::add( m_addNewAlbumButton, i18n( "<p>Add a new folder."));
    
    vlay->addWidget( groupBox1 );

    //---------------------------------------------

    QGroupBox * groupBox2 = new QGroupBox( i18n("Album Description"), page_setupAlbumsList );
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
    
    if ( !m_interface->hasFeature( KIPI::AlbumsHaveComments) )
        groupBox2->hide();
    else
        vlay->addStretch(1);
            
    //---------------------------------------------

    connect(m_addNewAlbumButton, SIGNAL(clicked()),
            m_uploadPath, SLOT(mkdir()));

    connect(m_uploadPath, SIGNAL( folderItemSelected( const KURL & ) ),
            this, SLOT( slotAlbumSelected( const KURL & )));
    
    //---------------------------------------------
                    
    slotAlbumSelected( m_uploadPath->path() );           
 }


//////////////////////////////////////// SLOTS //////////////////////////////////////////////

void AcquireImageDialog::slotAlbumSelected( const KURL &url )
{
    QString comments, category, date, items;
    QValueList<KIPI::ImageCollection> albums = m_interface->allAlbums();
    QValueList<KIPI::ImageCollection>::Iterator albumIt;
    
    for( albumIt = albums.begin() ; albumIt != albums.end() ; ++albumIt )
       {
       if ( (*albumIt).path() == url ) 
          break;
       }
    
    if (albumIt != albums.end())
       { 
       comments = (*albumIt).comment();
       category = (*albumIt).category();
       date     = (*albumIt).date().toString( Qt::LocalDate );
       items.setNum((*albumIt).images().count());
       }
        
    m_AlbumComments->setText( i18n("Comment: %1").arg(comments) );
    m_AlbumCollection->setText( i18n("Collection: %1").arg(category) );
    m_AlbumDate->setText( i18n("Date: %1").arg(date) );
    m_AlbumItems->setText( i18n("Items: %1").arg( items ) );
}


/////////////////////////////////////////////////////////////////////////////////////////////

  void AcquireImageDialog::slotOk()
{
    // PENDING( aurelien)
    // It would be nice if m_uploadPath kept its value between multiple snapshots.
    KURL url = m_uploadPath->path();
    url.adjustPath(1);
    kdDebug(51001) << k_funcinfo << "path:" << url.prettyURL() << endl;
    
    if (!url.isValid())
    {
        KMessageBox::error(this, i18n("You must select a target album for this image."));
        return;
    }

    if (m_FileName->text().isEmpty())
    {
        KMessageBox::error(this, i18n("You must provide a file name for this image."));
        return;
    }

    writeSettings();

    // Get all scanned image informations.
    QString imageFormat = m_imagesFormat->currentText();
    int imageCompression = m_imageCompression->value();
    QString Commentsimg = m_CommentsEdit->text();

    // Find an unique url
    QString fileName = m_FileName->text();
    QString ext = extension(imageFormat);
    url.setFileName(fileName + ext);

    if (KIO::NetAccess::exists(url, false NETACCESS_WIDGET)) 
        {
        for (int idx = 1; idx < 100 ; ++idx) 
            {
            url.setFileName(QString("%1_%2%3").arg(fileName).arg(idx).arg(ext));
            kdDebug(51001) << "File already exist. Try to fixed target Url to: " << url.prettyURL() << endl;
            
            if (!KIO::NetAccess::exists(url, false NETACCESS_WIDGET)) 
               break;
            }
        }

    kdDebug(51001) << k_funcinfo << "Saving image as " << url.prettyURL() << endl;

    // Save file
    KTempFile tmp;
    tmp.setAutoDelete(true);
    QString imagePath;
    if (url.isLocalFile()) {
        imagePath=url.path();
    } else {
        imagePath=tmp.name();
    }

    bool ok=false;
    if (imageFormat=="JPEG" || imageFormat=="PNG") {
        ok = m_qimageScanned.save(imagePath, imageFormat.latin1(), imageCompression);
    } else if (imageFormat=="TIFF") {
        ok = QImageToTiff(m_qimageScanned, imagePath);
    } else {
        ok =  m_qimageScanned.save(imagePath, imageFormat.latin1());
    }

    if ( !ok )
    {
       KMessageBox::error(this, i18n("Cannot write image file \"%1\".").arg(imagePath));
       return;
    }

    // Upload the image if necessary
    if ( !url.isLocalFile()) {
        if (!KIO::NetAccess::upload(imagePath, url NETACCESS_WIDGET)) {
           KMessageBox::error(this, i18n("Could not upload image to \"%1\".").arg(url.prettyURL()));
           return;
        }
    }

    // Save the comments for this image.
    QString err;
    ok = m_interface->addImage( url, err );
    if ( !ok ) {
        KMessageBox::error(this, i18n("<qt>Error when informing the application about the new image. "
                                  "The error was: %1</qt>" ).arg( err ) );
        return;
    }

    KIPI::ImageInfo info = m_interface->info( url );
    info.setDescription( Commentsimg );

    m_interface->refreshImages( KURL::List(url) );
    
    close();
    delete this;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void AcquireImageDialog::slotImageFormatChanged(const QString &string)
{
    if ( string == "JPEG" || string == "PNG" )
       m_imageCompression->setEnabled(true);
    else
       m_imageCompression->setEnabled(false);
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

}  // NameSpace KIPIAcquireImagesPlugin

