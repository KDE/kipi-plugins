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

#include "wizard.h"
#include "wizard.moc"

// Qt includes
#include <QFileInfo>

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

// Local includes
#include "kpaboutdata.h"
#include "ui_croppage.h"
#include "ui_infopage.h"
#include "ui_intropage.h"
#include "ui_photopage.h"
#include "tphoto.h"

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

typedef struct _TPhotoSize {
  QString label;
  int dpi;
  bool autoRotate;
  QList<QRect*> layouts;  // first element is page size
} TPhotoSize;

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
  // Default is A4
  d->mInfoPage->CmbPaperSize->setCurrentIndex(A4);
  initPhotoSizes(A4);   // default to A4 for now.

  
  // back button pressed
//  connect(this, SIGNAL(back(void)), this, SLOT(pageSelected(void)));
  // next button pressed
//  connect(this, SIGNAL(next(void)), this, SLOT(pageSelected(void)));

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




// Wizard SLOTS
void Wizard::slotHelp()
{
  KToolInvocation::invokeHelp("printwizard","kipi-plugins");
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

/**
 *
 */
void Wizard::accept() {
  KDialog();
	KAssistantDialog::accept();
}


} // namespace
