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

// Qt includes.

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qspinbox.h>

// KDE includes.

#include <kapplication.h>
#include <kconfigdialogmanager.h>
#include <kdebug.h>
#include <kdialog.h>
#include <khelpmenu.h>
#include <klistwidget.h>
#include <klocale.h>
#include <kmenu.h>
#include <ktextbrowser.h>
#include <ktoolinvocation.h>
#include <kurlrequester.h>

// KIPI includes.

#include <libkipi/imagecollectionselector.h>
#include <libkipi/interface.h>

// Local includes.

#include "kpaboutdata.h"
#include "ui_croppage.h"
#include "ui_infopage.h"
#include "ui_intropage.h"
#include "ui_photopage.h"

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


  //TODO fix icons
#ifdef NOT_YET
  // setting-up icons on buttons
  d->mInfoPage->BtnBrowseOutputPath->setText("");
  d->mInfoPage->BtnBrowseOutputPath->setIcon( SmallIcon( "fileopen" ) );
  d->mPhotoPage->BtnPrintOrderDown->setText("");
  d->mPhotoPage->BtnPrintOrderDown->setIcon( SmallIcon( "down" ) );
  d->mPhotoPage->BtnPrintOrderUp->setText("");
  d->mPhotoPage->BtnPrintOrderUp->setIcon( SmallIcon( "up" ) );
  d->mPhotoPage->BtnPreviewPageUp->setText("");
  d->mPhotoPage->BtnPreviewPageUp->setIcon( SmallIcon( "next" ) );
  d->mPhotoPage->BtnPreviewPageDown->setText("");
  d->mPhotoPage->BtnPreviewPageDown->setIcon( SmallIcon( "previous" ) );
  d->mCropPage->BtnCropPrev->setText("");
  d->mCropPage->BtnCropPrev->setIcon( SmallIcon( "previous" ) );
  d->mCropPage->BtnCropNext->setText("");
  d->mCropPage->BtnCropNext->setIcon( SmallIcon( "next" ) );
  d->mCropPage->BtnCropRotate->setText("");
  d->mCropPage->BtnCropRotate->setIcon( SmallIcon( "rotate" ) );
#endif //NOT_YET

  d->m_helpButton = button (Help);
  KHelpMenu* helpMenu = new KHelpMenu(this, d->mAbout, false);
  helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
  QAction *handbook       = new QAction(i18n("Plugin Handbook"), this);

  // create a QButtonGroup to manage button ids
  d->m_outputSettings = new QButtonGroup(this);
  d->m_outputSettings->addButton(d->mInfoPage->RdoOutputPrinter, RDO_BTN_PRINTER);
  d->m_outputSettings->addButton(d->mInfoPage->RdoOutputGimp,    RDO_BTN_GIMP);
  d->m_outputSettings->addButton(d->mInfoPage->RdoOutputFile,    RDO_BTN_FILE);



  // connections
  // connect help
  connect(handbook, SIGNAL(triggered(bool)),
          this, SLOT(slotHelp()));

  // get selected page
  connect(this, SIGNAL(currentPageChanged (KPageWidgetItem *, KPageWidgetItem *)),
          this, SLOT(pageChanged(KPageWidgetItem *)));

  // change caption information
  connect(d->mInfoPage->m_captions, SIGNAL(activated(const QString & )),
          this, SLOT(captionChanged(const QString &)));

  // Output button group
  connect(d->m_outputSettings, SIGNAL(buttonClicked(int)),
          this, SLOT(outputSettingsClicked(int)));

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


// Wizard SLOTS
void Wizard::slotHelp()
{
  KToolInvocation::invokeHelp("printwizard","kipi-plugins");
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
