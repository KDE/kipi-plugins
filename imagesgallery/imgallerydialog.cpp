//////////////////////////////////////////////////////////////////////////////
//
//    IMGGALLERYDIALOG.CPP
//
//    Copyright (C) 2003-2004 Gilles Caulier <caulier dot gilles at free.fr>
//    Copyright (C) 2003-2004 by Gregory Kokanosky <gregory dot kokanosky at free.fr>
//    for images navigation mode.
//
//    Adapted and improved for DigikamPlugins from the konqueror plugin
//    'kdeaddons/konq-plugins/kimgalleryplugin/' by Gilles Caulier.
//
//    Copyright 2001, 2003 by Lukas Tinkl <lukas at kde.org> and
//    Andreas Schlapbach <schlpbch at iam.unibe.ch> for orginal source
//    of 'kimgalleryplugin' from KDE CVS
//
//    Copyright 2003-2004 by Gilles Caulier <caulier dot gilles at free.fr> for
//    DigikamPlugins port.
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
//    Foundation, Inc., 51 Franklin Street, Fifth Floor, Cambridge, MA 02110-1301, USA.
//
//////////////////////////////////////////////////////////////////////////////


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
#include <qprogressdialog.h>
#include <qdir.h>
#include <qpushbutton.h>

// Include files for KDE

#include <klocale.h>
#include <kconfig.h>
#include <kurlrequester.h>
#include <kurl.h>
#include <kfontdialog.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <klineedit.h>
#include <knuminput.h>
#include <kcolorbutton.h>
#include <kglobalsettings.h>
#include <kmessagebox.h>
#include <kbuttonbox.h>
#include <kapplication.h>
#include <ksqueezedtextlabel.h>
#include <kio/previewjob.h>
#include <klistview.h>
#include <khelpmenu.h>
#include <kpopupmenu.h>

// KIPI include files

#include <libkipi/imagecollectionselector.h>

// Local include files

#include "kpaboutdata.h"
#include "pluginsversion.h"
#include "imgallerydialog.h"
#include "imgallerydialog.moc"


namespace KIPIImagesGalleryPlugin
{

KIGPDialog::KIGPDialog(KIPI::Interface* interface, QWidget *parent)
          : KDialogBase( IconList, i18n("Configure"), Help|Ok|Cancel, Ok,
                         parent, "HTMLExportDialog", true, true ), m_interface( interface )
{
    setCaption(i18n("Create Image Galleries"));
    setupSelection();
    setupLookPage();
    setupAlbumPage();
    setupThumbnailPage();
    page_setupSelection->setFocus();
    resize( 650, 450 );

    // About data and help button.

    m_about = new KIPIPlugins::KPAboutData(I18N_NOOP("Image Gallery"),
                                           kipiplugins_version,
                                           KAboutData::License_GPL,
                                           I18N_NOOP("A Kipi plugin for HTML album export.\n"
                                                     "Based on KimgalleryPlugin implementation."),
                                           "(c) 2003-2004, Gilles Caulier");

    m_about->addAuthor("Gilles Caulier", I18N_NOOP("Author and maintainer"),
                       "caulier dot gilles at free.fr");

    m_about->addAuthor("Gregory Kokanosky", I18N_NOOP("Image navigation mode patches"),
                       "gregory dot kokanosky at free.fr>");

    m_about->addAuthor("Achim Bohnet", I18N_NOOP("HTML implementation patches"),
                       "ach at mpe.mpg.de");

    m_about->addAuthor("Lukáš Tinkl", I18N_NOOP("Original HTML generator implementation"),
                       "lukas at kde.org");

    m_about->addAuthor("Andreas Schlapbach", I18N_NOOP("Original HTML generator implementation"),
                       "schlpbch at iam.unibe.ch");

    m_helpButton = actionButton( Help );
    KHelpMenu* helpMenu = new KHelpMenu(this, m_about, false);
    helpMenu->menu()->removeItemAt(0);
    helpMenu->menu()->insertItem(i18n("Plugin Handbook"), this, SLOT(slotHelp()), 0, -1, 0);
    m_helpButton->setPopup( helpMenu->menu() );
}


/////////////////////////////////////////////////////////////////////////////////////////////

KIGPDialog::~KIGPDialog()
{
    delete m_about;
}

void KIGPDialog::slotHelp()
{
    KApplication::kApplication()->invokeHelp("imagesgallery",
                                             "kipi-plugins");
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setupSelection(void)
{
    page_setupSelection = addPage(i18n("Selection"), i18n("Album Selection"),
                                  BarIcon("folder_image", KIcon::SizeMedium));

    QVBoxLayout *layout = new QVBoxLayout(page_setupSelection, 0, spacingHint() );
    m_imageCollectionSelector = new KIPI::ImageCollectionSelector(page_setupSelection, m_interface);
    layout->addWidget(m_imageCollectionSelector);
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setupLookPage(void)
{
    page_setupLook = addPage( i18n("Look"), i18n("Page Look"),
                              BarIcon("html", KIcon::SizeMedium ) );

    QVBoxLayout *vlay = new QVBoxLayout( page_setupLook, 0, spacingHint() );

    //---------------------------------------------

    QLabel *label, *labelTitle;
    labelTitle = new QLabel( i18n("Main &page title:"), page_setupLook);
    vlay->addWidget(labelTitle);

    m_title = new QLineEdit(i18n("Album Image Galleries"), page_setupLook);
    vlay->addWidget( m_title );
    labelTitle->setBuddy(m_title);
    QWhatsThis::add( m_title, i18n("<p>Enter here the title of the main HTML page "
                                   "(multiple Album selection only).") );

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

    m_imageName = new QCheckBox( i18n("Show image file &name"), page_setupLook);
    QWhatsThis::add( m_imageName, i18n("<p>If you enable this option, "
                                       "the image filenames will be added to the Album page.") );
    m_imageName->setChecked( true );
    grid->addWidget( m_imageName, 0, 0 );

    //---------------------------------------------

    m_imageSize = new QCheckBox( i18n("Show image file &size"), page_setupLook);
    QWhatsThis::add( m_imageSize, i18n("<p>If you enable this option, "
                                       "the image file sizes will be added to the Album page.") );
    m_imageSize->setChecked( true );
    grid->addWidget( m_imageSize, 0, 1 );

    //---------------------------------------------

    m_imageProperty = new QCheckBox( i18n("Show image &dimensions"), page_setupLook);
    QWhatsThis::add( m_imageProperty, i18n("<p>If you enable this option, "
                                           "the image dimensions will be added to the Album page.") );
    m_imageProperty->setChecked( true );
    grid->addWidget( m_imageProperty, 1, 0 );

    //---------------------------------------------

    m_PageCreationDate = new QCheckBox( i18n("Show page creation date"), page_setupLook);
    QWhatsThis::add( m_PageCreationDate, i18n("<p>If you enable this option, "
                                              "the creation date will be added to the Album page.") );
    m_PageCreationDate->setChecked( true );
    grid->addWidget( m_PageCreationDate, 1, 1 );

    //---------------------------------------------

    m_createPageForPhotos = new QCheckBox(  i18n("Create pages for each image"), page_setupLook);
    QWhatsThis::add( m_createPageForPhotos, i18n("<p>If you enable this option, "
                                                 "a HTML page will be added for each photo.") );
    m_createPageForPhotos->setChecked( true );
    grid->addWidget( m_createPageForPhotos, 2, 0 );

    //---------------------------------------------

    QHBoxLayout *hlay10  = new QHBoxLayout( );
    vlay->addLayout( hlay10 );

    m_openInWebBrowser = new QCheckBox( i18n("Open image gallery in:"), page_setupLook);
    QWhatsThis::add( m_openInWebBrowser, i18n("<p>Select here the application to preview or "
                                              "edit the HTML pages.") );
    m_openInWebBrowser->setChecked( true );

    m_webBrowserName = new QComboBox( false, page_setupLook );
    m_webBrowserName->insertItem( "Amaya" );
    m_webBrowserName->insertItem( "Firefox" );
    m_webBrowserName->insertItem( "Galeon" );
    m_webBrowserName->insertItem( "Konqueror" );
    m_webBrowserName->insertItem( "Mozilla" );
    m_webBrowserName->insertItem( "Netscape" );
    m_webBrowserName->insertItem( "Opera" );
    m_webBrowserName->insertItem( "Quanta" );
    m_webBrowserName->insertItem( "Screem" );
    m_webBrowserName->setCurrentText( "Konqueror" );

    hlay10->addWidget( m_openInWebBrowser );
    hlay10->addStretch( 1 );
    hlay10->addWidget( m_webBrowserName );

    //---------------------------------------------

    QHBoxLayout *hlay11  = new QHBoxLayout( );
    vlay->addLayout( hlay11 );

    m_fontName = new QComboBox( false, page_setupLook );
    QWhatsThis::add( m_fontName, i18n("<p>Select here the font name to use for the pages.") );
    QStringList standardFonts;
    KFontChooser::getFontList(standardFonts, 0);
    m_fontName->insertStringList( standardFonts );
    m_fontName->setCurrentText( KGlobalSettings::generalFont().family());

    label = new QLabel( i18n("Fon&t name:"), page_setupLook );
    label->setBuddy( m_fontName );
    hlay11->addWidget( label );
    hlay11->addStretch( 1 );
    hlay11->addWidget( m_fontName );

    //---------------------------------------------

    QHBoxLayout *hlay12  = new QHBoxLayout( );
    vlay->addLayout( hlay12 );

    m_fontSize = new QSpinBox( 6, 15, 1, page_setupLook );
    m_fontSize->setValue( 14 );
    QWhatsThis::add( m_fontSize, i18n("<p>Select here the font size to use for the pages.") );

    label = new QLabel( i18n("Font si&ze:"), page_setupLook );
    label->setBuddy( m_fontSize );
    hlay12->addWidget( label );
    hlay12->addStretch( 1 );
    hlay12->addWidget( m_fontSize );

    //---------------------------------------------

    QHBoxLayout *hlay1  = new QHBoxLayout( spacingHint() );
    vlay->addLayout( hlay1 );

    m_foregroundColor = new KColorButton(page_setupLook);
    m_foregroundColor->setColor(QColor("#d0ffd0"));
    QWhatsThis::add( m_foregroundColor, i18n("<p>Select here the foreground color to use for the pages.") );

    label = new QLabel( i18n("&Foreground color:"), page_setupLook);
    label->setBuddy( m_foregroundColor );
    hlay1->addWidget( label );
    hlay1->addStretch( 1 );
    hlay1->addWidget(m_foregroundColor);

    //---------------------------------------------

    QHBoxLayout *hlay2 = new QHBoxLayout( spacingHint() );
    vlay->addLayout( hlay2 );

    m_backgroundColor = new KColorButton(page_setupLook);
    m_backgroundColor->setColor(QColor("#333333"));
    QWhatsThis::add( m_backgroundColor, i18n("<p>Select here the background color to use for the pages.") );

    label = new QLabel( i18n("&Background color:"), page_setupLook);
    hlay2->addWidget( label );
    label->setBuddy( m_backgroundColor );
    hlay2->addStretch( 1 );
    hlay2->addWidget(m_backgroundColor);

    //---------------------------------------------

    QHBoxLayout *hlay13  = new QHBoxLayout( );
    vlay->addLayout( hlay13 );

    m_bordersImagesSize = new QSpinBox( 1, 20, 1, page_setupLook );
    m_bordersImagesSize->setValue( 1 );
    QWhatsThis::add( m_bordersImagesSize, i18n("<p>Select here the image borders' size in pixels.") );

    label = new QLabel( i18n("Images borders s&ize:"), page_setupLook );
    label->setBuddy( m_bordersImagesSize );
    hlay13->addWidget( label );
    hlay13->addStretch( 1 );
    hlay13->addWidget( m_bordersImagesSize );

    //---------------------------------------------

    QHBoxLayout *hlay3 = new QHBoxLayout( spacingHint() );
    vlay->addLayout( hlay3 );

    m_bordersImagesColor = new KColorButton(page_setupLook);
    m_bordersImagesColor->setColor(QColor("#d0ffd0"));
    QWhatsThis::add( m_bordersImagesColor, i18n("<p>Select here the color to use "
                                                "for the image borders.") );

    label = new QLabel( i18n("Image bo&rder color:"), page_setupLook);
    hlay3->addWidget( label );
    label->setBuddy( m_bordersImagesColor );
    hlay3->addStretch( 1 );
    hlay3->addWidget(m_bordersImagesColor);

    //---------------------------------------------

    vlay->addStretch(1);

    connect(m_openInWebBrowser, SIGNAL(toggled(bool)),
            m_webBrowserName, SLOT(setEnabled(bool)));
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setupAlbumPage(void)
{
    QString whatsThis;
    page_setupAlbum = addPage(i18n("Album"), i18n("Photo Album"),
                              BarIcon("image.png", KIcon::SizeMedium));

    QVBoxLayout *dvlay = new QVBoxLayout( page_setupAlbum, 0, spacingHint() );
    QLabel *label;

    //---------------------------------------------

    label = new QLabel(i18n("&Save gallery to:"), page_setupAlbum);
    dvlay->addWidget( label );

    m_imageNameReq = new KURLRequester( KGlobalSettings::documentPath(), page_setupAlbum);
    m_imageNameReq->setMode(KFile::Directory | KFile::ExistingOnly | KFile::LocalOnly);
    label->setBuddy( m_imageNameReq );
    dvlay->addWidget( m_imageNameReq );
    QWhatsThis::add( m_imageNameReq, i18n("<p>The folder name where the galleries will be saved.") );

    //---------------------------------------------

    m_useNotOriginalImageSize = new QCheckBox(i18n("Resize target images"), page_setupAlbum);
    m_useNotOriginalImageSize->setChecked(true);
    dvlay->addWidget(m_useNotOriginalImageSize);
    QWhatsThis::add( m_useNotOriginalImageSize, i18n("<p>If you enable this option, "
                                                     "all target images can be resized.") );

    m_imagesresize = new KIntNumInput(640, page_setupAlbum);
    m_imagesresize->setRange(200, 2000, 1, true );
    m_imagesresize->setLabel( i18n("Target images size:") );
    dvlay->addWidget( m_imagesresize );
    QWhatsThis::add( m_imagesresize, i18n("<p>The new size of the target images in pixels") );

    //---------------------------------------------

    m_useSpecificTargetimageCompression = new QCheckBox(i18n("Use a specific target image compression"),
                                                        page_setupAlbum);
    m_useSpecificTargetimageCompression->setChecked(true);
    dvlay->addWidget(m_useSpecificTargetimageCompression);
    QWhatsThis::add( m_useSpecificTargetimageCompression, i18n("<p>If you enable this option, "
                     "all target images can be compressed with a specific compression value.") );

    m_Targetimagescompression = new KIntNumInput(75, page_setupAlbum);
    m_Targetimagescompression->setRange(1, 100, 1, true );
    m_Targetimagescompression->setLabel( i18n("Target images compression:") );
    dvlay->addWidget( m_Targetimagescompression );
    whatsThis = i18n("<p>The compression value of the target images :<p>");
    whatsThis = whatsThis + i18n("<b>1</b>: very high compression<p>"
                                 "<b>25</b>: high compression<p>"
                                 "<b>50</b>: medium compression<p>"
                                 "<b>75</b>: low compression (default value)<p>"
                                 "<b>100</b>: no compression");

    QWhatsThis::add( m_Targetimagescompression, whatsThis);

    //---------------------------------------------

    QHBoxLayout *hlay12  = new QHBoxLayout( );
    dvlay->addLayout( hlay12 );

    m_targetimagesFormat = new QComboBox(false, page_setupAlbum);
    m_targetimagesFormat->insertItem("JPEG");
    m_targetimagesFormat->insertItem("PNG");
    m_targetimagesFormat->setCurrentText ("JPEG");
    whatsThis = i18n("<p>Select here the image file format for the target images.<p>");
    whatsThis = whatsThis + i18n("<b>JPEG</b>: The Joint Photographic Experts Group's file "
                "format is a good Web file format but uses lossy data compression.<p>"
                "<b>PNG</b>: the Portable Network Graphics format is an extensible file format "
                "for the lossless, portable, well-compressed storage of raster images. PNG provides "
                "a patent-free replacement for GIF and can also replace many common uses of TIFF. "
                "PNG is designed to work well in online viewing applications, such as the World Wide Web, "
                "so it is fully streamable with a progressive display option. Also, PNG can store gamma "
                "and chromaticity data for improved color matching on heterogeneous platforms.");
    QWhatsThis::add( m_targetimagesFormat, whatsThis );

    label = new QLabel( i18n("Target images file format:"), page_setupAlbum);
    hlay12->addWidget( label );
    label->setBuddy( m_targetimagesFormat );
    hlay12->addStretch( 1 );
    hlay12->addWidget(m_targetimagesFormat);

    //---------------------------------------------

    QHBoxLayout *hlay13 = new QHBoxLayout( spacingHint() );
    dvlay->addLayout( hlay13 );
    m_colorDepthSetTargetImages = new QCheckBox(i18n("&Set different color depth:"), page_setupAlbum);
    m_colorDepthSetTargetImages->setChecked(false);
    QWhatsThis::add( m_colorDepthSetTargetImages, i18n("<p>Select here the color depth used for "
                                                       "the target image rendering.") );
    hlay13->addWidget( m_colorDepthSetTargetImages );

    m_colorDepthTargetImages = new QComboBox(false, page_setupAlbum);
    m_colorDepthTargetImages->insertItem("1");
    m_colorDepthTargetImages->insertItem("8");
    m_colorDepthTargetImages->insertItem("32");
    m_colorDepthTargetImages->setCurrentText("32");
    m_colorDepthTargetImages->setEnabled(false);
    hlay13->addWidget( m_colorDepthTargetImages );

    //---------------------------------------------

    m_useCommentFile = new QCheckBox(i18n("Use images album &captions"), page_setupAlbum);
    m_useCommentFile->setChecked(true);
    dvlay->addWidget(m_useCommentFile);

    QWhatsThis::add( m_useCommentFile, i18n("<p>If you enable this option, "
                                            "the images Album captions will be used for generating "
                                            "subtitles for the images.") );

    //---------------------------------------------

    m_useCommentsAlbum = new QCheckBox(i18n("Show album cap&tion"), page_setupAlbum);
    m_useCommentsAlbum->setChecked(true);
    dvlay->addWidget(m_useCommentsAlbum);
    QWhatsThis::add( m_useCommentsAlbum, i18n("<p>If you enable this option, "
                                              "the Album caption will be shown in the gallery.") );

    //---------------------------------------------

    m_useCollectionAlbum = new QCheckBox(i18n("Show album &collection"), page_setupAlbum);
    m_useCollectionAlbum->setChecked(false);
    dvlay->addWidget(m_useCollectionAlbum);
    QWhatsThis::add( m_useCollectionAlbum, i18n("<p>If you enable this option, "
                                                "the Album collection will be shown in the gallery.") );

    //---------------------------------------------

    m_useDateAlbum = new QCheckBox(i18n("Show album creation &date"), page_setupAlbum);
    m_useDateAlbum->setChecked(false);
    dvlay->addWidget(m_useDateAlbum);
    QWhatsThis::add( m_useDateAlbum, i18n("<p>If you enable this option, "
                                          "the Album creation date will be shown in the gallery.") );

    //---------------------------------------------

    m_useNbImagesAlbum = new QCheckBox(i18n("Show &number of images in album"), page_setupAlbum);
    m_useNbImagesAlbum->setChecked(true);
    dvlay->addWidget(m_useNbImagesAlbum);
    QWhatsThis::add( m_useNbImagesAlbum, i18n("<p>If you enable this option, "
                                              "the number of images in Album will be shown in the gallery.") );
    dvlay->addStretch(1);

    //---------------------------------------------

    connect( m_imageNameReq, SIGNAL(textChanged(const QString&)),
             this, SLOT(GalleryUrlChanged(const QString&)));

    connect(m_useNotOriginalImageSize, SIGNAL(toggled(bool)),
            m_imagesresize, SLOT(setEnabled(bool)));

    connect(m_useSpecificTargetimageCompression, SIGNAL(toggled(bool)),
            m_Targetimagescompression, SLOT(setEnabled(bool)));

    connect(m_colorDepthSetTargetImages, SIGNAL( toggled(bool) ),
            m_colorDepthTargetImages, SLOT( setEnabled(bool) ) );
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setupThumbnailPage(void)
{
    QString whatsThis;
    page_setupThumbnail = addPage(i18n("Thumbnails"), i18n("Image Thumbnails"),
                                  BarIcon("thumbnail", KIcon::SizeMedium));

    QLabel *label;
    QVBoxLayout *vlay = new QVBoxLayout( page_setupThumbnail, 0, spacingHint() );
    QHBoxLayout *hlay3 = new QHBoxLayout( spacingHint() );

    //---------------------------------------------

    m_thumbnailsSize = new KIntNumInput(140, page_setupThumbnail);
    m_thumbnailsSize->setRange(10, 1000, 1, true );
    m_thumbnailsSize->setLabel( i18n("Thumbnails size:") );
    vlay->addWidget( m_thumbnailsSize );
    QWhatsThis::add( m_thumbnailsSize, i18n("<p>The new size of the thumbnails in pixels") );

    //---------------------------------------------

    m_useSpecificThumbsCompression = new QCheckBox(i18n("Use a specific thumbnail compression"),
                                                   page_setupThumbnail);
    m_useSpecificThumbsCompression->setChecked(true);
    vlay->addWidget(m_useSpecificThumbsCompression);
    QWhatsThis::add( m_useSpecificThumbsCompression, i18n("<p>If you enable this option, "
                     "all thumbnails can be compressed with a specific compression value.") );

    m_thumbscompression = new KIntNumInput(75, page_setupThumbnail);
    m_thumbscompression->setRange(1, 100, 1, true );
    m_thumbscompression->setLabel( i18n("Thumbnails compression:") );
    vlay->addWidget( m_thumbscompression );
    whatsThis = i18n("<p>The compression value of thumbnails :<p>");
    whatsThis = whatsThis + i18n("<b>1</b>: very high compression<p>"
                                 "<b>25</b>: high compression<p>"
                                 "<b>50</b>: medium compression<p>"
                                 "<b>75</b>: low compression (default value)<p>"
                                 "<b>100</b>: no compression");

    QWhatsThis::add( m_thumbscompression, whatsThis);

    //---------------------------------------------

    vlay->addLayout( hlay3 );
    m_imageFormat = new QComboBox(false, page_setupThumbnail);
    m_imageFormat->insertItem("JPEG");
    m_imageFormat->insertItem("PNG");
    m_imageFormat->setCurrentText ("JPEG");
    whatsThis = i18n("<p>Select here the image file format for thumbnails.<p>");
    whatsThis = whatsThis + i18n("<b>JPEG</b>: The Joint Photographic Experts Group's file format "
                "is a good Web file format but uses lossy data compression.<p>"
                "<b>PNG</b>: the Portable Network Graphics format is an extensible file format for "
                "the lossless, portable, well-compressed storage of raster images. PNG provides a "
                "patent-free replacement for GIF and can also replace many common uses of TIFF. "
                "PNG is designed to work well in online viewing applications, such as the World Wide Web, "
                "so it is fully streamable with a progressive display option. Also, PNG can store gamma "
                "and chromaticity data for improved color matching on heterogeneous platforms.");
    QWhatsThis::add( m_imageFormat, whatsThis );

    label = new QLabel( i18n("Thumbnails file format:"), page_setupThumbnail);
    hlay3->addWidget( label );
    label->setBuddy( m_imageFormat );
    hlay3->addStretch( 1 );
    hlay3->addWidget(m_imageFormat);

    //---------------------------------------------

    QGridLayout *grid = new QGridLayout( 2, 2 );
    vlay->addLayout( grid );
    QHBoxLayout *hlay4 = new QHBoxLayout( spacingHint() );
    vlay->addLayout( hlay4 );
    m_colorDepthSetThumbnails = new QCheckBox(i18n("&Set different color depth:"), page_setupThumbnail);
    m_colorDepthSetThumbnails->setChecked(false);
    QWhatsThis::add( m_colorDepthSetThumbnails, i18n("<p>Select here the color depth used for "
                                                     "the thumbnail image rendering.") );
    hlay4->addWidget( m_colorDepthSetThumbnails );

    m_colorDepthThumbnails = new QComboBox(false, page_setupThumbnail);
    m_colorDepthThumbnails->insertItem("1");
    m_colorDepthThumbnails->insertItem("8");
    m_colorDepthThumbnails->insertItem("32");
    m_colorDepthThumbnails->setCurrentText("32");
    m_colorDepthThumbnails->setEnabled(false);
    hlay4->addWidget( m_colorDepthThumbnails );

    vlay->addStretch(1);

    //---------------------------------------------

    connect(m_useSpecificThumbsCompression, SIGNAL(toggled(bool)),
            m_thumbscompression, SLOT(setEnabled(bool)));

    connect(m_colorDepthSetThumbnails, SIGNAL( toggled(bool) ),
            m_colorDepthThumbnails, SLOT( setEnabled(bool) ) );

    //---------------------------------------------
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::GalleryUrlChanged(const QString &url )
{
    enableButtonOK( !url.isEmpty());
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::slotOk()
{
    m_selectedAlbums = m_imageCollectionSelector->selectedImageCollections();

    if (m_selectedAlbums.size() == 0)
       {
       KMessageBox::sorry(this, i18n("You must select at least one album."));
       return;
       }

    QDir dirGallery(getImageName());

    if (dirGallery.exists() == false)
       {
       KMessageBox::sorry(this, i18n("Image gallery folder does not exist; please check it..."));
       return;
       }

    accept();
}


/////////////////////////////////////////////////////////////////////////////////////////////

bool KIGPDialog::printImageName()  const
{
    return m_imageName->isChecked();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setPrintImageName(bool Value)
{
    m_imageName->setChecked(Value);
}


/////////////////////////////////////////////////////////////////////////////////////////////

bool KIGPDialog::printImageSize() const
{
    return m_imageSize->isChecked();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setPrintImageSize(bool Value)
{
    m_imageSize->setChecked(Value);
}


/////////////////////////////////////////////////////////////////////////////////////////////

bool KIGPDialog::printImageProperty() const
{
    return m_imageProperty->isChecked();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setPrintImageProperty(bool Value)
{
    m_imageProperty->setChecked(Value);
}


/////////////////////////////////////////////////////////////////////////////////////////////

bool KIGPDialog::OpenGalleryInWebBrowser() const
{
    return m_openInWebBrowser->isChecked();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setOpenGalleryInWebBrowser(bool Value)
{
    m_openInWebBrowser->setChecked(Value);
}


////////////////////////////////////////////////////////////////////////////////////////////


bool KIGPDialog::getCreatePageForPhotos() const
{
    return m_createPageForPhotos->isChecked();
}


void KIGPDialog::setCreatePageForPhotos(bool Value)
{
    m_createPageForPhotos->setChecked(Value);
}


/////////////////////////////////////////////////////////////////////////////////////////////

bool KIGPDialog::useCommentFile() const
{
    return m_useCommentFile->isChecked();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setUseCommentFile(bool Value)
{
    m_useCommentFile->setChecked( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

bool KIGPDialog::useCommentsAlbum() const
{
    return m_useCommentsAlbum->isChecked();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setUseCommentsAlbum(bool Value)
{
    m_useCommentsAlbum->setChecked( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

bool KIGPDialog::useCollectionAlbum() const
{
    return m_useCollectionAlbum->isChecked();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setUseCollectionAlbum(bool Value)
{
    m_useCollectionAlbum->setChecked( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

bool KIGPDialog::useDateAlbum() const
{
    return m_useDateAlbum->isChecked();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setUseDateAlbum(bool Value)
{
    m_useDateAlbum->setChecked( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

bool KIGPDialog::useNbImagesAlbum() const
{
    return m_useNbImagesAlbum->isChecked();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setUseNbImagesAlbum(bool Value)
{
    m_useNbImagesAlbum->setChecked( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

int KIGPDialog::getImagesPerRow() const
{
    return m_imagesPerRow->value();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setImagesPerRow(int Value)
{
    m_imagesPerRow->setValue(Value);
}


/////////////////////////////////////////////////////////////////////////////////////////////

int KIGPDialog::getThumbnailsSize() const
{
    return m_thumbnailsSize->value();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setThumbnailsSize(int Value)
{
    m_thumbnailsSize->setValue( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

bool KIGPDialog::colorDepthSetThumbnails() const
{
    return m_colorDepthSetThumbnails->isChecked();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setColorDepthSetThumbnails(bool Value)
{
    m_colorDepthSetThumbnails->setChecked( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setColorDepthThumbnails(QString Value)
{
    m_colorDepthThumbnails->setCurrentText( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

int KIGPDialog::getColorDepthThumbnails() const
{
    return m_colorDepthThumbnails->currentText().toInt();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setColorDepthTargetImages(QString Value)
{
    m_colorDepthTargetImages->setCurrentText( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

int KIGPDialog::getColorDepthTargetImages() const
{
    return m_colorDepthTargetImages->currentText().toInt();
}


/////////////////////////////////////////////////////////////////////////////////////////////

bool KIGPDialog::colorDepthSetTargetImages() const
{
    return m_colorDepthSetTargetImages->isChecked();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setColorDepthSetTargetImages(bool Value)
{
    m_colorDepthSetTargetImages->setChecked( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

const QString KIGPDialog::getImageName() const
{
    return m_imageNameReq->url();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setImageName(QString Value)
{
    m_imageNameReq->setURL( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

const QString KIGPDialog::getFontName() const
{
    return m_fontName->currentText();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setFontName(QString Value)
{
    m_fontName->setCurrentText (Value);
}


/////////////////////////////////////////////////////////////////////////////////////////////

const QString KIGPDialog::getFontSize() const
{
    return m_fontSize->text();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setFontSize(int Value)
{
    m_fontSize->setValue( Value );
}



/////////////////////////////////////////////////////////////////////////////////////////////

const QString KIGPDialog::getBordersImagesSize() const
{
    return m_bordersImagesSize->text();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setBordersImagesSize(int Value)
{
    m_bordersImagesSize->setValue( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

const QColor KIGPDialog::getBackgroundColor() const
{
    return m_backgroundColor->color();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setBackgroundColor(QColor Value)
{
    m_backgroundColor->setColor( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

const QColor KIGPDialog::getForegroundColor() const
{
    return m_foregroundColor->color();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setForegroundColor(QColor Value)
{
    m_foregroundColor->setColor( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

const QColor KIGPDialog::getBordersImagesColor() const
{
    return m_bordersImagesColor->color();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setBordersImagesColor(QColor Value)
{
    m_bordersImagesColor->setColor( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

const QString KIGPDialog::getImageFormat() const
{
    return m_imageFormat->currentText();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setImageFormat(QString Value)
{
    return m_imageFormat->setCurrentText( Value );
}

/////////////////////////////////////////////////////////////////////////////////////////////

int KIGPDialog::getImagesResize() const
{
    return m_imagesresize->value();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setImagesResizeFormat(int Value)
{
    m_imagesresize->setValue( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

const QString KIGPDialog::getTargetImagesFormat() const
{
    return m_targetimagesFormat->currentText();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setTargetImagesFormat(QString Value)
{
    return m_targetimagesFormat->setCurrentText( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

const QString KIGPDialog::getMainTitle() const
{
    return m_title->text();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setMainTitle(QString Value)
{
    return m_title->setText( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

bool KIGPDialog::useNotOriginalImageSize() const
{
    return m_useNotOriginalImageSize->isChecked();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setNotUseOriginalImageSize(bool Value)
{
    m_useNotOriginalImageSize->setChecked( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

bool KIGPDialog::useSpecificTargetimageCompression() const
{
    return m_useSpecificTargetimageCompression->isChecked();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setUseSpecificTargetimageCompression(bool Value)
{
    m_useSpecificTargetimageCompression->setChecked( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

int KIGPDialog::getTargetImagesCompression() const
{
    return m_Targetimagescompression->value();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setTargetImagesCompression(int Value)
{
    m_Targetimagescompression->setValue( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

bool KIGPDialog::useSpecificThumbsCompression() const
{
    return m_useSpecificThumbsCompression->isChecked();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setUseSpecificThumbsCompression(bool Value)
{
    m_useSpecificThumbsCompression->setChecked( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

int KIGPDialog::getThumbsCompression() const
{
    return m_thumbscompression->value();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setThumbsCompression(int Value)
{
    m_thumbscompression->setValue( Value );
}


/////////////////////////////////////////////////////////////////////////////////////////////

bool  KIGPDialog::printPageCreationDate() const
{
    return m_PageCreationDate->isChecked();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void  KIGPDialog::setPrintPageCreationDate(bool Value)
{
    m_PageCreationDate->setChecked(Value);
}


/////////////////////////////////////////////////////////////////////////////////////////////

const QString KIGPDialog::getWebBrowserName() const
{
    return m_webBrowserName->currentText();
}


/////////////////////////////////////////////////////////////////////////////////////////////

void KIGPDialog::setWebBrowserName(QString Value)
{
    return m_webBrowserName->setCurrentText( Value );
}

}  // NameSpace KIPIImagesGalleryPlugin

