/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-01-11
 * Description : a kipi plugin to print images
 *
 * Copyright 2008 by Angelo Naselli <anaselli at linux dot it>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "wizard.moc"

// STD

#include <memory>

// Qt includes

#include <QFileInfo>
#include <QPainter>
#include <QPalette>
#include <QtGlobal>
#include <QPrinter>
#include <QPrintDialog>

// KDE includes

#include <kapplication.h>
#include <kconfigdialogmanager.h>
#include <khelpmenu.h>
#include <kmenu.h>
#include <kpushbutton.h>
#include <ktoolinvocation.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kdeprintdialog.h>
#include <kdebug.h>

// libkipi includes

#include <libkipi/imagecollectionselector.h>
#include <libkipi/interface.h>

// libkexiv2 includes

#include <libkexiv2/kexiv2.h>

// Local includes

#include "kpaboutdata.h"
#include "ui_croppage.h"
#include "ui_infopage.h"
#include "ui_intropage.h"
#include "ui_photopage.h"
#include "tphoto.h"
#include "utils.h"

namespace KIPIPrintImagesPlugin
{

  template <class Ui_Class>

  class WizardPage : public QWidget, public Ui_Class
  {
    public:

      WizardPage ( KAssistantDialog* dialog, const QString& title )
          : QWidget ( dialog )
      {
        setupUi ( this );
        layout()->setMargin ( 0 );
        mPage = dialog->addPage ( this, title );
      }

      KPageWidgetItem* page() const
      {
        return mPage;
      }

    private:
      KPageWidgetItem* mPage;
  };

  const char* introPageName="Introduction";
  const char* infoPageName="Select printing information";
  const char* photoPageName="Photo information";
  const char* cropPageName="Crop photos";

  typedef WizardPage<Ui_IntroPage> IntroPage;
  typedef WizardPage<Ui_InfoPage>  InfoPage;
  typedef WizardPage<Ui_PhotoPage> PhotoPage;
  typedef WizardPage<Ui_CropPage>  CropPage;


// Wizard implementation
  struct Wizard::Private
  {
    IntroPage *mIntroPage;
    InfoPage  *mInfoPage;
    PhotoPage *mPhotoPage;
    CropPage  *mCropPage;

    KPushButton  *m_helpButton;
    QButtonGroup *m_outputSettings;

    KIPI::ImageCollectionSelector* mCollectionSelector;
    KIPI::Interface* mInterface;

    KIPIPlugins::KPAboutData* mAbout;

    PageSize           m_pageSize;
    QList<TPhoto*>     m_photos;
    QList<TPhotoSize*> m_photoSizes;
    int                m_currentPreviewPage;
    int                m_currentCropPhoto;
    bool               m_cancelPrinting;
    QString            m_tempPath;
    QStringList        m_gimpFiles;
  };

  Wizard::Wizard ( QWidget* parent, KIPI::Interface* interface )
      : KAssistantDialog ( parent )
  {
    d=new Private;
    d->mInterface = interface;

    // Caption
    setCaption ( i18n ( "Print assistant" ) );

    // About data
    d->mAbout =new KIPIPlugins::KPAboutData ( ki18n ( "Print assistant" ),
            QByteArray(),
            KAboutData::License_GPL,
            ki18n ( "A KIPI plugin to print images" ),
            ki18n ( "(c) 2003-2004, Todd Shoemaker\n(c) 2007-2008, Angelo Naselli" ) );

    d->mAbout->addAuthor ( ki18n ( "Todd Shoemaker" ), ki18n ( "Author" ),
                           "todd@theshoemakers.net" );
    d->mAbout->addAuthor ( ki18n ( "Angelo Naselli" ), ki18n ( "Developer and maintainer" ),
                           "anaselli@linux.it" );
    d->mAbout->addAuthor ( ki18n ( "Valerio Fuoglio" ), ki18n ( "Contributor" ),
                           "valerio.fuoglio@gmail.com" );

    d->mIntroPage = new IntroPage ( this, i18n ( introPageName ) );
    d->mInfoPage  = new InfoPage ( this, i18n ( infoPageName ) );
    d->mPhotoPage = new PhotoPage ( this, i18n ( photoPageName ) );
    d->mCropPage  = new CropPage ( this, i18n ( cropPageName ) ) ;

    d->m_helpButton = button ( Help );
    KHelpMenu* helpMenu = new KHelpMenu ( this, d->mAbout, false );
    helpMenu->menu()->removeAction ( helpMenu->menu()->actions().first() );
    QAction *handbook   = new QAction ( i18n ( "Handbook" ), this );

    // create a QButtonGroup to manage button ids
    d->m_outputSettings = new QButtonGroup ( this );
    d->m_outputSettings->addButton ( d->mInfoPage->RdoOutputPrinter, ToPrinter );
    d->m_outputSettings->addButton ( d->mInfoPage->RdoOutputGimp,    ToGimp );
    d->m_outputSettings->addButton ( d->mInfoPage->RdoOutputFile,    ToFile );

    d->m_pageSize = Unknown; // select a different page to force a refresh in initPhotoSizes.

    // connections
    // help
    connect ( handbook, SIGNAL ( triggered ( bool ) ),
              this, SLOT ( slotHelp() ) );

    // selected page
    connect ( this, SIGNAL ( currentPageChanged ( KPageWidgetItem *, KPageWidgetItem * ) ),
              this, SLOT ( pageChanged ( KPageWidgetItem *, KPageWidgetItem * ) ) );

    // cancel button
    connect ( this, SIGNAL ( cancelClicked() ),
              this, SLOT ( reject() ) );

    // caption information
    connect ( d->mInfoPage->m_captions, SIGNAL ( activated ( const QString & ) ),
              this, SLOT ( captionChanged ( const QString & ) ) );

    // Output
    connect ( d->m_outputSettings, SIGNAL ( buttonClicked ( int ) ),
              this, SLOT ( outputSettingsClicked ( int ) ) );

    // Browse path button
    connect ( d->mInfoPage->BtnBrowseOutputPath, SIGNAL ( clicked ( void ) ),
              this, SLOT ( btnBrowseOutputPathClicked ( void ) ) );

    // Paper Size
    connect ( d->mInfoPage->CmbPaperSize, SIGNAL ( activated ( int ) ),
              this, SLOT ( paperSizeChanged ( int ) ) );

    // Print order (down)
    connect ( d->mPhotoPage->BtnPrintOrderDown, SIGNAL ( clicked ( void ) ),
              this, SLOT ( BtnPrintOrderDown_clicked ( void ) ) );

    // Print order (up)
    connect ( d->mPhotoPage->BtnPrintOrderUp, SIGNAL ( clicked ( void ) ),
              this, SLOT ( BtnPrintOrderUp_clicked ( void ) ) );

    connect ( d->mPhotoPage->BtnPreviewPageUp, SIGNAL ( clicked ( void ) ),
              this, SLOT ( BtnPreviewPageUp_clicked ( void ) ) );

    connect ( d->mPhotoPage->BtnPreviewPageDown, SIGNAL ( clicked ( void ) ),
              this, SLOT ( BtnPreviewPageDown_clicked ( void ) ) );

    connect ( d->mCropPage->BtnCropPrev, SIGNAL ( clicked() ),
              this, SLOT ( BtnCropPrev_clicked() ) );

    connect ( d->mCropPage->BtnCropNext, SIGNAL ( clicked() ),
              this, SLOT ( BtnCropNext_clicked() ) );

    connect ( d->mCropPage->BtnCropRotate, SIGNAL ( clicked() ),
              this, SLOT ( BtnCropRotate_clicked() ) );

    connect ( d->mPhotoPage->ListPrintOrder, SIGNAL ( itemSelectionChanged() ),
              this, SLOT ( ListPrintOrder_selected() ) );

    connect ( d->mPhotoPage->ListPrintOrder, SIGNAL ( itemEntered ( QListWidgetItem * ) ),
              this, SLOT ( ListPrintOrder_selected() ) );

    connect ( d->mPhotoPage->EditCopies, SIGNAL ( valueChanged ( int ) ),
              this, SLOT ( EditCopies_valueChanged ( int ) ) );

    connect ( d->mPhotoPage->ListPhotoSizes, SIGNAL ( currentRowChanged ( int ) ),
              this, SLOT ( ListPhotoSizes_selected() ) );

    // don't crop
    connect ( d->mCropPage->m_disableCrop, SIGNAL ( stateChanged ( int ) ),
              this, SLOT ( crop_selection ( int ) ) );

    // remove a page
    connect ( this, SIGNAL ( pageRemoved ( KPageWidgetItem * ) ),
              this, SLOT ( PageRemoved ( KPageWidgetItem * ) ) );

    // Default is A4
    d->mInfoPage->CmbPaperSize->setCurrentIndex ( A4 );
    initPhotoSizes ( A4 );   // default to A4 for now.
    d->m_currentPreviewPage = 0;
    d->m_currentCropPhoto   = 0;
    d->m_cancelPrinting     = false;

    helpMenu->menu()->insertAction ( helpMenu->menu()->actions().first(), handbook );
    d->m_helpButton->setMenu ( helpMenu->menu() );

    readSettings();

    if ( d->mIntroPage->m_skipIntro->isChecked() )
    {
      removePage ( d->mIntroPage->page() );
    }
  }

  Wizard::~Wizard()
  {
    delete d->mAbout;
    delete d;
  }

// create a MxN grid of photos, fitting on the page
  TPhotoSize * createPhotoGrid ( int pageWidth, int pageHeight, QString label, int rows, int columns )
  {
    int MARGIN = ( int ) ( ( pageWidth + pageHeight ) / 2 * 0.04 + 0.5 );
    int GAP = MARGIN / 4;
    int photoWidth = ( pageWidth - ( MARGIN * 2 ) - ( ( columns-1 ) * GAP ) ) / columns;
    int photoHeight = ( pageHeight - ( MARGIN * 2 ) - ( ( rows-1 ) * GAP ) ) / rows;

    TPhotoSize *p = new TPhotoSize;
    p->label = label;
    p->dpi = 100;
    p->autoRotate = false;
    p->layouts.append ( new QRect ( 0, 0, pageWidth, pageHeight ) );

    int row = 0;
    for ( int y=MARGIN; row < rows && y < pageHeight - MARGIN; y += photoHeight + GAP )
    {
      int col = 0;
      for ( int x=MARGIN; col < columns && x < pageWidth - MARGIN; x += photoWidth + GAP )
      {
        p->layouts.append ( new QRect ( x, y, photoWidth, photoHeight ) );
        col++;
      }
      row++;
    }
    return p;
  }

  void Wizard::print ( KUrl::List fileList, QString tempPath )
  {
    for ( int i=0; i < d->m_photos.count(); i++ )
      if ( d->m_photos.at ( i ) )
        delete d->m_photos.at ( i );
    d->m_photos.clear();
    d->mPhotoPage->ListPrintOrder->clear();

    for ( int i=0; i < fileList.count(); i++ )
    {
      TPhoto *photo = new TPhoto ( 150 );
      photo->filename = fileList[i];
      d->m_photos.append ( photo );
      // load the print order listbox
      d->mPhotoPage->ListPrintOrder->addItem ( photo->filename.fileName() );
    }
    d->mPhotoPage->ListPrintOrder->setCurrentRow ( 0, QItemSelectionModel::Select );

    d->m_tempPath = tempPath;
    d->mPhotoPage->LblPhotoCount->setText ( QString::number ( d->m_photos.count() ) );

    d->mCropPage->BtnCropPrev->setEnabled ( false );

    if ( d->m_photos.count() == 1 )
      d->mCropPage->BtnCropNext->setEnabled ( false );
  }

// TODO page layout configurable (using XML?)
  void Wizard::initPhotoSizes ( PageSize pageSize )
  {
    // don't refresh anything if we haven't changed page sizes.
    if ( pageSize == d->m_pageSize )
      return;

    d->m_pageSize = pageSize;

    // cleanng m_pageSize memory before invoking clear()
    for ( int i=0; i < d->m_photoSizes.count(); i++ )
      if ( d->m_photoSizes.at ( i ) )
        delete d->m_photoSizes.at ( i );
    d->m_photoSizes.clear();

    switch ( pageSize )
    {
        // ====================== LETTER SIZE =====================
      case Letter:
      {
        TPhotoSize *p;
        // ========== 5 x 3.5
        p = new TPhotoSize;
        p->label = i18n ( "3.5 x 5\"" );
        p->dpi = 0;
        p->autoRotate = true;
        // page size
        p->layouts.append ( new QRect ( 0, 0, 8500, 11000 ) );
        // photo layouts
        p->layouts.append ( new QRect ( 700,  500, 3500, 5000 ) );
        p->layouts.append ( new QRect ( 4300,  500, 3500, 5000 ) );
        p->layouts.append ( new QRect ( 700, 5600, 3500, 5000 ) );
        p->layouts.append ( new QRect ( 4300, 5600, 3500, 5000 ) );
        // add to the list
        d->m_photoSizes.append ( p );

        // ========== 4 x 6
        p = new TPhotoSize;
        p->label = i18n ( "4 x 6\"" );
        p->dpi = 0;
        p->autoRotate = true;
        // page size
        p->layouts.append ( new QRect ( 0, 0, 8500, 11000 ) );
        // photo layouts
        p->layouts.append ( new QRect ( 225,  500, 4000, 6000 ) );
        p->layouts.append ( new QRect ( 4275,  500, 4000, 6000 ) );
        p->layouts.append ( new QRect ( 1250, 6600, 6000, 4000 ) );
        // add to the list
        d->m_photoSizes.append ( p );

        // ========== 4 x 6 Album
        p = new TPhotoSize;
        p->label = i18n ( "4 x 6\" Album" );
        p->dpi = 0;
        p->autoRotate = true;
        // page size
        p->layouts.append ( new QRect ( 0, 0, 8500, 11000 ) );
        // photo layouts
        p->layouts.append ( new QRect ( 1250,  1000, 6000, 4000 ) );
        p->layouts.append ( new QRect ( 1250,  6000, 6000, 4000 ) );
        // add to the list
        d->m_photoSizes.append ( p );

        // ========== 5 x 7
        p = new TPhotoSize;
        p->label = i18n ( "5 x 7\"" );
        p->dpi = 0;
        p->autoRotate = true;
        // page size
        p->layouts.append ( new QRect ( 0, 0, 8500, 11000 ) );
        // photo layouts
        p->layouts.append ( new QRect ( 750,  500, 7000, 5000 ) );
        p->layouts.append ( new QRect ( 750, 5750, 7000, 5000 ) );
        // add to the list
        d->m_photoSizes.append ( p );

        // ========== 8 x 10
        p = new TPhotoSize;
        p->label = i18n ( "8 x 10\"" );
        p->dpi = 0;
        p->autoRotate = true;
        // page size
        p->layouts.append ( new QRect ( 0, 0, 8500, 11000 ) );
        // photo layouts
        p->layouts.append ( new QRect ( 250, 500, 8000, 10000 ) );
        // add to the list
        d->m_photoSizes.append ( p );

        // thumbnails
        d->m_photoSizes.append ( createPhotoGrid ( 8500, 11000, i18n ( "Thumbnails" ), 5, 4 ) );

        // small thumbnails
        d->m_photoSizes.append ( createPhotoGrid ( 8500, 11000, i18n ( "Small Thumbnails" ), 6, 5 ) );

        // album collage 1
        p = new TPhotoSize;
        p->label = i18n ( "Album Collage 1 (9 photos)" );
        p->dpi = 0;
        p->autoRotate = false;
        // page size
        p->layouts.append ( new QRect ( 0, 0, 8500, 11000 ) );
        // photo layouts
        // photo 1 is in the center, 3x4.5
        p->layouts.append ( new QRect ( 2750, 3250, 3000, 4500 ) );
        // the remaining 1.5x2 photos begin with upper left and circle around
        // top row
        p->layouts.append ( new QRect ( 750, 750, 1500, 2000 ) );
        p->layouts.append ( new QRect ( 3500, 750, 1500, 2000 ) );
        p->layouts.append ( new QRect ( 6250, 750, 1500, 2000 ) );
        p->layouts.append ( new QRect ( 6250, 4500, 1500, 2000 ) );
        p->layouts.append ( new QRect ( 6250, 8250, 1500, 2000 ) );
        p->layouts.append ( new QRect ( 3500, 8250, 1500, 2000 ) );
        p->layouts.append ( new QRect ( 750, 8250, 1500, 2000 ) );
        p->layouts.append ( new QRect ( 750, 4500, 1500, 2000 ) );
        d->m_photoSizes.append ( p );

        // album collage 2
        p = new TPhotoSize;
        p->label = i18n ( "Album Collage 2 (6 photos)" );
        p->dpi = 0;
        p->autoRotate = false;
        // page size
        p->layouts.append ( new QRect ( 0, 0, 8500, 11000 ) );
        // photo layouts
        p->layouts.append ( new QRect ( 1000, 1000, 3000, 3000 ) );
        p->layouts.append ( new QRect ( 5000, 1000, 2500, 1250 ) );
        p->layouts.append ( new QRect ( 5000, 2750, 2500, 1250 ) );
        p->layouts.append ( new QRect ( 1000, 5000, 1500, 2000 ) );
        p->layouts.append ( new QRect ( 2750, 5000, 4750, 2000 ) );
        p->layouts.append ( new QRect ( 1000, 8000, 6500, 2000 ) );
        d->m_photoSizes.append ( p );
      } // letter
      break;

      // ====================== A4 SIZE =====================
      case A4:
      {
        // A4 is 21 x 29.7cm
        TPhotoSize *p;

        // ========== 20x25cm
        p = new TPhotoSize;
        p->dpi = 0;
        p->autoRotate = true;
        p->label = i18n ( "21 x 29.7cm" );
        // page size
        p->layouts.append ( new QRect ( 0, 0, 2100, 2970 ) );
        // photo layouts
        p->layouts.append ( new QRect ( 0, 0, 2100, 2970 ) );
        // add to the list
        d->m_photoSizes.append ( p );

        // ========== 6x9 cm - 8 photos
        p = new TPhotoSize;
        p->dpi = 0;
        p->autoRotate = true;
        p->label = i18n ( "6 x 9cm (8 photos)" );
        // page size
        p->layouts.append ( new QRect ( 0, 0, 2100, 2970 ) );
        // photo layouts
        p->layouts.append ( new QRect ( 100,  100, 900, 600 ) );
        p->layouts.append ( new QRect ( 1100,  100, 900, 600 ) );
        p->layouts.append ( new QRect ( 100,  800, 900, 600 ) );
        p->layouts.append ( new QRect ( 1100,  800, 900, 600 ) );
        p->layouts.append ( new QRect ( 100, 1500, 900, 600 ) );
        p->layouts.append ( new QRect ( 1100, 1500, 900, 600 ) );
        p->layouts.append ( new QRect ( 100, 2200, 900, 600 ) );
        p->layouts.append ( new QRect ( 1100, 2200, 900, 600 ) );
        // add to the list
        d->m_photoSizes.append ( p );

        // ========== 9x13
        p = new TPhotoSize;
        p->dpi = 0;
        p->autoRotate = true;
        p->label = i18n ( "9 x 13cm" );
        // page size
        p->layouts.append ( new QRect ( 0, 0, 2100, 2970 ) );
        // photo layouts
        p->layouts.append ( new QRect ( 100,  100, 900, 1300 ) );
        p->layouts.append ( new QRect ( 1100,  100, 900, 1300 ) );
        p->layouts.append ( new QRect ( 100, 1500, 900, 1300 ) );
        p->layouts.append ( new QRect ( 1100, 1500, 900, 1300 ) );
        // add to the list
        d->m_photoSizes.append ( p );

        // ========== 10x13.33cm
        p = new TPhotoSize;
        p->dpi = 0;
        p->autoRotate = true;
        p->label = i18n ( "10 x 13.33cm" );
        // page size
        p->layouts.append ( new QRect ( 0, 0, 2100, 2970 ) );
        // photo layouts
        p->layouts.append ( new QRect ( 50,  100, 1000, 1333 ) );
        p->layouts.append ( new QRect ( 1060,  100, 1000, 1333 ) );
        p->layouts.append ( new QRect ( 50, 1500, 1000, 1333 ) );
        p->layouts.append ( new QRect ( 1060, 1500, 1000, 1333 ) );
        // add to the list
        d->m_photoSizes.append ( p );

        // ========== 10x15cm
        p = new TPhotoSize;
        p->dpi = 0;
        p->autoRotate = true;
        p->label = i18n ( "10 x 15cm" );
        // page size
        p->layouts.append ( new QRect ( 0, 0, 2100, 2970 ) );
        // photo layouts
        p->layouts.append ( new QRect ( 50,  150, 1000, 1500 ) );
        p->layouts.append ( new QRect ( 1060,  150, 1000, 1500 ) );
        p->layouts.append ( new QRect ( 300, 1750, 1500, 1000 ) );
        // add to the list
        d->m_photoSizes.append ( p );

        // ========== 10x15cm album
        p = new TPhotoSize;
        p->dpi = 0;
        p->autoRotate = true;
        p->label = i18n ( "10 x 15cm Album" );
        // page size
        p->layouts.append ( new QRect ( 0, 0, 2100, 2970 ) );
        // photo layouts
        p->layouts.append ( new QRect ( 300, 350, 1500, 1000 ) );
        p->layouts.append ( new QRect ( 300, 1620, 1500, 1000 ) );
        // add to the list
        d->m_photoSizes.append ( p );

        // ========== 11.5x15cm album
        p = new TPhotoSize;
        p->dpi = 0;
        p->autoRotate = true;
        p->label = i18n ( "11.5 x 15cm Album" );
        // page size
        p->layouts.append ( new QRect ( 0, 0, 2100, 2970 ) );
        // photo layouts
        p->layouts.append ( new QRect ( 300, 250, 1500, 1100 ) );
        p->layouts.append ( new QRect ( 300, 1570, 1500, 1100 ) );
        // add to the list
        d->m_photoSizes.append ( p );

        // ========== 13x18cm
        p = new TPhotoSize;
        p->dpi = 0;
        p->autoRotate = true;
        p->label = i18n ( "13 x 18cm" );
        // page size
        p->layouts.append ( new QRect ( 0, 0, 2100, 2970 ) );
        // photo layouts
        p->layouts.append ( new QRect ( 150, 150, 1800, 1300 ) );
        p->layouts.append ( new QRect ( 150, 1520, 1800, 1300 ) );
        // add to the list
        d->m_photoSizes.append ( p );

        // ========== 20x25cm
        p = new TPhotoSize;
        p->dpi = 0;
        p->autoRotate = true;
        p->label = i18n ( "20 x 25cm" );
        // page size
        p->layouts.append ( new QRect ( 0, 0, 2100, 2970 ) );
        // photo layouts
        p->layouts.append ( new QRect ( 50, 230, 2000, 2500 ) );
        // add to the list
        d->m_photoSizes.append ( p );

        // thumbnails
        d->m_photoSizes.append ( createPhotoGrid ( 2100, 2970, i18n ( "Thumbnails" ), 5, 4 ) );

        // small thumbnails
        d->m_photoSizes.append ( createPhotoGrid ( 2100, 2970, i18n ( "Small Thumbnails" ), 6, 5 ) );
      } // A4
      break;

      // ====================== A6 SIZE =====================
      case A6:
      {
        // A6 is 10.5 x 14.8 cm
        TPhotoSize *p;
        // ========== 9x13
        p = new TPhotoSize;
        p->dpi = 0;
        p->autoRotate = true;
        p->label = i18n ( "9 x 13cm" );
        // page size
        p->layouts.append ( new QRect ( 0, 0, 1050, 1480 ) );
        // photo layouts
        p->layouts.append ( new QRect ( 50,  100, 900, 1300 ) );
        // add to the list
        d->m_photoSizes.append ( p );

        // ========== 10x15cm
        p = new TPhotoSize;
        p->dpi = 0;
        p->autoRotate = true;
        p->label = i18n ( "10.5 x 14.8cm" );
        // page size
        p->layouts.append ( new QRect ( 0, 0, 1050, 1480 ) );
        // photo layouts
        p->layouts.append ( new QRect ( 0, 0, 1050, 1480 ) );
        // add to the list
        d->m_photoSizes.append ( p );

        // thumbnails
        d->m_photoSizes.append ( createPhotoGrid ( 1050, 1480, i18n ( "Thumbnails" ), 5, 4 ) );

        // small thumbnails
        d->m_photoSizes.append ( createPhotoGrid ( 1050, 1480, i18n ( "Small Thumbnails" ), 6, 5 ) );
      } // A6
      break;

      // ====================== 10x15cm SIZE =====================
      case P10X15:
      {
        // 10x15cm photo paper is 4x6" so the right size is 10.16 x 15.24 cm
        TPhotoSize *p;
        // ========== 10x15cm
        p = new TPhotoSize;
        p->dpi = 0;
        p->autoRotate = true;
        p->label = i18n ( "10 x 15cm" );
        // page size
        p->layouts.append ( new QRect ( 0, 0, 1016, 1524 ) );
        // photo layouts
        p->layouts.append ( new QRect ( 0, 0, 1016, 1524 ) );
        // add to the list
        d->m_photoSizes.append ( p );

        // ========== 9x13
        p = new TPhotoSize;
        p->dpi = 0;
        p->autoRotate = true;
        p->label = i18n ( "9 x 13cm" );
        // page size
        p->layouts.append ( new QRect ( 0, 0, 1016, 1524 ) );
        // photo layouts
        p->layouts.append ( new QRect ( 50,  100, 900, 1300 ) );
        // add to the list
        d->m_photoSizes.append ( p );

        // thumbnails
        d->m_photoSizes.append ( createPhotoGrid ( 1016, 1524, i18n ( "Thumbnails" ), 5, 4 ) );

        // small thumbnails
        d->m_photoSizes.append ( createPhotoGrid ( 1016, 1524, i18n ( "Small Thumbnails" ), 6, 5 ) );

      } // 10x15 cm
      break;

      // ====================== 13x18cm SIZE =====================
      case P13X18:
      {
        // 10x18cm photo paper is 5x7" so the right conversion
        // is 12.7 x 17.78 cm
        TPhotoSize *p;
        // ========== 10x15cm
        p = new TPhotoSize;
        p->dpi = 0;
        p->autoRotate = true;
        p->label = i18n ( "13 x 18cm" );
        // page size
        p->layouts.append ( new QRect ( 0, 0, 1270, 1778 ) );
        // photo layouts
        p->layouts.append ( new QRect ( 0, 0, 1270, 1778 ) );
        // add to the list
        d->m_photoSizes.append ( p );

        // ========== 10x15cm
        p = new TPhotoSize;
        p->dpi = 0;
        p->autoRotate = true;
        p->label = i18n ( "10 x 15cm" );
        // page size
        p->layouts.append ( new QRect ( 0, 0, 1270, 1778 ) );
        // photo layouts
        p->layouts.append ( new QRect ( 0, 0, 1016, 1524 ) );
        // add to the list
        d->m_photoSizes.append ( p );

        // ========== 9x13
        p = new TPhotoSize;
        p->dpi = 0;
        p->autoRotate = true;
        p->label = i18n ( "9 x 13cm" );
        // page size
        p->layouts.append ( new QRect ( 0, 0, 1270, 1778 ) );
        // photo layouts
        p->layouts.append ( new QRect ( 50,  100, 900, 1300 ) );
        // add to the list
        d->m_photoSizes.append ( p );

        // thumbnails
        d->m_photoSizes.append ( createPhotoGrid ( 1270, 1778, i18n ( "Thumbnails" ), 5, 4 ) );

        // small thumbnails
        d->m_photoSizes.append ( createPhotoGrid ( 1270, 1778, i18n ( "Small Thumbnails" ), 6, 5 ) );

      } // 13x18 cm
      break;

      default:
      {
        kDebug(51000) << "Initializing Unsupported page layouts\n";
        // We don't support this page size yet.  Just create a default page.
        TPhotoSize *p;
        p = new TPhotoSize;
        p->dpi = 0;
        p->autoRotate = false;
        p->label = i18n ( "Unsupported Paper Size" );
        // page size
        p->layouts.append ( new QRect ( 0, 0, 8500, 11000 ) );
        p->layouts.append ( new QRect ( 0, 0, 8500, 11000 ) );
        // add to the list
        d->m_photoSizes.append ( p );
      }
      break;
    };

    // load the photo sizes into the listbox
    d->mPhotoPage->ListPhotoSizes->blockSignals ( true );
    d->mPhotoPage->ListPhotoSizes->clear();
    QList<TPhotoSize*>::iterator it;
    for ( it = d->m_photoSizes.begin(); it != d->m_photoSizes.end(); ++it )
    {
      TPhotoSize *s = static_cast<TPhotoSize*> ( *it );
      if ( s ) d->mPhotoPage->ListPhotoSizes->addItem ( s->label );
    }
    d->mPhotoPage->ListPhotoSizes->blockSignals ( false );

    d->mPhotoPage->ListPhotoSizes->setCurrentRow ( 0, QItemSelectionModel::Select );

//   d->mPhotoPage->ListPhotoSizes->setCurrentItem(0);
  }

  double getMaxDPI ( QList<TPhoto*> photos, QList<QRect*> layouts, /*unsigned*/ int current )
  {
    Q_ASSERT ( layouts.count() > 1 );

    QList<QRect*>::iterator it = layouts.begin();
    QRect *layout = static_cast<QRect*> ( *it );

    double maxDPI = 0.0;

    for ( ; current < photos.count(); current++ )
    {
      TPhoto *photo = photos.at ( current );
      double dpi = ( ( double ) photo->cropRegion.width() + ( double ) photo->cropRegion.height() ) /
                   ( ( ( double ) layout->width() / 1000.0 ) + ( ( double ) layout->height() / 1000.0 ) );
      if ( dpi > maxDPI )
        maxDPI = dpi;
      // iterate to the next position
      ++it;
      layout = ( it == layouts.end() ) ? 0 : static_cast<QRect*> ( *it );
      if ( layout == 0 )
      {
        break;
      }
    }
    return maxDPI;
  }

  QRect * Wizard::getLayout ( int photoIndex )
  {
    TPhotoSize *s = d->m_photoSizes.at ( d->mPhotoPage->ListPhotoSizes->currentRow() );
    // how many photos would actually be printed, including copies?
    int photoCount  = ( photoIndex + 1 );
    // how many pages?  Recall that the first layout item is the paper size
    int photosPerPage = s->layouts.count() - 1;
    int remainder = photoCount % photosPerPage;

    int retVal = remainder;
    if ( remainder == 0 )
      retVal = photosPerPage;
    return s->layouts.at ( retVal );
  }

  int Wizard::getPageCount()
  {
    // get the selected layout
    TPhotoSize *s = d->m_photoSizes.at ( d->mPhotoPage->ListPhotoSizes->currentRow() );

    int photoCount  =  d->m_photos.count();
    // how many pages?  Recall that the first layout item is the paper size
    int photosPerPage = s->layouts.count() - 1;
    int remainder = photoCount % photosPerPage;
    int emptySlots = 0;
    if ( remainder > 0 )
      emptySlots = photosPerPage - remainder;
    int pageCount = photoCount / photosPerPage;
    if ( emptySlots > 0 )
      pageCount++;
    return pageCount;
  }


  const float FONT_HEIGHT_RATIO = 0.8F;

  void Wizard::printCaption ( QPainter &p, TPhoto* /*photo*/, int captionW, int captionH, QString caption )
  {
    // PENDING anaselli TPhoto*photo will be needed to add a per photo caption management
    QStringList captionByLines;

    int captionIndex = 0;

    while ( captionIndex < caption.length() )
    {
      QString newLine;
      bool breakLine = false; // End Of Line found
      int currIndex; //  Caption QString current index

      // Check minimal lines dimension
      //TODO fix length, maybe useless
      int captionLineLocalLength = 40;

      for ( currIndex = captionIndex; currIndex < caption.length() && !breakLine; currIndex++ )
        if ( caption[currIndex] == QChar ( '\n' ) || caption[currIndex].isSpace() )
          breakLine = true;

      if ( captionLineLocalLength <= ( currIndex - captionIndex ) )
        captionLineLocalLength = ( currIndex - captionIndex );

      breakLine = false;

      for ( currIndex = captionIndex;
              currIndex <= captionIndex + captionLineLocalLength &&
              currIndex < caption.length() && !breakLine;
              currIndex++ )
      {
        breakLine = ( caption[currIndex] == QChar ( '\n' ) ) ? true : false;

        if ( breakLine )
          newLine.append ( ' ' );
        else
          newLine.append ( caption[currIndex] );
      }

      captionIndex = currIndex; // The line is ended

      if ( captionIndex != caption.length() )
        while ( !newLine.endsWith ( " " ) )
        {
          newLine.truncate ( newLine.length() - 1 );
          captionIndex--;
        }

      captionByLines.prepend ( newLine.trimmed() );
    }

    QFont font ( d->mInfoPage->m_font_name->currentFont() );
    font.setStyleHint ( QFont::SansSerif );
    font.setPixelSize ( ( int ) ( captionH * FONT_HEIGHT_RATIO ) );
    font.setWeight ( QFont::Normal );

    QFontMetrics fm ( font );
    int pixelsHigh = fm.height();

    p.setFont ( font );
    p.setPen ( d->mInfoPage->m_font_color->color() );
    kDebug ( 51000 ) << "Number of lines " << ( int ) captionByLines.count() << endl;

    // Now draw the caption
    // TODO allow printing captions  per photo and on top, bottom and vertically
    for ( int lineNumber = 0; lineNumber < ( int ) captionByLines.count(); lineNumber++ )
    {
      if ( lineNumber > 0 )
        p.translate ( 0, - ( int ) ( pixelsHigh ) );
      QRect r ( 0, 0, captionW, captionH );
      //TODO anaselli check if ok
      p.drawText ( r, Qt::AlignLeft, captionByLines[lineNumber], &r );
    }
  }



  QString Wizard::captionFormatter ( TPhoto *photo, const QString& format )
  {
    QString str=format;

    QFileInfo fi ( photo->filename.path() );
    QString resolution;
    QSize imageSize =  photo->exiv2Iface()->getImageDimensions();
    if ( imageSize.isValid() )
    {
      resolution = QString ( "%1x%2" ).arg ( imageSize.width() ).arg ( imageSize.height() );
    }
    str.replace ( "\\n", "\n" );

    // %f filename
    // %c comment
    // %d date-time
    // %t exposure time
    // %i iso
    // %r resolution
    // %a aperture
    // %l focal length
    str.replace ( "%f", fi.fileName() );
    str.replace ( "%c", photo->exiv2Iface()->getExifComment() );
    str.replace ( "%d", KGlobal::locale()->formatDateTime ( photo->exiv2Iface()->getImageDateTime() ) );
    str.replace ( "%t", photo->exiv2Iface()->getExifTagString ( "Exif.Photo.ExposureTime" ) );
    str.replace ( "%i", photo->exiv2Iface()->getExifTagString ( "Exif.Photo.ISOSpeedRatings" ) );
    str.replace ( "%r", resolution );
    str.replace ( "%a", photo->exiv2Iface()->getExifTagString ( "Exif.Photo.FNumber" ) );
    str.replace ( "%l", photo->exiv2Iface()->getExifTagString ( "Exif.Photo.FocalLength" ) );

    return str;
  }

  bool Wizard::paintOnePage ( QPainter &p, QList<TPhoto*> photos, QList<QRect*> layouts,
                              int captionType, int &current, bool useThumbnails )
  {
    Q_ASSERT ( layouts.count() > 1 );

    if ( photos.count() == 0 ) return true; // no photos => last photo

    QList<QRect*>::iterator it = layouts.begin();
    QRect *srcPage = static_cast<QRect*> ( *it );
    ++it;
    QRect *layout = static_cast<QRect*> ( *it );

    // scale the page size to best fit the painter
    // size the rectangle based on the minimum image dimension
    int destW = p.window().width();
    int destH = p.window().height();

    int srcW = srcPage->width();
    int srcH = srcPage->height();
    if ( destW < destH )
    {
      destH = NINT ( ( double ) destW * ( ( double ) srcH / ( double ) srcW ) );
      if ( destH > p.window().height() )
      {
        destH = p.window().height();
        destW = NINT ( ( double ) destH * ( ( double ) srcW / ( double ) srcH ) );
      }
    }
    else
    {
      destW = NINT ( ( double ) destH * ( ( double ) srcW / ( double ) srcH ) );
      if ( destW > p.window().width() )
      {
        destW = p.window().width();
        destH = NINT ( ( double ) destW * ( ( double ) srcH / ( double ) srcW ) );
      }
    }

    double xRatio = ( double ) destW / ( double ) srcPage->width();
    double yRatio = ( double ) destH / ( double ) srcPage->height();

    int left = ( p.window().width()  - destW ) / 2;
    int top  = ( p.window().height() - destH ) / 2;

    // FIXME: may not want to erase the background page
    p.eraseRect ( left, top,
                  NINT ( ( double ) srcPage->width() * xRatio ),
                  NINT ( ( double ) srcPage->height() * yRatio ) );

    for ( ; current < photos.count(); current++ )
    {
      TPhoto *photo = photos.at ( current );
      // crop
      QImage img;
      if ( useThumbnails )
        img = photo->thumbnail().toImage();
      else
        img = photo->loadPhoto();

      // next, do we rotate?
      if ( photo->rotation != 0 )
      {
        // rotate
        QMatrix matrix;
        matrix.rotate ( photo->rotation );
        img = img.transformed ( matrix );
      }

      if ( useThumbnails )
      {
        // scale the crop region to thumbnail coords
        double xRatio = 0.0;
        double yRatio = 0.0;

        if ( photo->thumbnail().width() != 0 )
          xRatio = ( double ) photo->thumbnail().width() / ( double ) photo->width();
        if ( photo->thumbnail().height() != 0 )
          yRatio = ( double ) photo->thumbnail().height() / ( double ) photo->height();

        int x1 = NINT ( ( double ) photo->cropRegion.left() * xRatio );
        int y1 = NINT ( ( double ) photo->cropRegion.top()  * yRatio );

        int w = NINT ( ( double ) photo->cropRegion.width()  * xRatio );
        int h = NINT ( ( double ) photo->cropRegion.height() * yRatio );

        img = img.copy ( QRect ( x1, y1, w, h ) );
      }
      else if ( !d->mCropPage->m_disableCrop->isChecked() )
      {
        img = img.copy ( photo->cropRegion );
      }

      int x1 = NINT ( ( double ) layout->left() * xRatio );
      int y1 = NINT ( ( double ) layout->top()  * yRatio );
      int w  = NINT ( ( double ) layout->width() * xRatio );
      int h  = NINT ( ( double ) layout->height() * yRatio );

      QRect rectViewPort = p.viewport();
      QRect newRectViewPort = QRect ( x1 + left, y1 + top, w, h );
      QSize imageSize = img.size();
      QPoint point;
      if ( d->mCropPage->m_disableCrop->isChecked() )
      {
        imageSize.scale ( newRectViewPort.size(), Qt::KeepAspectRatio );
        p.setViewport ( newRectViewPort.x(), newRectViewPort.y(), imageSize.width(), imageSize.height() );
        point = QPoint ( newRectViewPort.x() +imageSize.width(), newRectViewPort.y() +imageSize.height() );
      }
      else
      {
        p.setViewport ( newRectViewPort );
        point = QPoint ( x1 + left+w,y1 + top + w );
      }
      QRect rectWindow = p.window();
      p.setWindow ( img.rect() );
      p.drawImage ( 0, 0, img );
      p.setViewport ( rectViewPort );
      p.setWindow ( rectWindow );
      p.setBrushOrigin ( point );

      if ( captionType > 0 )
      {
        p.save();
        QString caption;
        QString format;
        switch ( captionType )
        {
          case FileNames:
            format = "%f";
            break;
          case ExifDateTime:
            format = "%d";
            break;
          case Comment:
            format = "%c";
            break;
          case Free:
            format = d->mInfoPage->m_FreeCaptionFormat->text();
            break;
          default:
            kWarning ( 51000 ) << "UNKNOWN caption type " << captionType << endl;
            break;
        }
        caption = captionFormatter ( photo, format );
        kDebug ( 51000 ) << "Caption " << caption << endl;

        // draw the text at (0,0), but we will translate and rotate the world
        // before drawing so the text will be in the correct location
        // next, do we rotate?
        int captionW = w-2;
        double ratio =  d->mInfoPage->m_font_size->value() * 0.01;
        int captionH = ( int ) ( qMin ( w, h ) * ratio );

        int exifOrientation = photo->exiv2Iface()->getImageOrientation();
        int orientatation = photo->rotation;


        //ORIENTATION_ROT_90_HFLIP .. ORIENTATION_ROT_270
        if ( exifOrientation == KExiv2Iface::KExiv2::ORIENTATION_ROT_90_HFLIP ||
                exifOrientation == KExiv2Iface::KExiv2::ORIENTATION_ROT_90 ||
                exifOrientation == KExiv2Iface::KExiv2::ORIENTATION_ROT_90_VFLIP ||
                exifOrientation == KExiv2Iface::KExiv2::ORIENTATION_ROT_270 )
          orientatation = ( photo->rotation + 270 ) % 360; // -90 degrees

        if ( orientatation == 90 || orientatation == 270 )
        {
          captionW = h;
        }
        p.rotate ( orientatation );
        kDebug ( 51000 ) << "rotation " << photo->rotation << " orientation " << orientatation << endl;
        int tx = left;
        int ty = top;

        switch ( orientatation )
        {
          case 0 :
          {
            tx += x1 + 1;
            ty += y1 + ( h - captionH - 1 );
            break;
          }
          case 90 :
          {
            tx = top + y1 + 1;
            ty = -left - x1 - captionH - 1;
            break;
          }
          case 180 :
          {
            tx = -left - x1 - w + 1;
            ty = -top -y1 - ( captionH + 1 );
            break;
          }
          case 270 :
          {
            tx = -top - y1 - h + 1;
            ty = left + x1 + ( w - captionH )- 1;
            break;
          }
        }
        p.translate ( tx, ty );
        printCaption ( p, photo, captionW, captionH, caption );
        p.restore();
      } // caption

      // iterate to the next position
      ++it;
      layout = it == layouts.end() ? 0 : static_cast<QRect*> ( *it );
      if ( layout == 0 )
      {
        current++;
        break;
      }
    }
    // did we print the last photo?
    return ( current < photos.count() );
  }


// Like above, but outputs to an initialized QImage.  UseThumbnails is
// not an option.
// We have to use QImage for saving to a file, otherwise we would have
// to use a QPixmap, which will have the same bit depth as the display.
// So someone with an 8-bit display would not be able to save 24-bit
// images!
  bool Wizard::paintOnePage ( QImage &p, QList<TPhoto*> photos, QList<QRect*> layouts,
                              int captionType, int &current )
  {
    Q_ASSERT ( layouts.count() > 1 );

    QList<QRect*>::iterator it = layouts.begin();
    QRect *srcPage = static_cast<QRect*> ( *it );
    ++it;
    QRect *layout = static_cast<QRect*> ( *it );

    // scale the page size to best fit the painter
    // size the rectangle based on the minimum image dimension
    int destW = p.width();
    int destH = p.height();

    int srcW = srcPage->width();
    int srcH = srcPage->height();
    if ( destW < destH )
    {
      destH = NINT ( ( double ) destW * ( ( double ) srcH / ( double ) srcW ) );
      if ( destH > p.height() )
      {
        destH = p.height();
        destW = NINT ( ( double ) destH * ( ( double ) srcW / ( double ) srcH ) );
      }
    }
    else
    {
      destW = NINT ( ( double ) destH * ( ( double ) srcW / ( double ) srcH ) );
      if ( destW > p.width() )
      {
        destW = p.width();
        destH = NINT ( ( double ) destW * ( ( double ) srcH / ( double ) srcW ) );
      }
    }

    double xRatio = ( double ) destW / ( double ) srcPage->width();
    double yRatio = ( double ) destH / ( double ) srcPage->height();

    int left = ( p.width()  - destW ) / 2;
    int top  = ( p.height() - destH ) / 2;


    p.fill ( 0xffffff );

    for ( ; current < photos.count(); current++ )
    {
      TPhoto *photo = photos.at ( current );
      // crop
      QImage img;
      img = photo->loadPhoto();

      // next, do we rotate?
      if ( photo->rotation != 0 )
      {
        // rotate
        QMatrix matrix;
        matrix.rotate ( photo->rotation );
        img = img.transformed ( matrix );
      }

      img = img.copy ( photo->cropRegion );

      int x1 = NINT ( ( double ) layout->left() * xRatio );
      int y1 = NINT ( ( double ) layout->top()  * yRatio );
      int w  = NINT ( ( double ) layout->width() * xRatio );
      int h  = NINT ( ( double ) layout->height() * yRatio );

      // We can use scaleFree because the crop frame should have the proper dimensions.
      img = img.scaled ( w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation );
//     img = img.smoothScale(w, h, QImage::ScaleFree);

      // don't have drawimage, so we copy the pixels over manually
      for ( int srcY = 0; srcY < img.height(); srcY++ )
        for ( int srcX = 0; srcX < img.width(); srcX++ )
        {
          p.setPixel ( x1 + left + srcX, y1 + top + srcY, img.pixel ( srcX, srcY ) );
        }

      if ( captionType != NoCaptions )
      {
        // Now draw the caption
        QString caption;
        QString format;
        switch ( captionType )
        {
          case FileNames:
            format = "%f";
            break;
          case ExifDateTime:
            format = "%d";
            break;
          case Comment:
            format = "%c";
            break;
          case Free:
            format = d->mInfoPage->m_FreeCaptionFormat->text();
            break;
          default:
            kWarning ( 51000 ) << "UNKNOWN caption type " << captionType << endl;
            break;
        }
        caption = captionFormatter ( photo, format );
        kDebug ( 51000 ) << "Caption " << caption << endl;

        int captionW = w-2;
        double ratio = d->mInfoPage->m_font_size->value() * 0.01;
        int captionH = ( int ) ( qMin ( w, h ) * ratio );

        int exifOrientation = photo->exiv2Iface()->getImageOrientation();
        int orientatation = photo->rotation;

        //ORIENTATION_ROT_90_HFLIP .. ORIENTATION_ROT_270
        if ( exifOrientation == KExiv2Iface::KExiv2::ORIENTATION_ROT_90_HFLIP ||
                exifOrientation == KExiv2Iface::KExiv2::ORIENTATION_ROT_90 ||
                exifOrientation == KExiv2Iface::KExiv2::ORIENTATION_ROT_90_VFLIP ||
                exifOrientation == KExiv2Iface::KExiv2::ORIENTATION_ROT_270 )
          orientatation = ( photo->rotation + 270 ) % 360; // -90 degrees

        if ( orientatation == 90 || orientatation == 270 )
        {
          captionW = h;
        }

        QPixmap pixmap ( w-2, img.height()-2 );
        //TODO black is not ok if font is black...
        pixmap.fill ( Qt::black );
        QPainter painter;
        painter.begin ( &pixmap );
        painter.rotate ( orientatation );
        kDebug ( 51000 ) << "rotation " << photo->rotation << " orientation " << orientatation << endl;
        int tx = left;
        int ty = top;

        switch ( orientatation )
        {
          case 0 :
          {
            tx += x1 + 1;
            ty += y1 + ( h - captionH - 1 );
            break;
          }
          case 90 :
          {
            tx = top + y1 + 1;
            ty = -left - x1 - captionH - 1;
            break;
          }
          case 180 :
          {
            tx = -left - x1 - w + 1;
            ty = -top -y1 - ( captionH + 1 );
            break;
          }
          case 270 :
          {
            tx = -top - y1 - h + 1;
            ty = left + x1 + ( w - captionH )- 1;
            break;
          }
        }

        painter.translate ( tx, ty );
        printCaption ( painter, photo, captionW, captionH, caption );
        painter.end();

        // now put it on picture
        QImage fontImage = pixmap.toImage();
        QRgb black = QColor ( 0, 0, 0 ).rgb();
        for ( int srcY = 0; srcY < fontImage.height(); srcY++ )
          for ( int srcX = 0; srcX < fontImage.width(); srcX++ )
          {
            if ( fontImage.pixel ( srcX, srcY ) != black )
              p.setPixel ( srcX, srcY, fontImage.pixel ( srcX, srcY ) );
          }
      } // caption

      // iterate to the next position
      ++it;
      layout = ( it == layouts.end() ) ? 0 : static_cast<QRect*> ( *it );
      if ( layout == 0 )
      {
        current++;
        break;
      }
    }
    // did we print the last photo?
    return ( current < photos.count() );
  }

  void Wizard::updateCropFrame ( TPhoto *photo, int photoIndex )
  {
    TPhotoSize *s = d->m_photoSizes.at ( d->mPhotoPage->ListPhotoSizes->currentRow() );
    d->mCropPage->cropFrame->init ( photo, getLayout ( photoIndex )->width(), getLayout ( photoIndex )->height(), s->autoRotate );
    d->mCropPage->LblCropPhoto->setText ( i18n ( "Photo %1 of %2", photoIndex + 1, QString::number ( d->m_photos.count() ) ) );
  }

// update the pages to be printed and preview first/last pages
  void Wizard::previewPhotos()
  {
    // get the selected layout
    int curr = d->mPhotoPage->ListPhotoSizes->currentRow();
    TPhotoSize *s = d->m_photoSizes.at ( curr );

    int photoCount  =  d->m_photos.count();
    // how many pages?  Recall that the first layout item is the paper size
    int photosPerPage = s->layouts.count() - 1;
    int remainder = photoCount % photosPerPage;
    int emptySlots = 0;
    if ( remainder > 0 )
      emptySlots = photosPerPage - remainder;
    int pageCount = photoCount / photosPerPage;
    if ( emptySlots > 0 )
      pageCount++;

    d->mPhotoPage->LblPhotoCount->setText ( QString::number ( photoCount ) );
    d->mPhotoPage->LblSheetsPrinted->setText ( QString::number ( pageCount ) );
    d->mPhotoPage->LblEmptySlots->setText ( QString::number ( emptySlots ) );

    // photo previews
    // preview the first page.
    // find the first page of photos
    int count = 0;
    int page = 0;
    int current = 0;

    QList<TPhoto*>::iterator it;
    for ( it = d->m_photos.begin(); it != d->m_photos.end(); ++it )
    {
      TPhoto *photo = static_cast<TPhoto*> ( *it );

      if ( page == d->m_currentPreviewPage )
      {
        photo->cropRegion.setRect ( -1, -1, -1, -1 );
        photo->rotation = 0;
        int w = s->layouts.at ( count+1 )->width();
        int h = s->layouts.at ( count+1 )->height();
        d->mCropPage->cropFrame->init ( photo, w, h, s->autoRotate, false );
      }
      count++;
      if ( count >= photosPerPage )
      {
        if ( page == d->m_currentPreviewPage )
          break;
        page++;
        current += photosPerPage;
        count = 0;
      }
    }

    // send this photo list to the painter
    QImage img ( d->mPhotoPage->BmpFirstPagePreview->size(), QImage::Format_ARGB32_Premultiplied );
    QPainter p ( &img );
    p.setCompositionMode( QPainter::CompositionMode_Destination );
    p.fillRect ( img.rect(), Qt::transparent );
    p.setCompositionMode( QPainter::CompositionMode_SourceOver );
    paintOnePage ( p, d->m_photos, s->layouts, d->mInfoPage->m_captions-> currentIndex(), current, true );
    p.end();
    d->mPhotoPage->BmpFirstPagePreview->setPixmap ( QPixmap::fromImage(img) );
    d->mPhotoPage->LblPreview->setText ( i18n ( "Page %1 of %2", d->m_currentPreviewPage + 1, getPageCount() ) );

    manageBtnPreviewPage();
    manageBtnPrintOrder();
  }

  void Wizard::manageBtnPreviewPage()
  {
    d->mPhotoPage->BtnPreviewPageDown->setEnabled ( true );
    d->mPhotoPage->BtnPreviewPageUp->setEnabled ( true );
    if ( d->m_currentPreviewPage == 0 )
    {
      d->mPhotoPage->BtnPreviewPageDown->setEnabled ( false );
    }

    if ( ( d->m_currentPreviewPage + 1 ) == getPageCount() )
    {
      d->mPhotoPage->BtnPreviewPageUp->setEnabled ( false );
    }
  }

  void Wizard::manageBtnPrintOrder()
  {
    if ( d->mPhotoPage->ListPrintOrder->currentRow() == -1 )
      return;

    d->mPhotoPage->BtnPrintOrderDown->setEnabled ( true );
    d->mPhotoPage->BtnPrintOrderUp->setEnabled ( true );
    if ( d->mPhotoPage->ListPrintOrder->currentRow() == 0 )
    {
      d->mPhotoPage->BtnPrintOrderUp->setEnabled ( false );
    }
    if ( ( d->mPhotoPage->ListPrintOrder->currentRow() + 1 ) == d->mPhotoPage->ListPrintOrder->count() )
    {
      d->mPhotoPage->BtnPrintOrderDown->setEnabled ( false );
    }
  }

// Wizard SLOTS
  void Wizard::slotHelp()
  {
    KToolInvocation::invokeHelp ( "printwizard","kipi-plugins" );
  }


  void Wizard::btnBrowseOutputPathClicked ( void )
  {
    QString newPath = KFileDialog::getExistingDirectory ( d->mInfoPage->EditOutputPath->text(), this,
                      i18n ( "Select Output Folder" ) );
    if ( newPath.isEmpty() )
      return;
    // assume this directory exists
    d->mInfoPage->EditOutputPath->setText ( newPath );
    outputSettingsClicked ( d->m_outputSettings->checkedId () );
  }

  void Wizard::paperSizeChanged ( int index )
  {
    PageSize pageSize = ( PageSize ) index;
    initPhotoSizes ( pageSize );

    if ( pageSize > A6 )
    {
      KMessageBox::information ( this,
                                 i18n ( "Do not forget to set the correct page size according to your printer settings." ),
                                 i18n ( "Page size settings" ), "pageSizeInfo" );
    }
  }

  void  Wizard::pageChanged ( KPageWidgetItem *current, KPageWidgetItem *before )
  {
    if ( before )
    {
      saveSettings ( before->name() );
      kDebug(51000) << " before " << before->name();
    }
    if ( current )
    {
      readSettings ( current->name() );
      kDebug(51000) << " current " << current->name();
    }
    if ( current->name() == i18n ( introPageName ) )
    {
    }
    else if ( current->name() == i18n ( infoPageName ) )
    {
#ifdef NOT_YET
      if ( d->mInfoPage->GrpOutputSettings->id ( RdoOutputPrinter ) )
        this->nextButton()->setEnabled ( true );
      else if ( id == GrpOutputSettings->id ( RdoOutputFile ) )
      {
        if ( !EditOutputPath->text().isEmpty() )
        {
          QFileInfo fileInfo ( EditOutputPath->text() );
          if ( fileInfo.exists() && fileInfo.isDir() )
            this->nextButton()->setEnabled ( true );
        }
      }
      else
        if ( id == GrpOutputSettings->id ( RdoOutputGimp ) )
        {
          this->nextButton()->setEnabled ( true );
        }
      kDebug(51000) << "CCCC" << endl;
#endif
    }
    else if ( current->name() == i18n ( photoPageName ) )
    {
      // create our photo sizes list
      initPhotoSizes ( d->m_pageSize );
      previewPhotos();
    }
    else if ( current->name() == i18n ( cropPageName ) )
    {
      d->m_currentCropPhoto = 0;
      TPhoto *photo = d->m_photos[d->m_currentCropPhoto];
      setBtnCropEnabled();
      updateCropFrame ( photo, d->m_currentCropPhoto );
    }

  }


  void Wizard::outputSettingsClicked ( int id )
  {
    if ( id == d->m_outputSettings->id ( d->mInfoPage->RdoOutputPrinter ) )
      this->setValid ( d->mCropPage->page(), true );
    else if ( id == d->m_outputSettings->id ( d->mInfoPage->RdoOutputFile ) )
    {
      if ( !d->mInfoPage->EditOutputPath->text().isEmpty() )
      {
        QFileInfo fileInfo ( d->mInfoPage->EditOutputPath->text() );
        if ( ! ( fileInfo.exists() && fileInfo.isDir() ) )
          this->setValid ( d->mCropPage->page(), false );
        else
          this->setValid ( d->mCropPage->page(), true );
      }
      else
        this->setValid ( d->mCropPage->page(), false );
    }
    else if ( id == d->m_outputSettings->id ( d->mInfoPage->RdoOutputGimp ) )
    {
      this->setValid ( d->mCropPage->page(), true );
    }
  }

  void Wizard::captionChanged ( const QString & text )
  {
    //TODO use QVariant and add them by hands
    bool fontSettingsEnabled;
    if ( text == i18n ( "No captions" ) )
    {
      fontSettingsEnabled = false;
      d->mInfoPage->m_FreeCaptionFormat->setEnabled ( false );
      d->mInfoPage->m_free_label->setEnabled ( false );
    }
    else if ( text == i18n ( "Free" ) )
    {
      fontSettingsEnabled = true;
      d->mInfoPage->m_FreeCaptionFormat->setEnabled ( true );
      d->mInfoPage->m_free_label->setEnabled ( true );
    }
    else
    {
      fontSettingsEnabled = true;
      d->mInfoPage->m_FreeCaptionFormat->setEnabled ( false );
      d->mInfoPage->m_free_label->setEnabled ( false );
    }
    d->mInfoPage->m_font_name->setEnabled ( fontSettingsEnabled );
    d->mInfoPage->m_font_size->setEnabled ( fontSettingsEnabled );
    d->mInfoPage->m_font_color->setEnabled ( fontSettingsEnabled );
  }

  void Wizard::BtnCropRotate_clicked()
  {
    // by definition, the cropRegion should be set by now,
    // which means that after our rotation it will become invalid,
    // so we will initialize it to -2 in an awful hack (this
    // tells the cropFrame to reset the crop region, but don't
    // automagically rotate the image to fit.
    TPhoto *photo = d->m_photos[d->m_currentCropPhoto];
    photo->cropRegion = QRect ( -2, -2, -2, -2 );
    photo->rotation = ( photo->rotation + 90 ) % 360;

    updateCropFrame ( photo, d->m_currentCropPhoto );

  }

  void Wizard::setBtnCropEnabled()
  {
    if ( d->m_currentCropPhoto == 0 )
      d->mCropPage->BtnCropPrev->setEnabled ( false );
    else
      d->mCropPage->BtnCropPrev->setEnabled ( true );

    if ( d->m_currentCropPhoto == ( int ) d->m_photos.count() - 1 )
      d->mCropPage->BtnCropNext->setEnabled ( false );
    else
      d->mCropPage->BtnCropNext->setEnabled ( true );
  }

  void Wizard::BtnCropNext_clicked()
  {
    TPhoto *photo = 0;
    photo = d->m_photos[++d->m_currentCropPhoto];
    setBtnCropEnabled();
    if ( photo == 0 )
    {
      d->m_currentCropPhoto = d->m_photos.count() - 1;
      return;
    }
    updateCropFrame ( photo, d->m_currentCropPhoto );
  }

  void Wizard::BtnCropPrev_clicked()
  {
    TPhoto *photo = 0;
    photo = d->m_photos[--d->m_currentCropPhoto];

    setBtnCropEnabled();

    if ( photo == 0 )
    {
      d->m_currentCropPhoto = 0;
      return;
    }
    updateCropFrame ( photo, d->m_currentCropPhoto );
  }

  void Wizard::BtnPrintOrderUp_clicked()
  {
    if ( d->mPhotoPage->ListPrintOrder->currentItem() == 0 )
      return;

    int currentIndex = d->mPhotoPage->ListPrintOrder->currentRow();

    kDebug ( ) << "Selected photo " << currentIndex+1 << " of " << d->mPhotoPage->ListPrintOrder->count();


    d->mPhotoPage->ListPrintOrder->blockSignals(true);
    // swap these items
    QListWidgetItem *item1 = d->mPhotoPage->ListPrintOrder->takeItem ( currentIndex - 1 );
    QListWidgetItem *item2 = d->mPhotoPage->ListPrintOrder->takeItem ( currentIndex - 1 );
    d->mPhotoPage->ListPrintOrder->insertItem ( currentIndex - 1, item1 );
    d->mPhotoPage->ListPrintOrder->insertItem ( currentIndex - 1, item2 );
    d->mPhotoPage->ListPrintOrder->blockSignals(false);

    // select picture again
    d->mPhotoPage->ListPrintOrder->setCurrentItem(item2);

    // the list box items are swapped, now swap the items in the photo list
    d->m_photos.swap ( currentIndex, currentIndex - 1 );
    /*
    TPhoto *photo1 = d->m_photos.at(currentIndex);
    TPhoto *photo2 = d->m_photos.at(currentIndex - 1);
    d->m_photos.remove(currentIndex - 1);
    d->m_photos.remove(currentIndex - 1);
    d->m_photos.insert(currentIndex - 1, photo2);
    d->m_photos.insert(currentIndex - 1, photo1);
    */
    previewPhotos();
  }


  void Wizard::EditCopies_valueChanged ( int copies )
  {
    if ( copies < 1 )
      return;

    int currentIndex = d->mPhotoPage->ListPrintOrder->currentRow();
    QString item = d->mPhotoPage->ListPrintOrder->currentItem()->text();
    TPhoto *pCurPhoto = d->m_photos.at ( currentIndex );
    KUrl fileName = pCurPhoto->filename;

    if ( pCurPhoto->copies >= copies )
    {
      // removing copies
      if ( pCurPhoto->copies == 1 || pCurPhoto->copies == copies )
        return;

      d->mPhotoPage->ListPrintOrder->blockSignals ( true );
      d->mPhotoPage->ListPrintOrder->setCurrentRow ( currentIndex, QItemSelectionModel::Deselect );
      for ( int removing = pCurPhoto->copies - copies; removing >0 ;removing-- )
      {
        for ( int index = 0; index < d->mPhotoPage->ListPrintOrder->count(); index++ )
        {
          if ( d->mPhotoPage->ListPrintOrder->item ( index )->text() == item )
          {
            TPhoto *pPhoto = d->m_photos.at ( index );
            d->m_photos.removeAt ( index );
            delete ( pPhoto );
            d->mPhotoPage->ListPrintOrder->takeItem ( index );
            break;
          }
        }
      }
      d->mPhotoPage->ListPrintOrder->blockSignals ( false );
      currentIndex = -1;
    }
    else
    {
      // adding copies
      for ( int adding = copies - pCurPhoto->copies; adding >0 ;adding-- )
      {
        TPhoto *pPhoto = new TPhoto ( 150 );
        pPhoto->filename = pCurPhoto->filename;
        d->m_photos.insert ( currentIndex, pPhoto );
        d->mPhotoPage->ListPrintOrder->insertItem ( currentIndex, pPhoto->filename.fileName() );
      }
    }

    d->mPhotoPage->LblPhotoCount->setText ( QString::number ( d->m_photos.count() ) );

    // TODO check if possible to use iterator
    for ( int index = 0;
            index < d->m_photos.count();
            ++index )
    {
      TPhoto *pPhoto = d->m_photos[index];
      if ( pPhoto == 0 )
        break;
      if ( pPhoto->filename == fileName )
      {
        pPhoto->copies = copies;
        if ( currentIndex == -1 )
          currentIndex = index;
      }
    }
    if ( currentIndex != -1 )
    {
      d->mPhotoPage->ListPrintOrder->blockSignals ( true );
      d->mPhotoPage->ListPrintOrder->setCurrentRow ( currentIndex, QItemSelectionModel::Select );
      d->mPhotoPage->ListPrintOrder->blockSignals ( false );
    }
    previewPhotos();
  }


  void Wizard::ListPhotoOrder_highlighted ( int index )
  {
    d->mPhotoPage->EditCopies->blockSignals ( true );
    d->mPhotoPage->EditCopies->setValue ( d->m_photos.at ( index )->copies );
    d->mPhotoPage->EditCopies->blockSignals ( false );

    manageBtnPrintOrder();
  }

  void Wizard::ListPrintOrder_selected()
  {
    int currentIndex = d->mPhotoPage->ListPrintOrder->currentRow();
    d->mPhotoPage->EditCopies->blockSignals ( true );
    d->mPhotoPage->EditCopies->setValue ( d->m_photos.at ( currentIndex )->copies );
    d->mPhotoPage->EditCopies->blockSignals ( false );

    manageBtnPrintOrder();
  }

  void Wizard::ListPhotoSizes_selected()
  {
    previewPhotos();
  }

  void Wizard::BtnPrintOrderDown_clicked()
  {
    int currentIndex = d->mPhotoPage->ListPrintOrder->currentRow();

    kDebug ( ) << "Selected photo " << currentIndex+1 << " of " << d->mPhotoPage->ListPrintOrder->count();

    if ( currentIndex == d->mPhotoPage->ListPrintOrder->count() - 1 )
      return;


    d->mPhotoPage->ListPrintOrder->blockSignals(true);
    // swap these items
    QListWidgetItem *item1 = d->mPhotoPage->ListPrintOrder->takeItem ( currentIndex );
    QListWidgetItem *item2 = d->mPhotoPage->ListPrintOrder->takeItem ( currentIndex );
    d->mPhotoPage->ListPrintOrder->insertItem ( currentIndex, item1 );
    d->mPhotoPage->ListPrintOrder->insertItem ( currentIndex, item2 );
    d->mPhotoPage->ListPrintOrder->blockSignals(false);

    // select picture again
    d->mPhotoPage->ListPrintOrder->setCurrentItem(item1);

    // the list box items are swapped, now swap the items in the photo list
    d->m_photos.swap ( currentIndex, currentIndex + 1 );

    previewPhotos();
  }

  void Wizard::BtnPreviewPageDown_clicked()
  {
    if ( d->m_currentPreviewPage == 0 )
      return;
    d->m_currentPreviewPage--;
    previewPhotos();
  }
  void Wizard::BtnPreviewPageUp_clicked()
  {
    if ( d->m_currentPreviewPage == getPageCount() - 1 )
      return;
    d->m_currentPreviewPage++;
    previewPhotos();
  }

  void Wizard::saveSettings ( QString pageName )
  {
    // Save the current settings
    KConfig config ( "kipirc" );
    KConfigGroup group = config.group ( QString ( "PrintAssistant" ) );


    if ( pageName == i18n ( introPageName ) )
    {
      // IntroPage
      //skip intro
      group.writeEntry ( "SkipIntro", d->mIntroPage->m_skipIntro->isChecked() );
    }
    else if ( pageName == i18n ( infoPageName ) )
    {
      // InfoPage

      // Page size
      group.writeEntry ( "PageSize", ( int ) d->m_pageSize );

      // Margins
      group.writeEntry ( "NoMargins", d->mInfoPage->m_fullbleed->isChecked() );

      // output
      int output = d->m_outputSettings->checkedId();
      if ( output == d->m_outputSettings->id ( d->mInfoPage->RdoOutputFile ) )
        output = ToFile;
      else if ( output == d->m_outputSettings->id ( d->mInfoPage->RdoOutputGimp ) )
        output = ToGimp;
      else
        output = ToPrinter;
      group.writeEntry ( "PrintOutput", output );

      // image captions
      group.writeEntry ( "Captions", d->mInfoPage->m_captions->currentIndex() );
      // caption color
      group.writeEntry ( "CaptionColor", d->mInfoPage->m_font_color->color() );
      // caption font
      group.writeEntry ( "CaptionFont", QFont ( d->mInfoPage->m_font_name->currentFont() ) );
      // caption size
      group.writeEntry ( "CaptionSize", d->mInfoPage->m_font_size->value() );
      // free caption
      group.writeEntry ( "FreeCaption", d->mInfoPage->m_FreeCaptionFormat->text() );

      // output path
      group.writePathEntry ( "OutputPath", d->mInfoPage->EditOutputPath->text() );

    }
    else if ( pageName == i18n ( photoPageName ) )
    {
      // PhotoPage
      // photo size
      group.writeEntry ( "PhotoSize", d->mPhotoPage->ListPhotoSizes->currentItem()->text() );
    }
    else if ( pageName == i18n ( cropPageName ) )
    {
      // CropPage
    }


#ifdef NOT_YET
    // kjobviewer
    config.writeEntry ( "KjobViewer", m_kjobviewer->isChecked() );
#endif //NOT_YET
  }

// global settings
  void Wizard::readSettings()
  {
    KConfig config ( "kipirc" );
    KConfigGroup group = config.group ( QString ( "PrintAssistant" ) );

    //skip intro
    d->mIntroPage->m_skipIntro->setChecked ( group.readEntry ( "SkipIntro", false ) );
  }

  void Wizard::readSettings ( QString pageName )
  {
    KConfig config ( "kipirc" );
    KConfigGroup group = config.group ( QString ( "PrintAssistant" ) );


    if ( pageName == i18n ( introPageName ) )
    {
      // IntroPage
      //skip intro
      d->mIntroPage->m_skipIntro->setChecked ( group.readEntry ( "SkipIntro", false ) );
    }
    else if ( pageName == i18n ( infoPageName ) )
    {
      // InfoPage
      //internal PageSize  - default A4
      PageSize pageSize = ( PageSize ) group.readEntry ( "PageSize", ( int ) A4 );
      initPhotoSizes ( pageSize );
      d->mInfoPage->CmbPaperSize->setCurrentIndex ( pageSize );

      // No Margins - default false
      d->mInfoPage->m_fullbleed->setChecked ( group.readEntry ( "NoMargins", false ) );

      // set the output
      int id = group.readEntry ( "PrintOutput", d->m_outputSettings->id ( d->mInfoPage->RdoOutputPrinter ) );
      if ( id == ToFile )
        d->mInfoPage->RdoOutputFile->setChecked ( true );
      else if ( id == ToGimp )
        d->mInfoPage->RdoOutputGimp->setChecked ( true );
      else
        d->mInfoPage->RdoOutputPrinter->setChecked ( true );

      // image captions
      d->mInfoPage->m_captions->setCurrentIndex ( group.readEntry ( "Captions", 0 ) );
      // caption color
      QColor defColor ( Qt::yellow );
      QColor color = group.readEntry ( "CaptionColor", defColor );
      d->mInfoPage->m_font_color->setColor ( color );
      // caption font
      QFont defFont ( "Sans Serif" );
      QFont font = group.readEntry ( "CaptionFont", defFont );
      d->mInfoPage->m_font_name->setCurrentFont ( font.family() );
      // caption size
      int fontSize = group.readEntry ( "CaptionSize", 4 );
      d->mInfoPage->m_font_size->setValue ( fontSize );
      // free caption
      QString captionTxt = group.readEntry ( "FreeCaption" );
      d->mInfoPage->m_FreeCaptionFormat->setText ( captionTxt );
      //enable right caption stuff
      captionChanged ( d->mInfoPage->m_captions->currentText() );

      // set the last output path
      QString outputPath = group.readPathEntry ( "OutputPath", d->mInfoPage->EditOutputPath->text() );
      d->mInfoPage->EditOutputPath->setText ( outputPath );
    }
    else if ( pageName == i18n ( photoPageName ) )
    {
      // PhotoPage
      // photo size
      QString photoSize = group.readEntry ( "PhotoSize" );
      QList<QListWidgetItem *> list = d->mPhotoPage->ListPhotoSizes->findItems ( photoSize, Qt::MatchExactly );
      if ( list.count() )
        d->mPhotoPage->ListPhotoSizes->setCurrentItem ( list[0] );
      else
        d->mPhotoPage->ListPhotoSizes->setCurrentRow ( 0 );
    }
    else if ( pageName == i18n ( cropPageName ) )
    {
      // CropPage
    }


#ifdef NOT_YET
    // kjobviewer
    m_kjobviewer->setChecked ( config.readBoolEntry ( "KjobViewer", true ) );
#endif //NOT_YET
  }

  void Wizard::printPhotos ( QList<TPhoto*> photos, QList<QRect*> layouts, QPrinter &printer )
  {
    d->m_cancelPrinting = false;
#ifdef NOT_YET
    LblPrintProgress->setText ( "" );
    PrgPrintProgress->setProgress ( 0 );
    PrgPrintProgress->setTotalSteps ( photos.count() );
#endif
    KApplication::kApplication()->processEvents();

    QPainter p;
    p.begin ( &printer );

    int current = 0;

    bool printing = true;
    while ( printing )
    {
      printing = paintOnePage ( p, photos, layouts, d->mInfoPage->m_captions->currentIndex(), current );
      if ( printing )
        printer.newPage();
#ifdef NOT_YET
      PrgPrintProgress->setProgress ( current );
#endif
      KApplication::kApplication()->processEvents();
      if ( d->m_cancelPrinting )
      {
        printer.abort();
        return;
      }
    }
    p.end();

#ifdef NOT_YET
    if ( m_kjobviewer->isChecked() )
      if ( !m_Proc->start() )
        kDebug ( 51000 ) << "Error running kjobviewr\n";
    LblPrintProgress->setText ( i18n ( "Complete. Click Finish to exit the Print Wizard." ) );
#endif
  }

  QStringList Wizard::printPhotosToFile ( QList<TPhoto*> photos, QString &baseFilename, TPhotoSize* layouts )
  {
    Q_ASSERT ( layouts->layouts.count() > 1 );

    d->m_cancelPrinting = false;
#ifdef NOT_YET
    LblPrintProgress->setText ( "" );
    PrgPrintProgress->setProgress ( 0 );
    PrgPrintProgress->setTotalSteps ( photos.count() );
#endif
    KApplication::kApplication()->processEvents();

    int current = 0;
    int pageCount = 1;
    bool printing = true;
    QStringList files;

    QRect *srcPage = layouts->layouts.at ( 0 );

    while ( printing )
    {
      // make a pixmap to save to file.  Make it just big enough to show the
      // highest-dpi image on the page without losing data.
      double dpi = layouts->dpi;
      if ( dpi == 0.0 )
        dpi = getMaxDPI ( photos, layouts->layouts, current ) * 1.1;
      int w = NINT ( srcPage->width() / 1000.0 * dpi );
      int h = NINT ( srcPage->height()  / 1000.0 * dpi );
      QImage *img = new QImage ( w, h, QImage::Format_RGB32 );
      if ( !img )
        break;

      // save this page out to file
      QString filename = baseFilename + QString::number ( pageCount ) + ".jpeg";
      bool saveFile = true;
      if ( QFile::exists ( filename ) )
      {
        int result = KMessageBox::warningYesNoCancel ( this,
                     i18n ( "The following file will be overwritten. Are you sure you want to overwrite it?" ) +
                     "\n\n" + filename );
        if ( result == KMessageBox::No )
          saveFile = false;
        else if ( result == KMessageBox::Cancel )
        {
          delete img;
          break;
        }
      }

      // paint this page, even if we aren't saving it to keep the page
      // count accurate.
      printing = paintOnePage ( *img, photos, layouts->layouts, d->mInfoPage->m_captions->currentIndex(), current );

      if ( saveFile )
      {
        files.append ( filename );
        img->save ( filename, "JPEG" );
      }
      delete img;
      pageCount++;

#ifdef NOT_YET
      PrgPrintProgress->setProgress ( current );
#endif
      KApplication::kApplication()->processEvents();
      if ( d->m_cancelPrinting )
        break;
    }

#ifdef NOT_YET
    // did we cancel?
    if ( printing )
      LblPrintProgress->setText ( i18n ( "Printing Canceled." ) );
    else
    {
      if ( m_kjobviewer->isChecked() )
        if ( !m_Proc->start() )
          kDebug ( 51000 ) << "Error launching kjobviewr\n";
      LblPrintProgress->setText ( i18n ( "Complete. Click Finish to exit the Print Wizard." ) );
    }
#endif
    return files;
  }

  void Wizard::removeGimpFiles()
  {
    for ( QStringList::ConstIterator it = d->m_gimpFiles.constBegin(); it != d->m_gimpFiles.constEnd(); ++it )
    {
      if ( QFile::exists ( *it ) )
      {
        if ( QFile::remove ( *it ) == false )
        {
          KMessageBox::sorry ( this, i18n ( "Could not remove the GIMP's temporary files." ) );
          break;
        }
      }
    }
  }

//TODO not needed at the moment maybe we can remove it
  void Wizard::PageRemoved ( KPageWidgetItem *page )
  {
    kDebug(51000) << page->name();
  }

  void Wizard::crop_selection ( int )
  {
    d->mCropPage->cropFrame->drawCropRectangle ( !d->mCropPage->m_disableCrop->isChecked() );
    update();
  }

// this is called when Cancel is clicked.
  void Wizard::reject()
  {
    d->m_cancelPrinting = true;
    if ( d->m_gimpFiles.count() > 0 )
      removeGimpFiles();
    QDialog::reject();
  }

  /**
   *
   */
  void Wizard::accept()
  {
    // set the default crop regions if not already set
    TPhotoSize *s = d->m_photoSizes.at ( d->mPhotoPage->ListPhotoSizes->currentRow() );
    QList<TPhoto*>::iterator it;
    int i = 0;
    for ( it = d->m_photos.begin(); it != d->m_photos.end(); ++it )
    {
      TPhoto *photo = static_cast<TPhoto*> ( *it );
      if ( photo && photo->cropRegion == QRect ( -1, -1, -1, -1 ) )
        d->mCropPage->cropFrame->init ( photo, getLayout ( i )->width(),
                                        getLayout ( i )->height(), s->autoRotate );
      i++;
    }

    if ( d->mInfoPage->RdoOutputPrinter->isChecked() )
    {
      QPrinter printer;
      std::auto_ptr<QPrintDialog> dialog ( KdePrint::createPrintDialog ( &printer, this ) );
      dialog->setWindowTitle ( i18n ( "Print Image" ) );
      dialog->addEnabledOption ( QAbstractPrintDialog::PrintShowPageSize );

      switch ( d->m_pageSize )
      {
        case Letter :
          printer.setPaperSize ( QPrinter::Letter );
          break;
        case A4 :
          printer.setPaperSize ( QPrinter::A4 );
          break;
        case A6 :
          printer.setPaperSize ( QPrinter::A6 );
          break;
        default:
          break;
      }

      kDebug(51000) << " page size " << d->m_pageSize
      << " printer: " << printer.paperSize() << " A6: " << QPrinter::A6 << endl;

      if ( d->mInfoPage->m_fullbleed->isChecked() )
      {
        printer.setFullPage ( true );
        printer.setPageMargins ( 0, 0, 0, 0, QPrinter::Millimeter );
      }

      bool wantToPrint = dialog->exec();
      kDebug(51000) << "full page " << printer.fullPage() ;

      if ( !wantToPrint )
      {
        return;
      }

      printPhotos ( d->m_photos, s->layouts, printer );
      kDebug(51000) << "paper page " << dialog->printer()->paperSize() ;
    }
    else if ( d->mInfoPage->RdoOutputFile->isChecked() )
    {
      // now output the items
      QString path = d->mInfoPage->EditOutputPath->text();
      if ( path.right ( 1 ) != "/" )
        path = path + "/";
      path = path + "kipi_printassistant_";
      printPhotosToFile ( d->m_photos, path, s );
    }
    else if ( d->mInfoPage->RdoOutputGimp->isChecked() )
    {
      // now output the items
      QString path = d->m_tempPath;
      if ( !checkTempPath ( this, path ) )
        return;
      path = path + "kipi_tmp_";
      if ( d->m_gimpFiles.count() > 0 )
        removeGimpFiles();

      d->m_gimpFiles = printPhotosToFile ( d->m_photos, path, s );
      QStringList args;
      QString prog = "gimp-remote";
      for ( QStringList::ConstIterator it = d->m_gimpFiles.constBegin(); it != d->m_gimpFiles.constEnd(); ++it )
        args << ( *it );
      if ( !launchExternalApp ( prog, args ) )
      {
        KMessageBox::sorry ( this,
                             i18n ( "There was an error launching the GIMP. Please make sure it is properly installed." ),
                             i18n ( "KIPI" ) );
        return;
      }
    }

//   if (d->m_gimpFiles.count() > 0)
//     removeGimpFiles();

    saveSettings ( currentPage ()->name() );

    KAssistantDialog::accept();
  }

} // namespace
