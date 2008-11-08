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

// Qt includes
#include <QFileInfo>
#include <QPainter>
#include <QPalette>
#include <QtGlobal>

// KDE includes
#include <kapplication.h>
#include <kconfigdialogmanager.h>
#include <khelpmenu.h>
#include <kmenu.h>
#include <kpushbutton.h>
#include <ktoolinvocation.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
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

namespace KIPIPrintWizardPlugin {


template <class Ui_Class>
class WizardPage : public QWidget, public Ui_Class {
  public:
    WizardPage(KAssistantDialog* dialog, const QString& title)
    : QWidget(dialog) {
        setupUi(this);
        layout()->setMargin(0);
        mPage = dialog->addPage(this, title);
    }

    KPageWidgetItem* page() const {
        return mPage;
    }

  private:
    KPageWidgetItem* mPage;
};


typedef WizardPage<Ui_IntroPage> IntroPage;
typedef WizardPage<Ui_InfoPage>  InfoPage;
typedef WizardPage<Ui_PhotoPage> PhotoPage;
typedef WizardPage<Ui_CropPage>  CropPage;


struct Wizard::Private {
  KConfigDialogManager* mConfigManager;

  IntroPage *mIntroPage;
  InfoPage  *mInfoPage;
  PhotoPage *mPhotoPage;
  CropPage  *mCropPage;

  KPushButton  *m_helpButton;
  QButtonGroup *m_outputSettings;

  KIPI::ImageCollectionSelector* mCollectionSelector;
  KIPI::Interface* mInterface;

  KIPIPlugins::KPAboutData* mAbout;

  PageSize m_pageSize;
  QList<TPhoto*> m_photos;
  QList<TPhotoSize*> m_photoSizes;
  int m_currentPreviewPage;
  int m_currentCropPhoto;
};

enum OutputIDs
{
  RDO_BTN_PRINTER,
  RDO_BTN_FILE,
  RDO_BTN_GIMP
};

Wizard::Wizard(QWidget* parent, KIPI::Interface* interface)
: KAssistantDialog(parent)
{
  d=new Private;
  d->mInterface = interface;

  // About data
  d->mAbout =new KIPIPlugins::KPAboutData(ki18n("Print Wizard"),
                                  QByteArray(),
                                    KAboutData::License_GPL,
                                    ki18n("A KIPI plugin to print images"),
                                    ki18n("(c) 2003-2004, Todd Shoemaker\n(c) 2007-2008, Angelo Naselli"));

  d->mAbout->addAuthor(ki18n("Todd Shoemaker"), ki18n("Author"),
                     "todd@theshoemakers.net");
  d->mAbout->addAuthor(ki18n("Angelo Naselli"), ki18n("Developer and maintainer"),
                     "anaselli@linux.it");
  d->mAbout->addAuthor(ki18n("Valerio Fuoglio"), ki18n("Contributor"),
                     "valerio.fuoglio@gmail.com");

  d->mIntroPage = new IntroPage(this, i18n("Introduction"));
  d->mInfoPage  = new InfoPage(this, i18n("Select printing information"));
  d->mPhotoPage = new PhotoPage(this, i18n("Photo information"));
  d->mCropPage  = new CropPage(this, i18n("Crop photos")) ;

  d->m_helpButton = button (Help);
  KHelpMenu* helpMenu = new KHelpMenu(this, d->mAbout, false);
  helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
  QAction *handbook       = new QAction(i18n("Plugin Handbook"), this);

  // create a QButtonGroup to manage button ids
  d->m_outputSettings = new QButtonGroup(this);
  d->m_outputSettings->addButton(d->mInfoPage->RdoOutputPrinter, RDO_BTN_PRINTER);
  d->m_outputSettings->addButton(d->mInfoPage->RdoOutputGimp,    RDO_BTN_GIMP);
  d->m_outputSettings->addButton(d->mInfoPage->RdoOutputFile,    RDO_BTN_FILE);

  d->m_pageSize = Unknown; // select a different page to force a refresh in initPhotoSizes.

  // connections
  // help
  connect(handbook, SIGNAL(triggered(bool)),
          this, SLOT(slotHelp()));

  // selected page
  connect(this, SIGNAL(currentPageChanged (KPageWidgetItem *, KPageWidgetItem *)),
          this, SLOT(pageChanged(KPageWidgetItem *)));

  // caption information
  connect(d->mInfoPage->m_captions, SIGNAL(activated(const QString & )),
          this, SLOT(captionChanged(const QString &)));

  // Output
  connect(d->m_outputSettings, SIGNAL(buttonClicked(int)),
          this, SLOT(outputSettingsClicked(int)));

  // Browse path button
  connect(d->mInfoPage->BtnBrowseOutputPath, SIGNAL(clicked(void)),
          this, SLOT(btnBrowseOutputPathClicked(void)));

  // Paper Size
  connect(d->mInfoPage->CmbPaperSize, SIGNAL(activated(int)),
          this, SLOT(paperSizeChanged(int)));

  // Print order (down)
  connect(d->mPhotoPage->BtnPrintOrderDown, SIGNAL(clicked(void)),
          this, SLOT(printOrderDownClicked(void)));

  // Print order (up)
  connect(d->mPhotoPage->BtnPrintOrderUp, SIGNAL(clicked(void)),
          this, SLOT(BtnPrintOrderUp_clicked(void)));
  
  connect(d->mPhotoPage->BtnPreviewPageUp, SIGNAL(clicked(void)),
          this, SLOT(BtnPreviewPageUp_clicked(void)));
  connect(d->mPhotoPage->BtnPreviewPageDown, SIGNAL(clicked(void)),
          this, SLOT(BtnPreviewPageDown_clicked(void)));

  connect(d->mCropPage->BtnCropPrev, SIGNAL(clicked()),
          this, SLOT(BtnCropPrev_clicked()));
  connect(d->mCropPage->BtnCropNext, SIGNAL(clicked()),
          this, SLOT(BtnCropNext_clicked()));

  connect(d->mCropPage->BtnCropRotate, SIGNAL(clicked()),
          this, SLOT(BtnCropRotate_clicked()));

  // Default is A4
  d->mInfoPage->CmbPaperSize->setCurrentIndex(A4);
  initPhotoSizes(A4);   // default to A4 for now.
  d->m_currentPreviewPage = 0;
  d->m_currentCropPhoto   = 0;

  helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
  d->m_helpButton->setDelayedMenu( helpMenu->menu() );

  //TODO
	//updateFinishButton();
}

Wizard::~Wizard() {
  delete d->mAbout;
  delete d;
}

// create a MxN grid of photos, fitting on the page
TPhotoSize * createPhotoGrid(int pageWidth, int pageHeight, QString label, int rows, int columns) {
  int MARGIN = (int)((pageWidth + pageHeight) / 2 * 0.04 + 0.5); 
  int GAP = MARGIN / 4;
  int photoWidth = (pageWidth - (MARGIN * 2) - ((columns-1) * GAP)) / columns;
  int photoHeight = (pageHeight - (MARGIN * 2) - ((rows-1) * GAP)) / rows;

  TPhotoSize *p = new TPhotoSize;
  p->label = label;
  p->dpi = 100;
  p->autoRotate = false;
  p->layouts.append(new QRect(0, 0, pageWidth, pageHeight));

  int row = 0;
  for(int y=MARGIN; row < rows && y < pageHeight - MARGIN; y += photoHeight + GAP) {
    int col = 0;
    for(int x=MARGIN; col < columns && x < pageWidth - MARGIN; x += photoWidth + GAP) {
      p->layouts.append(new QRect(x, y, photoWidth, photoHeight));
      col++;
    }
    row++;
  }
  return p;
}

// TODO page layout configurable (using XML?)
void Wizard::initPhotoSizes(PageSize pageSize)
{
        // don't refresh anything if we haven't changed page sizes.
  if (pageSize == d->m_pageSize)
    return;

  d->m_pageSize = pageSize;

  // cleanng m_pageSize memory before invoking clear()
  for(int i=0; i < d->m_photoSizes.count(); i++)
    if (d->m_photoSizes.at(i))
      delete d->m_photoSizes.at(i);
  d->m_photoSizes.clear();

  switch (pageSize)
  {
                // ====================== LETTER SIZE =====================
    case Letter:
    {
      TPhotoSize *p;
                        // ========== 5 x 3.5
      p = new TPhotoSize;
      p->label = i18n("3.5 x 5\"");
      p->dpi = 0;
      p->autoRotate = true;
                        // page size
      p->layouts.append(new QRect(0, 0, 8500, 11000));
                        // photo layouts
      p->layouts.append(new QRect( 700,  500, 3500, 5000));
      p->layouts.append(new QRect(4300,  500, 3500, 5000));
      p->layouts.append(new QRect( 700, 5600, 3500, 5000));
      p->layouts.append(new QRect(4300, 5600, 3500, 5000));
                        // add to the list
      d->m_photoSizes.append(p);

                        // ========== 4 x 6
      p = new TPhotoSize;
      p->label = i18n("4 x 6\"");
      p->dpi = 0;
      p->autoRotate = true;
                        // page size
      p->layouts.append(new QRect(0, 0, 8500, 11000));
                        // photo layouts
      p->layouts.append(new QRect( 225,  500, 4000, 6000));
      p->layouts.append(new QRect(4275,  500, 4000, 6000));
      p->layouts.append(new QRect(1250, 6600, 6000, 4000));
                        // add to the list
      d->m_photoSizes.append(p);

                        // ========== 4 x 6 Album
      p = new TPhotoSize;
      p->label = i18n("4 x 6\" Album");
      p->dpi = 0;
      p->autoRotate = true;
                        // page size
      p->layouts.append(new QRect(0, 0, 8500, 11000));
                        // photo layouts
      p->layouts.append(new QRect( 1250,  1000, 6000, 4000));
      p->layouts.append(new QRect( 1250,  6000, 6000, 4000));
                        // add to the list
      d->m_photoSizes.append(p);

                        // ========== 5 x 7
      p = new TPhotoSize;
      p->label = i18n("5 x 7\"");
      p->dpi = 0;
      p->autoRotate = true;
                        // page size
      p->layouts.append(new QRect(0, 0, 8500, 11000));
                        // photo layouts
      p->layouts.append(new QRect( 750,  500, 7000, 5000));
      p->layouts.append(new QRect( 750, 5750, 7000, 5000));
                        // add to the list
      d->m_photoSizes.append(p);

                        // ========== 8 x 10
      p = new TPhotoSize;
      p->label = i18n("8 x 10\"");
      p->dpi = 0;
      p->autoRotate = true;
                        // page size
      p->layouts.append(new QRect(0, 0, 8500, 11000));
                        // photo layouts
      p->layouts.append(new QRect(250, 500, 8000, 10000));
                        // add to the list
      d->m_photoSizes.append(p);

                        // thumbnails
      d->m_photoSizes.append(createPhotoGrid(8500, 11000, i18n("Thumbnails"), 5, 4));

                        // small thumbnails
      d->m_photoSizes.append(createPhotoGrid(8500, 11000, i18n("Small Thumbnails"), 6, 5));

                        // album collage 1
      p = new TPhotoSize;
      p->label = i18n("Album Collage 1 (9 photos)");
      p->dpi = 0;
      p->autoRotate = false;
                        // page size
      p->layouts.append(new QRect(0, 0, 8500, 11000));
                        // photo layouts
                        // photo 1 is in the center, 3x4.5
      p->layouts.append(new QRect(2750, 3250, 3000, 4500));
                        // the remaining 1.5x2 photos begin with upper left and circle around
                        // top row
      p->layouts.append(new QRect(750, 750, 1500, 2000));
      p->layouts.append(new QRect(3500, 750, 1500, 2000));
      p->layouts.append(new QRect(6250, 750, 1500, 2000));
      p->layouts.append(new QRect(6250, 4500, 1500, 2000));
      p->layouts.append(new QRect(6250, 8250, 1500, 2000));
      p->layouts.append(new QRect(3500, 8250, 1500, 2000));
      p->layouts.append(new QRect(750, 8250, 1500, 2000));
      p->layouts.append(new QRect(750, 4500, 1500, 2000));
      d->m_photoSizes.append(p);

                        // album collage 2
      p = new TPhotoSize;
      p->label = i18n("Album Collage 2 (6 photos)");
      p->dpi = 0;
      p->autoRotate = false;
                        // page size
      p->layouts.append(new QRect(0, 0, 8500, 11000));
                        // photo layouts
      p->layouts.append(new QRect(1000, 1000, 3000, 3000));
      p->layouts.append(new QRect(5000, 1000, 2500, 1250));
      p->layouts.append(new QRect(5000, 2750, 2500, 1250));
      p->layouts.append(new QRect(1000, 5000, 1500, 2000));
      p->layouts.append(new QRect(2750, 5000, 4750, 2000));
      p->layouts.append(new QRect(1000, 8000, 6500, 2000));
      d->m_photoSizes.append(p);
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
      p->label = i18n("21 x 29.7cm");
                        // page size
      p->layouts.append(new QRect(0, 0, 2100, 2970));
                        // photo layouts
      p->layouts.append(new QRect( 0, 0, 2100, 2970));
                        // add to the list
      d->m_photoSizes.append(p);

                        // ========== 6x9 cm - 8 photos
      p = new TPhotoSize;
      p->dpi = 0;
      p->autoRotate = true;
      p->label = i18n("6 x 9cm (8 photos)");
                        // page size
      p->layouts.append(new QRect(0, 0, 2100, 2970));
                        // photo layouts
      p->layouts.append(new QRect( 100,  100, 900, 600));
      p->layouts.append(new QRect(1100,  100, 900, 600));
      p->layouts.append(new QRect( 100,  800, 900, 600));
      p->layouts.append(new QRect(1100,  800, 900, 600));
      p->layouts.append(new QRect( 100, 1500, 900, 600));
      p->layouts.append(new QRect(1100, 1500, 900, 600));
      p->layouts.append(new QRect( 100, 2200, 900, 600));
      p->layouts.append(new QRect(1100, 2200, 900, 600));
                        // add to the list
      d->m_photoSizes.append(p);

                        // ========== 9x13
      p = new TPhotoSize;
      p->dpi = 0;
      p->autoRotate = true;
      p->label = i18n("9 x 13cm");
                        // page size
      p->layouts.append(new QRect(0, 0, 2100, 2970));
                        // photo layouts
      p->layouts.append(new QRect( 100,  100, 900, 1300));
      p->layouts.append(new QRect(1100,  100, 900, 1300));
      p->layouts.append(new QRect( 100, 1500, 900, 1300));
      p->layouts.append(new QRect(1100, 1500, 900, 1300));
                        // add to the list
      d->m_photoSizes.append(p);
                        
                          // ========== 10x13.33cm
      p = new TPhotoSize;
      p->dpi = 0;
      p->autoRotate = true;
      p->label = i18n("10 x 13.33cm");
                        // page size
      p->layouts.append(new QRect(0, 0, 2100, 2970));
                        // photo layouts
      p->layouts.append(new QRect( 50,  100, 1000, 1333));
      p->layouts.append(new QRect(1060,  100, 1000, 1333));
      p->layouts.append(new QRect( 50, 1500, 1000, 1333));
      p->layouts.append(new QRect(1060, 1500, 1000, 1333));
                        // add to the list
      d->m_photoSizes.append(p);

                        // ========== 10x15cm
      p = new TPhotoSize;
      p->dpi = 0;
      p->autoRotate = true;
      p->label = i18n("10 x 15cm");
                        // page size
      p->layouts.append(new QRect(0, 0, 2100, 2970));
                        // photo layouts
      p->layouts.append(new QRect(  50,  150, 1000, 1500));
      p->layouts.append(new QRect(1060,  150, 1000, 1500));
      p->layouts.append(new QRect( 300, 1750, 1500, 1000));
                        // add to the list
      d->m_photoSizes.append(p);

                        // ========== 10x15cm album
      p = new TPhotoSize;
      p->dpi = 0;
      p->autoRotate = true;
      p->label = i18n("10 x 15cm Album");
                        // page size
      p->layouts.append(new QRect(0, 0, 2100, 2970));
                        // photo layouts
      p->layouts.append(new QRect( 300, 350, 1500, 1000));
      p->layouts.append(new QRect( 300, 1620, 1500, 1000));
                        // add to the list
      d->m_photoSizes.append(p);

                        // ========== 11.5x15cm album
      p = new TPhotoSize;
      p->dpi = 0;
      p->autoRotate = true;
      p->label = i18n("11.5 x 15cm Album");
                        // page size
      p->layouts.append(new QRect(0, 0, 2100, 2970));
                        // photo layouts
      p->layouts.append(new QRect( 300, 250, 1500, 1100));
      p->layouts.append(new QRect( 300, 1570, 1500, 1100));
                        // add to the list
      d->m_photoSizes.append(p);

                        // ========== 13x18cm
      p = new TPhotoSize;
      p->dpi = 0;
      p->autoRotate = true;
      p->label = i18n("13 x 18cm");
                        // page size
      p->layouts.append(new QRect(0, 0, 2100, 2970));
                        // photo layouts
      p->layouts.append(new QRect( 150, 150, 1800, 1300));
      p->layouts.append(new QRect( 150, 1520, 1800, 1300));
                        // add to the list
      d->m_photoSizes.append(p);

                        // ========== 20x25cm
      p = new TPhotoSize;
      p->dpi = 0;
      p->autoRotate = true;
      p->label = i18n("20 x 25cm");
                        // page size
      p->layouts.append(new QRect(0, 0, 2100, 2970));
                        // photo layouts
      p->layouts.append(new QRect( 50, 230, 2000, 2500));
                        // add to the list
      d->m_photoSizes.append(p);

                        // thumbnails
      d->m_photoSizes.append(createPhotoGrid(2100, 2970, i18n("Thumbnails"), 5, 4));

                        // small thumbnails
      d->m_photoSizes.append(createPhotoGrid(2100, 2970, i18n("Small Thumbnails"), 6, 5));
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
      p->label = i18n("9 x 13cm");
                        // page size
      p->layouts.append(new QRect(0, 0, 1050, 1480));
                        // photo layouts
      p->layouts.append(new QRect( 50,  100, 900, 1300));
                        // add to the list
      d->m_photoSizes.append(p);

                        // ========== 10x15cm
      p = new TPhotoSize;
      p->dpi = 0;
      p->autoRotate = true;
      p->label = i18n("10.5 x 14.8cm");
                        // page size
      p->layouts.append(new QRect(0, 0, 1050, 1480));
                        // photo layouts
      p->layouts.append(new QRect(0, 0, 1050, 1480));
                        // add to the list
      d->m_photoSizes.append(p);

                        // thumbnails
      d->m_photoSizes.append(createPhotoGrid(1050, 1480, i18n("Thumbnails"), 5, 4));

                        // small thumbnails
      d->m_photoSizes.append(createPhotoGrid(1050, 1480, i18n("Small Thumbnails"), 6, 5));
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
      p->label = i18n("10 x 15cm");
                        // page size
      p->layouts.append(new QRect(0, 0, 1016, 1524));
                        // photo layouts
      p->layouts.append(new QRect(0, 0, 1016, 1524));
                        // add to the list
      d->m_photoSizes.append(p);

                        // ========== 9x13
      p = new TPhotoSize;
      p->dpi = 0;
      p->autoRotate = true;
      p->label = i18n("9 x 13cm");
                        // page size
      p->layouts.append(new QRect(0, 0, 1016, 1524));
                        // photo layouts
      p->layouts.append(new QRect( 50,  100, 900, 1300));
                        // add to the list
      d->m_photoSizes.append(p);
                                                                                                                                
                        // thumbnails
      d->m_photoSizes.append(createPhotoGrid(1016, 1524, i18n("Thumbnails"), 5, 4));
                
                        // small thumbnails
      d->m_photoSizes.append(createPhotoGrid(1016, 1524, i18n("Small Thumbnails"), 6, 5));
                
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
      p->label = i18n("13 x 18cm");
                        // page size
      p->layouts.append(new QRect(0, 0, 1270, 1778));
                        // photo layouts
      p->layouts.append(new QRect(0, 0, 1270, 1778));
                        // add to the list
      d->m_photoSizes.append(p);

                        // ========== 10x15cm
      p = new TPhotoSize;
      p->dpi = 0;
      p->autoRotate = true;
      p->label = i18n("10 x 15cm");
                        // page size
      p->layouts.append(new QRect(0, 0, 1270, 1778));
                        // photo layouts
      p->layouts.append(new QRect(0, 0, 1016, 1524));
                        // add to the list
      d->m_photoSizes.append(p);

                        // ========== 9x13
      p = new TPhotoSize;
      p->dpi = 0;
      p->autoRotate = true;
      p->label = i18n("9 x 13cm");
                        // page size
      p->layouts.append(new QRect(0, 0, 1270, 1778));
                        // photo layouts
      p->layouts.append(new QRect( 50,  100, 900, 1300));
                        // add to the list
      d->m_photoSizes.append(p);

                        // thumbnails
      d->m_photoSizes.append(createPhotoGrid(1270, 1778, i18n("Thumbnails"), 5, 4));
                
                        // small thumbnails
      d->m_photoSizes.append(createPhotoGrid(1270, 1778, i18n("Small Thumbnails"), 6, 5));
                
    } // 13x18 cm 
    break;
                        
    default:
    {
      kDebug() << "Initializing Unsupported page layouts\n";
                        // We don't support this page size yet.  Just create a default page.
      TPhotoSize *p;
      p = new TPhotoSize;
      p->dpi = 0;
      p->autoRotate = false;
      p->label = i18n("Unsupported Paper Size");
                        // page size
      p->layouts.append(new QRect(0, 0, 8500, 11000));
      p->layouts.append(new QRect(0, 0, 8500, 11000));
                        // add to the list
      d->m_photoSizes.append(p);
    }
    break;
  };

  // load the photo sizes into the listbox
  d->mPhotoPage->ListPhotoSizes->clear();
  QList<TPhotoSize*>::iterator it;
  for (it = d->m_photoSizes.begin(); it != d->m_photoSizes.end(); it++)
  {
    TPhotoSize *s = static_cast<TPhotoSize*>(*it);
    if (s) d->mPhotoPage->ListPhotoSizes->addItem(s->label);
  }
  d->mPhotoPage->ListPhotoSizes->setCurrentItem(0);
}
double getMaxDPI(QList<TPhoto*> photos, QList<QRect*> layouts, /*unsigned*/ int current)
{
  Q_ASSERT(layouts.count() > 1);

  QList<QRect*>::iterator it = layouts.begin();
  QRect *layout = static_cast<QRect*>(*it);

  double maxDPI = 0.0;

  for(; current < photos.count(); current++)
  {
    TPhoto *photo = photos.at(current);
    double dpi = ((double)photo->cropRegion.width() + (double)photo->cropRegion.height()) /
          (((double)layout->width() / 1000.0) + ((double)layout->height() / 1000.0));
    if (dpi > maxDPI)
      maxDPI = dpi;
    // iterate to the next position
    it++;
    layout = static_cast<QRect*>(*it);
    if (layout == 0)
    {
      break;
    }
  }
  return maxDPI;
}

QRect * Wizard::getLayout(int photoIndex)
{
  TPhotoSize *s = d->m_photoSizes.at(d->mPhotoPage->ListPhotoSizes->currentRow());
  // how many photos would actually be printed, including copies?
  int photoCount  = (photoIndex + 1);
  // how many pages?  Recall that the first layout item is the paper size
  int photosPerPage = s->layouts.count() - 1;
  int remainder = photoCount % photosPerPage;

  int retVal = remainder;
  if (remainder == 0)
    retVal = photosPerPage;
  return s->layouts.at(retVal);
}

int Wizard::getPageCount()
{
  // get the selected layout
  TPhotoSize *s = d->m_photoSizes.at(d->mPhotoPage->ListPhotoSizes->currentRow());
  
  int photoCount  =  d->m_photos.count();
  // how many pages?  Recall that the first layout item is the paper size
  int photosPerPage = s->layouts.count() - 1;
  int remainder = photoCount % photosPerPage;
  int emptySlots = 0;
  if (remainder > 0)
    emptySlots = photosPerPage - remainder;
  int pageCount = photoCount / photosPerPage;
  if (emptySlots > 0)
    pageCount++;
  return pageCount;
}


const float FONT_HEIGHT_RATIO = 0.8;

void Wizard::printCaption(QPainter &p, TPhoto*photo, int captionW, int captionH, QString caption)
{
  // PENDING anaselli TPhoto*photo will be needed to add a per photo caption management
  QStringList captionByLines;

  uint captionIndex = 0;

  while (captionIndex < caption.length())
  {
    QString newLine;
    bool breakLine = false; // End Of Line found
    uint currIndex; //  Caption QString current index

    // Check minimal lines dimension
    //TODO fix length, maybe useless
    uint captionLineLocalLength = 40;

    for ( currIndex = captionIndex; currIndex < caption.length() && !breakLine; currIndex++ )
      if( caption[currIndex] == QChar('\n') || caption[currIndex].isSpace() )
        breakLine = true;

    if (captionLineLocalLength <= (currIndex - captionIndex))
      captionLineLocalLength = (currIndex - captionIndex);

    breakLine = false;

    for ( currIndex = captionIndex;
          currIndex <= captionIndex + captionLineLocalLength &&
              currIndex < caption.length() && !breakLine;
          currIndex++ )
    {
      breakLine = (caption[currIndex] == QChar('\n')) ? true : false;

      if (breakLine)
        newLine.append( ' ' );
      else
        newLine.append( caption[currIndex] );
    }

    captionIndex = currIndex; // The line is ended

    if ( captionIndex != caption.length() )
      while ( !newLine.endsWith(" ") )
    {
      newLine.truncate(newLine.length() - 1);
      captionIndex--;
    }

    captionByLines.prepend(newLine.trimmed());
  }

  QFont font(d->mInfoPage->m_font_name->currentFont());
  font.setStyleHint(QFont::SansSerif);
  font.setPixelSize( (int)(captionH * FONT_HEIGHT_RATIO) );
  font.setWeight(QFont::Normal);

  QFontMetrics fm( font );
  int pixelsHigh = fm.height();

  p.setFont(font);
  p.setPen(d->mInfoPage->m_font_color->color());
  kDebug( 51000 ) << "Number of lines " << (int)captionByLines.count() << endl;

  // Now draw the caption
  // TODO allow printing captions  per photo and on top, bottom and vertically
  for ( int lineNumber = 0; lineNumber < (int)captionByLines.count(); lineNumber++ )
  {
    if (lineNumber > 0)
      p.translate(0, -(int)(pixelsHigh));
    QRect r(0, 0, captionW, captionH);
    //TODO anaselli check if ok
    p.drawText(r, Qt::AlignLeft, captionByLines[lineNumber], &r);
  }
}



QString Wizard::captionFormatter(TPhoto *photo, const QString& format)
{
  QString str=format;

  QFileInfo fi(photo->filename.path());
  QString resolution;
  QSize imageSize =  photo->exiv2Iface()->getImageDimensions();
  if (imageSize.isValid()) {
    resolution = QString( "%1x%2" ).arg( imageSize.width()).arg( imageSize.height());
  }
  str.replace("\\n", "\n");

  // %f filename
  // %c comment
  // %d date-time
  // %t exposure time
  // %i iso
  // %r resolution
  // %a aperture
  // %l focal length
  str.replace("%f", fi.fileName());
  str.replace("%c", photo->exiv2Iface()->getExifComment());
  str.replace("%d", KGlobal::locale()->formatDateTime(photo->exiv2Iface()->getImageDateTime()));
  str.replace("%t", photo->exiv2Iface()->getExifTagString("Exif.Photo.ExposureTime"));
  str.replace("%i", photo->exiv2Iface()->getExifTagString("Exif.Photo.ISOSpeedRatings"));
  str.replace("%r", resolution);
  str.replace("%a", photo->exiv2Iface()->getExifTagString("Exif.Photo.FNumber"));
  str.replace("%l", photo->exiv2Iface()->getExifTagString("Exif.Photo.FocalLength"));

  return str;
}

bool Wizard::paintOnePage(QPainter &p, QList<TPhoto*> photos, QList<QRect*> layouts,
                                  int captionType, unsigned int &current, bool useThumbnails)
{
  Q_ASSERT(layouts.count() > 1);

  if (photos.count() == 0) return true; // no photos => last photo

  QList<QRect*>::iterator it = layouts.begin();
  QRect *srcPage = static_cast<QRect*>(*it);
  it++;
  QRect *layout = static_cast<QRect*>(*it);

  // scale the page size to best fit the painter
  // size the rectangle based on the minimum image dimension
  int destW = p.window().width();
  int destH = p.window().height();

  int srcW = srcPage->width();
  int srcH = srcPage->height();
  if (destW < destH)
  {
    destH = NINT((double)destW * ((double)srcH / (double)srcW));
    if (destH > p.window().height())
    {
      destH = p.window().height();
      destW = NINT((double)destH * ((double)srcW / (double)srcH));
    }
  }
  else
  {
    destW = NINT((double)destH * ((double)srcW / (double)srcH));
    if (destW > p.window().width())
    {
      destW = p.window().width();
      destH = NINT((double)destW * ((double)srcH / (double)srcW));
    }
  }

  double xRatio = (double)destW / (double)srcPage->width();
  double yRatio = (double)destH / (double)srcPage->height();

  int left = (p.window().width()  - destW) / 2;
  int top  = (p.window().height() - destH) / 2;

  // FIXME: may not want to erase the background page
  p.eraseRect(left, top,
              NINT((double)srcPage->width() * xRatio),
                    NINT((double)srcPage->height() * yRatio));

  for(; current < photos.count(); current++)
  {
    TPhoto *photo = photos.at(current);
    // crop
    QImage img;
    if (useThumbnails)
      img = photo->thumbnail().toImage();
    else
      img = photo->loadPhoto();

    // next, do we rotate?
    if (photo->rotation != 0)
    {
      // rotate
      QMatrix matrix;
      matrix.rotate(photo->rotation);
      img = img.transformed(matrix);
    }

    if (useThumbnails)
    {
      // scale the crop region to thumbnail coords
      double xRatio = 0.0;
      double yRatio = 0.0;

      if (photo->thumbnail().width() != 0)
        xRatio = (double)photo->thumbnail().width() / (double) photo->width();
      if (photo->thumbnail().height() != 0)
        yRatio = (double)photo->thumbnail().height() / (double) photo->height();

      int x1 = NINT((double)photo->cropRegion.left() * xRatio);
      int y1 = NINT((double)photo->cropRegion.top()  * yRatio);

      int w = NINT((double)photo->cropRegion.width()  * xRatio);
      int h = NINT((double)photo->cropRegion.height() * yRatio);

      img = img.copy(QRect(x1, y1, w, h));
    }
    else
      img = img.copy(photo->cropRegion);

    int x1 = NINT((double)layout->left() * xRatio);
    int y1 = NINT((double)layout->top()  * yRatio);
    int w  = NINT((double)layout->width() * xRatio);
    int h  = NINT((double)layout->height() * yRatio);

    p.drawImage( QRect(x1 + left, y1 + top, w, h), img );

    if (captionType > 0)
    {
      p.save();
      QString caption;
      QString format;
      switch (captionType)
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
          kWarning( 51000 ) << "UNKNOWN caption type " << captionType << endl;
          break;
      }
      caption = captionFormatter(photo, format);
      kDebug( 51000 ) << "Caption " << caption << endl;

      // draw the text at (0,0), but we will translate and rotate the world
      // before drawing so the text will be in the correct location
      // next, do we rotate?
      int captionW = w-2;
      double ratio =  d->mInfoPage->m_font_size->value() * 0.01;
      int captionH = (int)(qMin(w, h) * ratio);

      int exifOrientation = photo->exiv2Iface()->getImageOrientation();
      int orientatation = photo->rotation;


      //ORIENTATION_ROT_90_HFLIP .. ORIENTATION_ROT_270
      if (exifOrientation == KExiv2Iface::KExiv2::ORIENTATION_ROT_90_HFLIP ||
          exifOrientation == KExiv2Iface::KExiv2::ORIENTATION_ROT_90 ||
          exifOrientation == KExiv2Iface::KExiv2::ORIENTATION_ROT_90_VFLIP ||
          exifOrientation == KExiv2Iface::KExiv2::ORIENTATION_ROT_270)
        orientatation = (photo->rotation + 270) % 360; // -90 degrees

      if(orientatation == 90 || orientatation == 270)
      {
        captionW = h;
      }
      p.rotate(orientatation);
      kDebug( 51000 ) << "rotation " << photo->rotation << " orientation " << orientatation << endl;
      int tx = left;
      int ty = top;

      switch(orientatation) {
        case 0 : {
          tx += x1 + 1;
          ty += y1 + (h - captionH - 1);
          break;
        }
        case 90 : {
          tx = top + y1 + 1;
          ty = -left - x1 - captionH - 1;
          break;
        }
        case 180 : {
          tx = -left - x1 - w + 1;
          ty = -top -y1 - (captionH + 1);
          break;
        }
        case 270 : {
          tx = -top - y1 - h + 1;
          ty = left + x1 + (w - captionH)- 1;
          break;
        }
      }
      p.translate(tx, ty);
      printCaption(p, photo, captionW, captionH, caption);
      p.restore();
    } // caption

    // iterate to the next position
    it++;
    layout = static_cast<QRect*>(*it);
    if (layout == 0)
    {
      current++;
      break;
    }
  }
  // did we print the last photo?
  return (current < photos.count());
}


// Like above, but outputs to an initialized QImage.  UseThumbnails is
// not an option.
// We have to use QImage for saving to a file, otherwise we would have
// to use a QPixmap, which will have the same bit depth as the display.
// So someone with an 8-bit display would not be able to save 24-bit
// images!
bool Wizard::paintOnePage(QImage &p, QList<TPhoto*> photos, QList<QRect*> layouts,
                                  int captionType, unsigned int &current)
{
  Q_ASSERT(layouts.count() > 1);

  QList<QRect*>::iterator it = layouts.begin();
  QRect *srcPage = static_cast<QRect*>(*it);
  it++;
  QRect *layout = static_cast<QRect*>(*it);

  // scale the page size to best fit the painter
  // size the rectangle based on the minimum image dimension
  int destW = p.width();
  int destH = p.height();

  int srcW = srcPage->width();
  int srcH = srcPage->height();
  if (destW < destH)
  {
    destH = NINT((double)destW * ((double)srcH / (double)srcW));
    if (destH > p.height())
    {
      destH = p.height();
      destW = NINT((double)destH * ((double)srcW / (double)srcH));
    }
  }
  else
  {
    destW = NINT((double)destH * ((double)srcW / (double)srcH));
    if (destW > p.width())
    {
      destW = p.width();
      destH = NINT((double)destW * ((double)srcH / (double)srcW));
    }
  }

  double xRatio = (double)destW / (double)srcPage->width();
  double yRatio = (double)destH / (double)srcPage->height();

  int left = (p.width()  - destW) / 2;
  int top  = (p.height() - destH) / 2;


  p.fill(0xffffff);

  for(; current < photos.count(); current++)
  {
    TPhoto *photo = photos.at(current);
    // crop
    QImage img;
    img = photo->loadPhoto();

    // next, do we rotate?
    if (photo->rotation != 0)
    {
      // rotate
      QMatrix matrix;
      matrix.rotate(photo->rotation);
      img = img.transformed(matrix);
    }

    img = img.copy(photo->cropRegion);

    int x1 = NINT((double)layout->left() * xRatio);
    int y1 = NINT((double)layout->top()  * yRatio);
    int w  = NINT((double)layout->width() * xRatio);
    int h  = NINT((double)layout->height() * yRatio);

    // We can use scaleFree because the crop frame should have the proper dimensions.
    img = img.scaled (w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
//     img = img.smoothScale(w, h, QImage::ScaleFree);

    // don't have drawimage, so we copy the pixels over manually
    for(int srcY = 0; srcY < img.height(); srcY++)
      for(int srcX = 0; srcX < img.width(); srcX++)
    {
      p.setPixel(x1 + left + srcX, y1 + top + srcY, img.pixel(srcX, srcY));
    }

    if (captionType != NoCaptions)
    {
      // Now draw the caption
      QString caption;
      QString format;
      switch (captionType)
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
          kWarning( 51000 ) << "UNKNOWN caption type " << captionType << endl;
          break;
      }
      caption = captionFormatter(photo, format);
      kDebug( 51000 ) << "Caption " << caption << endl;

      int captionW = w-2;
      double ratio = d->mInfoPage->m_font_size->value() * 0.01;
      int captionH = (int)(qMin(w, h) * ratio);

      int exifOrientation = photo->exiv2Iface()->getImageOrientation();
      int orientatation = photo->rotation;

      //ORIENTATION_ROT_90_HFLIP .. ORIENTATION_ROT_270
      if (exifOrientation == KExiv2Iface::KExiv2::ORIENTATION_ROT_90_HFLIP ||
          exifOrientation == KExiv2Iface::KExiv2::ORIENTATION_ROT_90 ||
          exifOrientation == KExiv2Iface::KExiv2::ORIENTATION_ROT_90_VFLIP ||
          exifOrientation == KExiv2Iface::KExiv2::ORIENTATION_ROT_270)
        orientatation = (photo->rotation + 270) % 360; // -90 degrees

      if (orientatation == 90 || orientatation == 270)
      {
        captionW = h;
      }

      QPixmap pixmap(w-2, img.height()-2);
      //TODO black is not ok if font is black...
      pixmap.fill(Qt::black);
      QPainter painter;
      painter.begin(&pixmap);
      painter.rotate(orientatation);
      kDebug( 51000 ) << "rotation " << photo->rotation << " orientation " << orientatation << endl;
      int tx = left;
      int ty = top;

      switch(orientatation) {
        case 0 : {
          tx += x1 + 1;
          ty += y1 + (h - captionH - 1);
          break;
        }
        case 90 : {
          tx = top + y1 + 1;
          ty = -left - x1 - captionH - 1;
          break;
        }
        case 180 : {
          tx = -left - x1 - w + 1;
          ty = -top -y1 - (captionH + 1);
          break;
        }
        case 270 : {
          tx = -top - y1 - h + 1;
          ty = left + x1 + (w - captionH)- 1;
          break;
        }
      }

      painter.translate(tx, ty);
      printCaption(painter, photo, captionW, captionH, caption);
      painter.end();

      // now put it on picture
      QImage fontImage = pixmap.toImage();
      QRgb black = QColor(0, 0, 0).rgb();
      for(int srcY = 0; srcY < fontImage.height(); srcY++)
        for(int srcX = 0; srcX < fontImage.width(); srcX++)
      {
        if (fontImage.pixel(srcX, srcY) != black)
          p.setPixel(srcX, srcY, fontImage.pixel(srcX, srcY));
      }
    } // caption

    // iterate to the next position
    it++;
    layout = static_cast<QRect*>(*it);
    if (layout == 0)
    {
      current++;
      break;
    }
  }
  // did we print the last photo?
  return (current < photos.count());
}

void Wizard::updateCropFrame(TPhoto *photo, int photoIndex)
{
  TPhotoSize *s = d->m_photoSizes.at(d->mPhotoPage->ListPhotoSizes->currentRow());
  d->mCropPage->cropFrame->init(photo, getLayout(photoIndex)->width(), getLayout(photoIndex)->height(), s->autoRotate);
  d->mCropPage->LblCropPhoto->setText(i18n("Photo %1 of %2").arg(photoIndex + 1).arg( QString::number(d->m_photos.count()) ));
}

// update the pages to be printed and preview first/last pages
void Wizard::previewPhotos()
{
  // get the selected layout
  TPhotoSize *s = d->m_photoSizes.at(d->mPhotoPage->ListPhotoSizes->currentRow());

  int photoCount  =  d->m_photos.count();
  // how many pages?  Recall that the first layout item is the paper size
  int photosPerPage = s->layouts.count() - 1;
  int remainder = photoCount % photosPerPage;
  int emptySlots = 0;
  if (remainder > 0)
    emptySlots = photosPerPage - remainder;
  int pageCount = photoCount / photosPerPage;
  if (emptySlots > 0)
    pageCount++;

  d->mPhotoPage->LblPhotoCount->setText(QString::number(photoCount));
  d->mPhotoPage->LblSheetsPrinted->setText(QString::number(pageCount));
  d->mPhotoPage->LblEmptySlots->setText(QString::number(emptySlots));

  // photo previews
  // preview the first page.
  // find the first page of photos
  int count = 0;
  int page = 0;
  unsigned int current = 0;
  
  QList<TPhoto*>::iterator it;
  for (it = d->m_photos.begin(); it != d->m_photos.end(); it++)
  {
    TPhoto *photo = static_cast<TPhoto*>(*it);

    if (page == d->m_currentPreviewPage) {
      photo->cropRegion.setRect(-1, -1, -1, -1);
      photo->rotation = 0;
      int w = s->layouts.at(count+1)->width();
      int h = s->layouts.at(count+1)->height();
      d->mCropPage->cropFrame->init(photo, w, h, s->autoRotate, false);
    }
    count++;
    if (count >= photosPerPage)
    {
      if (page == d->m_currentPreviewPage)
        break;
      page++;
      current += photosPerPage;
      count = 0;
    }
  }

  // send this photo list to the painter
  QPixmap img(d->mPhotoPage->BmpFirstPagePreview->width(), d->mPhotoPage->BmpFirstPagePreview->height());
  QPainter p;
  p.begin(&img);
  QPalette palette(d->mPhotoPage->backgroundRole());
  p.fillRect(0, 0, img.width(), img.height(), palette.color(QPalette::Background));
  paintOnePage(p, d->m_photos, s->layouts, d->mInfoPage->m_captions-> currentIndex(), current, true);
  p.end();
  d->mPhotoPage->BmpFirstPagePreview->setPixmap(img);
  d->mPhotoPage->LblPreview->setText(i18n("Page ") + QString::number(d->m_currentPreviewPage + 1) + i18n(" of ") + QString::number(getPageCount()));
  d->mPhotoPage->LblPreview->setText(i18n("Page %1 of %2").arg(d->m_currentPreviewPage + 1).arg(getPageCount()));

  manageBtnPreviewPage();
  manageBtnPrintOrder();
}

void Wizard::manageBtnPreviewPage()
{
  d->mPhotoPage->BtnPreviewPageDown->setEnabled(true);
  d->mPhotoPage->BtnPreviewPageUp->setEnabled(true);
  if (d->m_currentPreviewPage == 0)
  {
    d->mPhotoPage->BtnPreviewPageDown->setEnabled(false);
  }
  
  if ((d->m_currentPreviewPage + 1) == getPageCount())
  {
    d->mPhotoPage->BtnPreviewPageUp->setEnabled(false);
  }
}

void Wizard::manageBtnPrintOrder()
{
  if (d->mPhotoPage->ListPrintOrder->currentRow() == -1)
    return;

  d->mPhotoPage->BtnPrintOrderDown->setEnabled(true);
  d->mPhotoPage->BtnPrintOrderUp->setEnabled(true);
  if (d->mPhotoPage->ListPrintOrder->currentRow() == 0)
  {
    d->mPhotoPage->BtnPrintOrderUp->setEnabled(false);
  }
  if (uint(d->mPhotoPage->ListPrintOrder->currentRow() + 1) == d->mPhotoPage->ListPrintOrder->count())
  {
    d->mPhotoPage->BtnPrintOrderDown->setEnabled(false);
  }
}

// Wizard SLOTS
void Wizard::slotHelp()
{
  KToolInvocation::invokeHelp("printwizard","kipi-plugins");
}


void Wizard::printOrderDownClicked()
{
  int currentIndex = d->mPhotoPage->ListPrintOrder->currentRow();

  if (currentIndex == (signed int)d->mPhotoPage->ListPrintOrder->count() - 1)
    return;

  QString item1 = d->mPhotoPage->ListPrintOrder->currentItem()->text();
  QString item2 = d->mPhotoPage->ListPrintOrder->item(currentIndex + 1)->text();

  // swap these items
  d->mPhotoPage->ListPrintOrder->insertItem(currentIndex, item2);
  d->mPhotoPage->ListPrintOrder->insertItem(currentIndex + 1, item1);

  // the list box items are swapped, now swap the items in the photo list
  TPhoto *photo1 = d->m_photos.at(currentIndex);
  TPhoto *photo2 = d->m_photos.at(currentIndex + 1);
  d->m_photos.removeAt(currentIndex);
  d->m_photos.removeAt(currentIndex);
  d->m_photos.insert(currentIndex, photo1);
  d->m_photos.insert(currentIndex, photo2);
  previewPhotos();
}


void Wizard::btnBrowseOutputPathClicked(void)
{
  QString newPath = KFileDialog::getExistingDirectory(d->mInfoPage->EditOutputPath->text(), this, 
                                                      i18n("Select Output Folder"));
  if (newPath.isEmpty())
    return;
  // assume this directory exists
  d->mInfoPage->EditOutputPath->setText(newPath);
  outputSettingsClicked(d->m_outputSettings->checkedId ());
}

void Wizard::paperSizeChanged(int index)
{
  PageSize pageSize = (PageSize)index;
  initPhotoSizes(pageSize);

  if (pageSize > A6)
  {
    KMessageBox::information (this, 
                              i18n("Don't forget to set up the correct page size according to your printer settings"), 
                                   i18n("Page size settings"), "pageSizeInfo");
  }
}

void  Wizard::pageChanged (KPageWidgetItem *current)
{
  if (current->name() == i18n("Introduction"))
  {
  }
  else if (current->name() == i18n("Select printing information"))
  {
#ifdef NOT_YET
    if (d->mInfoPage->GrpOutputSettings->id(RdoOutputPrinter))
      this->nextButton()->setEnabled(true);
    else if (id == GrpOutputSettings->id(RdoOutputFile))
    {
      if (!EditOutputPath->text().isEmpty())
      {
        QFileInfo fileInfo(EditOutputPath->text());
        if (fileInfo.exists() && fileInfo.isDir())
          this->nextButton()->setEnabled(true);
      }
    }
    else
      if (id == GrpOutputSettings->id(RdoOutputGimp))
    {
      this->nextButton()->setEnabled(true);
    }
    kDebug() << "CCCC" << endl;
#endif
  }
  else if (current->name() == i18n("Crop photos"))
  {
  }


  /*d->mIntroPage = new IntroPage(this, i18n("Introduction"));
  d->mInfoPage  = new InfoPage(this, i18n("Select printing information"));
  d->mPhotoPage nt id= new PhotoPage(this, i18n("Photo information"));
  d->mCropPage  = new CropPage(this, i18n("Crop photos")) ;
   */
//   if (current ==   static_cast<KPageWidgetItem>(d->mIntroPage))
//   {
//     KDebug << "CCCCCCCCCCC" << endl;
//   }
//   else if (current == static_cast<KPageWidgetItem>(d->mInfoPage))
//   {
//   }
//   else if (current == d->mPhotoPage)
//   {
//   }
//   else if (current == d->mCropPage)
//   {
//   }
}


void Wizard::outputSettingsClicked(int id)
{
  if (id == d->m_outputSettings->id(d->mInfoPage->RdoOutputPrinter))
    this->setValid(d->mCropPage->page(), true);
  else if (id == d->m_outputSettings->id(d->mInfoPage->RdoOutputFile))
  {
    if (!d->mInfoPage->EditOutputPath->text().isEmpty())
    {
      QFileInfo fileInfo(d->mInfoPage->EditOutputPath->text());
      if (!(fileInfo.exists() && fileInfo.isDir()))
        this->setValid(d->mCropPage->page(), false);
    }
    else
      this->setValid(d->mCropPage->page(), false);
  }
  else if (id == d->m_outputSettings->id(d->mInfoPage->RdoOutputGimp))
  {
    this->setValid(d->mCropPage->page(), true);
  }
}

void Wizard::captionChanged(const QString & text)
{
  //TODO use QVariant and add them by hands
  if (text == i18n("No captions"))
  {
    d->mInfoPage->m_font_frame->setEnabled(false);
    d->mInfoPage->m_FreeCaptionFormat->setEnabled(false);
    d->mInfoPage->m_free_label->setEnabled(false);
  }
  else if (text == i18n("Free"))
  {
    d->mInfoPage->m_font_frame->setEnabled(true);
    d->mInfoPage->m_FreeCaptionFormat->setEnabled(true);
    d->mInfoPage->m_free_label->setEnabled(true);
  }
  else
  {
    d->mInfoPage->m_font_frame->setEnabled(true);
    d->mInfoPage->m_FreeCaptionFormat->setEnabled(false);
    d->mInfoPage->m_free_label->setEnabled(false);
  }
}

void Wizard::BtnCropRotate_clicked()
{
  // by definition, the cropRegion should be set by now,
  // which means that after our rotation it will become invalid,
  // so we will initialize it to -2 in an awful hack (this
  // tells the cropFrame to reset the crop region, but don't
  // automagically rotate the image to fit.
  TPhoto *photo = d->m_photos[d->m_currentCropPhoto];
  photo->cropRegion = QRect(-2, -2, -2, -2);
  photo->rotation = (photo->rotation + 90) % 360;

  updateCropFrame(photo, d->m_currentCropPhoto);

}

void Wizard::setBtnCropEnabled()
{
  if (d->m_currentCropPhoto == 0)
    d->mCropPage->BtnCropPrev->setEnabled(false);
  else
    d->mCropPage->BtnCropPrev->setEnabled(true);

  if (d->m_currentCropPhoto == (int)d->m_photos.count() - 1)
    d->mCropPage->BtnCropNext->setEnabled(false);
  else
    d->mCropPage->BtnCropNext->setEnabled(true);
}

void Wizard::BtnCropNext_clicked()
{
  TPhoto *photo = 0;
  photo = d->m_photos[++d->m_currentCropPhoto];
  setBtnCropEnabled();
  if (photo == 0)
  {
    d->m_currentCropPhoto = d->m_photos.count() - 1;
    return;
  }
  updateCropFrame(photo, d->m_currentCropPhoto);
}

void Wizard::BtnCropPrev_clicked()
{
  TPhoto *photo = 0;
  photo = d->m_photos[--d->m_currentCropPhoto];

  setBtnCropEnabled();

  if (photo == 0)
  {
    d->m_currentCropPhoto = 0;
    return;
  }
  updateCropFrame(photo, d->m_currentCropPhoto);
}

void Wizard::BtnPrintOrderUp_clicked() {
  if (d->mPhotoPage->ListPrintOrder->currentItem() == 0)
    return;

  int currentIndex = d->mPhotoPage->ListPrintOrder->currentRow();

  // swap these items
  QListWidgetItem *item1 = d->mPhotoPage->ListPrintOrder->takeItem(currentIndex - 1);
  QListWidgetItem *item2 = d->mPhotoPage->ListPrintOrder->takeItem(currentIndex - 1);
  d->mPhotoPage->ListPrintOrder->insertItem(currentIndex - 1, item2);
  d->mPhotoPage->ListPrintOrder->insertItem(currentIndex - 1, item1);

  // the list box items are swapped, now swap the items in the photo list
  d->m_photos.swap(currentIndex, currentIndex - 1);
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

void Wizard::BtnPrintOrderDown_clicked() {
  int currentIndex = d->mPhotoPage->ListPrintOrder->currentRow();

  if (currentIndex == d->mPhotoPage->ListPrintOrder->count() - 1)
    return;


  // swap these items
  QListWidgetItem *item1 = d->mPhotoPage->ListPrintOrder->takeItem(currentIndex);
  QListWidgetItem *item2 = d->mPhotoPage->ListPrintOrder->takeItem(currentIndex);
  d->mPhotoPage->ListPrintOrder->insertItem(currentIndex, item1);
  d->mPhotoPage->ListPrintOrder->insertItem(currentIndex, item2);

  // the list box items are swapped, now swap the items in the photo list
  d->m_photos.swap(currentIndex, currentIndex + 1);

  previewPhotos();
}

void Wizard::BtnPreviewPageDown_clicked() {
  if (d->m_currentPreviewPage == 0)
    return;
  d->m_currentPreviewPage--;
  previewPhotos();
}
void Wizard::BtnPreviewPageUp_clicked() {
  if (d->m_currentPreviewPage == getPageCount() - 1)
    return;
  d->m_currentPreviewPage++;
  previewPhotos();
}




/**
 *
 */
void Wizard::accept() {
  KDialog();
	KAssistantDialog::accept();
}


} // namespace
