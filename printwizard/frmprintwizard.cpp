/***************************************************************************
                          frmprintwizard.cpp  -  description
                             -------------------
    begin                : Mon Sep 30 2002
    copyright            : (C) 2002 by Todd Shoemaker
    email                : todd@theshoemakers.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// C Ansi includes.

extern "C"
{
#include <unistd.h>
}

// Include files for Qt

#include <qlabel.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qpainter.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qdir.h>

// Include files for KDE

#include <ksimpleconfig.h>
#include <klistbox.h>
#include <kprinter.h>
#include <klocale.h>
#include <klineedit.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kcombobox.h>
#include <kprogress.h>
#include <kurllabel.h>
#include <kfiledialog.h>
#include <kpushbutton.h>
#include <kapplication.h>
#include <kaboutdata.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kpopupmenu.h>

// Local includes

#include "utils.h"
#include "cropframe.h"
#include "frmprintwizard.h"


namespace KIPIPrintWizardPlugin
{

#if QT_VERSION<0x030200
// This function emulates int QButtonGroup::selectedId() which does not exist
// in Qt 3.1
inline int buttonGroupSelectedId(const QButtonGroup* group)
{
  QButton* button=group->selected();
  if (!button) return -1;
  return group->id(button);
}
#else
inline int buttonGroupSelectedId(const QButtonGroup* group)
{
  return group->selectedId();
}
#endif


FrmPrintWizard::FrmPrintWizard(QWidget *parent, const char *name )
              : FrmPrintWizardBase(parent, name)
{
  // enable help buttons
  for(int i = 0; i < pageCount(); ++i)
     setHelpEnabled(page(i), true);

  // ---------------------------------------------------------------

  // About data and help button.
    
  KAboutData* about = new KAboutData("kipiplugins",
                                     I18N_NOOP("Print Wizard"), 
                                     "0.1.0-cvs",
                                     I18N_NOOP("A KIPI plugin to print images"),
                                     KAboutData::License_GPL,
                                     "(c) 2003-2004, Todd Shoemaker", 
                                     0,
                                     "http://extragear.kde.org/apps/kipi.php");
    
  about->addAuthor("Todd Shoemaker", I18N_NOOP("Author"),
                   "todd@theshoemakers.net");

  m_helpButton = helpButton();
  KHelpMenu* helpMenu = new KHelpMenu(this, about, false);
  helpMenu->menu()->removeItemAt(0);
  helpMenu->menu()->insertItem(i18n("Print Wizard Handbook"), this, SLOT(slotHelp()), 0, -1, 0);
  m_helpButton->setPopup( helpMenu->menu() );

  // ---------------------------------------------------------------
     
  // turn off back button for first and last page
  setBackEnabled(page(0), false);

  // turn off unimplemented controls
  LblCopies->hide();
  LineCopies->hide();
  EditCopies->hide();

  m_currentPreviewPage = 0;
  m_pageSize = KPrinter::A1; // select a different page to force a refresh in initPhotoSizes.
  initPhotoSizes(KPrinter::Letter); // default to letter for now.

  EditOutputPath->setText(QDir::homeDirPath());

  connect(this, SIGNAL(selected(const QString &)),
          this, SLOT(FrmPrintWizardBaseSelected(const QString &)));

  connect(GrpOutputSettings, SIGNAL(clicked(int)),
          this, SLOT(GrpOutputSettings_clicked(int)));

  connect(EditOutputPath, SIGNAL(textChanged(const QString &)),
          this, SLOT(EditOutputPath_textChanged(const QString &)));

  connect(BtnBrowseOutputPath, SIGNAL(clicked(void)),
          this, SLOT(BtnBrowseOutputPath_clicked(void)));

  CmbPaperSize->setCurrentItem(0);
  
  connect(CmbPaperSize, SIGNAL(activated(int)), 
          this, SLOT(CmbPaperSize_activated(int)));
          
  connect(BtnPrintOrderDown, SIGNAL(clicked(void)),
          this, SLOT(BtnPrintOrderDown_clicked(void)));
  connect(BtnPrintOrderUp, SIGNAL(clicked(void)),
          this, SLOT(BtnPrintOrderUp_clicked(void)));
  connect(BtnPreviewPageUp, SIGNAL(clicked(void)),
          this, SLOT(BtnPreviewPageUp_clicked(void)));
  connect(BtnPreviewPageDown, SIGNAL(clicked(void)),
          this, SLOT(BtnPreviewPageDown_clicked(void)));
  loadSettings();
}

FrmPrintWizard::~FrmPrintWizard()
{
  for(unsigned int i=0; i < m_photos.count(); i++)
    if (m_photos.at(i))
      delete m_photos.at(i);
  m_photos.clear();
}

void FrmPrintWizard::slotHelp()
{
  KApplication::kApplication()->invokeHelp("printwizard","kipi-plugins");
}

void FrmPrintWizard::print( KURL::List fileList, QString tempPath)
{
  m_photos.clear();
  ListPrintOrder->clear();

  for(unsigned int i=0; i < fileList.count(); i++)
  {
    TPhoto *photo = new TPhoto(150);
    photo->filename = fileList[i];
    m_photos.append(photo);
    // load the print order listbox
    ListPrintOrder->insertItem(photo->filename.filename());
  }
  ListPrintOrder->setCurrentItem(0);

  m_tempPath = tempPath;
  LblPhotoCount->setText(QString::number(m_photos.count()));

  BtnCropPrev->setEnabled(false);

  if (m_photos.count() == 1)
    BtnCropNext->setEnabled(false);
}

void FrmPrintWizard::BtnCropRotate_clicked()
{
  // by definition, the cropRegion should be set by now,
  // which means that after our rotation it will become invalid,
  // so we will initialize it to -2 in an awful hack (this
  // tells the cropFrame to reset the crop region, but don't
  // automagically rotate the image to fit.
  TPhoto *photo = m_photos.current();
  photo->cropRegion = QRect(-2, -2, -2, -2);
  photo->rotation = (photo->rotation + 90) % 360;

  updateCropFrame(photo, m_photos.at());

}

void FrmPrintWizard::setBtnCropEnabled()
{
  if (m_photos.at() == 0)
    BtnCropPrev->setEnabled(false);
  else
    BtnCropPrev->setEnabled(true);

  if (m_photos.at() == (int)m_photos.count() - 1)
    BtnCropNext->setEnabled(false);
  else
    BtnCropNext->setEnabled(true);
}

void FrmPrintWizard::BtnCropNext_clicked()
{
  TPhoto *photo = 0;
  photo = m_photos.next();
  setBtnCropEnabled();
  if (photo == 0)
  {
    m_photos.last();
    return;
  }
  updateCropFrame(photo, m_photos.at());
}

void FrmPrintWizard::updateCropFrame(TPhoto *photo, int photoIndex)
{
  TPhotoSize *s = m_photoSizes.at(ListPhotoSizes->currentItem());
  cropFrame->init(photo, getLayout(photoIndex)->width(), getLayout(photoIndex)->height(), s->autoRotate);
  LblCropPhoto->setText(i18n("Photo %1 of %2").arg( QString::number(m_photos.at() + 1) ).arg( QString::number(m_photos.count()) ));
}

void FrmPrintWizard::BtnCropPrev_clicked()
{
  TPhoto *photo = 0;
  photo = m_photos.prev();

  setBtnCropEnabled();

  if (photo == 0)
  {
    m_photos.first();
    return;
  }
  updateCropFrame(photo, m_photos.at());
}

void FrmPrintWizard::FrmPrintWizardBaseSelected(const QString &)
{
  QString pageName = this->currentPage()->name();
  if (pageName == "pgPrinter")
  {
    // use this method to enable/disable the next button
    GrpOutputSettings_clicked(GrpOutputSettings->id(GrpOutputSettings->selected()));
  }
  else
  if (pageName == "pgLayout")
  {
    // create our photo sizes list
    initPhotoSizes(m_pageSize);
    previewPhotos();
  }
  else
  if (pageName == "pgCrop")
  {
    TPhoto *photo = m_photos.first();
    setBtnCropEnabled();
    updateCropFrame(photo, m_photos.at());
  } else
  if (pageName == "pgFinished")
  {
    this->finishButton()->setEnabled(true);

    // set the default crop regions if not already set
    TPhotoSize *s = m_photoSizes.at(ListPhotoSizes->currentItem());
    int i = 0;
    for (TPhoto *photo = m_photos.first(); photo != 0; photo = m_photos.next())
    {
      if (photo->cropRegion == QRect(-1, -1, -1, -1))
        cropFrame->init(photo, getLayout(i)->width(), getLayout(i)->height(), s->autoRotate);
      i++;
    }

    if (RdoOutputPrinter->isChecked())
    {
      KPrinter printer;
      printer.setPageSize(m_pageSize);
      printer.setUsePrinterResolution(true);
      if (printer.setup())
        printPhotos(m_photos, s->layouts, printer);
    }
    else
    if (RdoOutputFile->isChecked())
    {
      // now output the items
      QString path = EditOutputPath->text();
      if (path.right(1) != "/")
        path = path + "/";
      path = path + "kipi_printwizard_";
      printPhotosToFile(m_photos, path, s);
    } else
    if (RdoOutputGimp->isChecked())
    {
      // now output the items
      QString path = m_tempPath;
      if (!checkTempPath(this, path))
        return;
      path = path + "kipi_tmp_";
      if (m_gimpFiles.count() > 0)
        removeGimpFiles();
      m_gimpFiles = printPhotosToFile(m_photos, path, s);
      QStringList args;
      args << "gimp";
      for(QStringList::Iterator it = m_gimpFiles.begin(); it != m_gimpFiles.end(); ++it)
          args << (*it);
      if (!launchExternalApp(args))
      {
        KMessageBox::sorry(this, i18n("There was an error launching the Gimp. Please make sure it is properly installed."), i18n("KIPI"));         return;
      }

    }
  }
}

double getMaxDPI(QPtrList<TPhoto> photos, QPtrList<QRect> layouts, unsigned int current)
{
    Q_ASSERT(layouts.count() > 1);

  QRect *layout = layouts.at(1);

  double maxDPI = 0.0;

  for(; current < photos.count(); current++)
  {
    TPhoto *photo = photos.at(current);
    double dpi = ((double)photo->cropRegion.width() + (double)photo->cropRegion.height()) /
      (((double)layout->width() / 1000.0) + ((double)layout->height() / 1000.0));
    if (dpi > maxDPI)
      maxDPI = dpi;
    // iterate to the next position
    layout = layouts.next();
    if (layout == 0)
    {
      break;
    }
  }
  return maxDPI;
}

QRect * FrmPrintWizard::getLayout(int photoIndex)
{
  TPhotoSize *s = m_photoSizes.at(ListPhotoSizes->currentItem());

  int photoCopies = EditCopies->value();
  // how many photos would actually be printed, including copies?
  int photoCount  = photoCopies * (photoIndex + 1);
  // how many pages?  Recall that the first layout item is the paper size
  int photosPerPage = s->layouts.count() - 1;
  int remainder = photoCount % photosPerPage;

  int retVal = remainder;
  if (remainder == 0)
    retVal = photosPerPage;
  return s->layouts.at(retVal);
}

int FrmPrintWizard::getPageCount() {
  // get the selected layout
  TPhotoSize *s = m_photoSizes.at(ListPhotoSizes->currentItem());

  int photoCopies = EditCopies->value();
  int photoCount  = photoCopies * m_photos.count();
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

// update the pages to be printed and preview first/last pages
void FrmPrintWizard::previewPhotos()
{
  // get the selected layout
  TPhotoSize *s = m_photoSizes.at(ListPhotoSizes->currentItem());

  int photoCopies = EditCopies->value();
  int photoCount  = photoCopies * m_photos.count();
  // how many pages?  Recall that the first layout item is the paper size
  int photosPerPage = s->layouts.count() - 1;
  int remainder = photoCount % photosPerPage;
  int emptySlots = 0;
  if (remainder > 0)
    emptySlots = photosPerPage - remainder;
  int pageCount = photoCount / photosPerPage;
  if (emptySlots > 0)
    pageCount++;

  LblPhotoCount->setText(QString::number(photoCount));
  LblSheetsPrinted->setText(QString::number(pageCount));
  LblEmptySlots->setText(QString::number(emptySlots));

  // photo previews
  // preview the first page.
  // find the first page of photos
  int count = 0;
  int page = 0;
  unsigned int current = 0;
  for (TPhoto *photo = m_photos.first(); photo != 0; photo = m_photos.next())
    {
      if (page == m_currentPreviewPage) {
      photo->cropRegion.setRect(-1, -1, -1, -1);
      photo->rotation = 0;
      int w = s->layouts.at(count+1)->width();
      int h = s->layouts.at(count+1)->height();
      cropFrame->init(photo, w, h, s->autoRotate, false);
    }
    count++;
    if (count >= photosPerPage)
    {
      if (page == m_currentPreviewPage)
        break;
      page++;
      current += photosPerPage;
      count = 0;
    }
  }

  // send this photo list to the painter
  QPixmap img(BmpFirstPagePreview->width(), BmpFirstPagePreview->height());
  QPainter p;
  p.begin(&img);
  p.fillRect(0, 0, img.width(), img.height(), this->paletteBackgroundColor());
  paintOnePage(p, m_photos, s->layouts, buttonGroupSelectedId(GrpImageCaptions), current, true);
  p.end();
  BmpFirstPagePreview->setPixmap(img);
  LblPreview->setText(i18n("Page ") + QString::number(m_currentPreviewPage + 1) + i18n(" of ") + QString::number(getPageCount()));
  LblPreview->setText(i18n("Page %1 of %2").arg(m_currentPreviewPage + 1).arg(getPageCount()));
}

void FrmPrintWizard::ListPhotoSizes_highlighted ( int )
{
  m_currentPreviewPage = 0;
  for (TPhoto *photo = m_photos.first(); photo != 0; photo = m_photos.next())
  {
    photo->cropRegion.setRect(-1, -1, -1, -1);
    photo->rotation = 0;
  }
  previewPhotos();
}

void FrmPrintWizard::ListPhotoSizes_selected( QListBoxItem * )
{
  previewPhotos();
}

void FrmPrintWizard::EditCopies_valueChanged( int copies )
{
  for (TPhoto *photo = m_photos.first(); photo != 0; photo = m_photos.next())
      photo->copies = copies;
  previewPhotos();
}

void FrmPrintWizard::removeGimpFiles()
{
    for(QStringList::Iterator it = m_gimpFiles.begin(); it != m_gimpFiles.end(); ++it)
    {
      if (QFile::exists(*it))
      {
        if (QFile::remove(*it) == false)
        {
          KMessageBox::sorry(this, i18n("Could not remove the Gimp's temporary files."));
          break;
        }
      }
    }
}

// this is called when Finish is clicked.
void FrmPrintWizard::accept()
{
  saveSettings();

  // if we output to Gimp, we need to remove the temp files
  if (m_gimpFiles.count() > 0)
    removeGimpFiles();
  QDialog::accept();
}

// this is called when Cancel is clicked.
void FrmPrintWizard::reject()
{
  m_cancelPrinting = true;
  if (m_gimpFiles.count() > 0)
    removeGimpFiles();
  QDialog::reject();
}

void FrmPrintWizard::printPhotos(QPtrList<TPhoto> photos, QPtrList<QRect> layouts, KPrinter &printer)
{
  m_cancelPrinting = false;
  LblPrintProgress->setText("");
  PrgPrintProgress->setProgress(0);
  PrgPrintProgress->setTotalSteps(photos.count());
  this->finishButton()->setEnabled(false);
  KApplication::kApplication()->processEvents();

  QPainter p;
  p.begin(&printer);

  unsigned int current = 0;

  bool printing = true;
  while(printing)
  {
    printing = paintOnePage(p, photos, layouts, buttonGroupSelectedId(GrpImageCaptions), current);
    if (printing)
      printer.newPage();
    PrgPrintProgress->setProgress(current);
    KApplication::kApplication()->processEvents();
    if (m_cancelPrinting)
    {
      printer.abort();
      return;
    }
  }
  p.end();

  this->finishButton()->setEnabled(true);
  LblPrintProgress->setText(i18n("Complete.  Click Finish to exit the Print Wizard."));
}

QStringList FrmPrintWizard::printPhotosToFile(QPtrList<TPhoto> photos, QString &baseFilename, TPhotoSize* layouts)
{
    Q_ASSERT(layouts->layouts.count() > 1);

  m_cancelPrinting = false;
  LblPrintProgress->setText("");
  PrgPrintProgress->setProgress(0);
  PrgPrintProgress->setTotalSteps(photos.count());
  this->finishButton()->setEnabled(false);
  KApplication::kApplication()->processEvents();

  unsigned int current = 0;
  int pageCount = 1;
  bool printing = true;
  QStringList files;

  QRect *srcPage = layouts->layouts.at(0);

  while (printing)
  {
    // make a pixmap to save to file.  Make it just big enough to show the
    // highest-dpi image on the page without losing data.
    double dpi = layouts->dpi;
    if (dpi == 0.0)
      dpi = getMaxDPI(photos, layouts->layouts, current) * 1.1;
    int w = NINT(srcPage->width() / 1000.0 * dpi);
    int h = NINT(srcPage->height()  / 1000.0 * dpi);
    QImage *img = new QImage(w, h, 32);
    if (!img)
      break;

    // save this page out to file
    QString filename = baseFilename + QString::number(pageCount) + ".jpeg";
    bool saveFile = true;
    if (QFile::exists(filename))
    {
      int result = KMessageBox::warningYesNoCancel( this,
          i18n("The following file will be overwritten. Do you want to overwrite this file?") +
          "\n\n" + filename);
      if (result == KMessageBox::No)
        saveFile = false;
      else if (result == KMessageBox::Cancel)
      {
        delete img;
        break;
      }
    }

    // paint this page, even if we aren't saving it to keep the page
    // count accurate.
    printing = paintOnePage(*img, photos, layouts->layouts, buttonGroupSelectedId(GrpImageCaptions), current);

    if (saveFile)
    {
      files.append(filename);
      img->save(filename, "JPEG");
    }
    delete img;
    pageCount++;

    PrgPrintProgress->setProgress(current);
    KApplication::kApplication()->processEvents();
    if (m_cancelPrinting)
      break;
  }
  this->finishButton()->setEnabled(true);
  // did we cancel?
  if (printing)
    LblPrintProgress->setText(i18n("Printing Canceled."));
  else
    LblPrintProgress->setText(i18n("Complete.  Click Finish to exit the Print Wizard."));
  return files;
}

void FrmPrintWizard::loadSettings()
{
  KSimpleConfig config("kipirc");
  config.setGroup("PrintWizard");

  int pageSize = config.readNumEntry("PageSize", (int)m_pageSize);
  initPhotoSizes((KPrinter::PageSize)pageSize);

  if (m_pageSize == KPrinter::A6)
      CmbPaperSize->setCurrentItem(2);
  else if (m_pageSize == KPrinter::A4)
      CmbPaperSize->setCurrentItem(1);      
  else
      CmbPaperSize->setCurrentItem(0);

  // captions
  int captions = config.readNumEntry("ImageCaptions", 0);
  GrpImageCaptions->setButton(captions);

  // set the last output path
  QString outputPath = config.readPathEntry("OutputPath", EditOutputPath->text());
  EditOutputPath->setText(outputPath);

  // set the proper radio button
  int id = config.readNumEntry("PrintOutput", GrpOutputSettings->id(RdoOutputPrinter));
  GrpOutputSettings->setButton(id);

  // photo size
  QString photoSize = config.readEntry("PhotoSize");
  QListBoxItem *item = ListPhotoSizes->findItem(photoSize);
  if (item)
    ListPhotoSizes->setCurrentItem(item);
  else
    ListPhotoSizes->setCurrentItem(0);

  EditCopies->setValue(config.readNumEntry("Copies", EditCopies->value()));
}


// save the current wizard settings
void FrmPrintWizard::saveSettings()
{
  KSimpleConfig config("kipirc");
  config.setGroup("PrintWizard");

  config.writeEntry("PageSize", (int)m_pageSize);

  // output
  int output = 0;
  if (RdoOutputPrinter->isChecked())
    output = GrpOutputSettings->id(RdoOutputPrinter);
  else
    if (RdoOutputFile->isChecked())
      output = GrpOutputSettings->id(RdoOutputFile);
    else
      if (RdoOutputGimp->isChecked())
        output = GrpOutputSettings->id(RdoOutputGimp);
  config.writeEntry("PrintOutput", output);

  // image captions
  config.writeEntry("ImageCaptions", buttonGroupSelectedId(GrpImageCaptions));

  // output path
  config.writePathEntry("OutputPath", EditOutputPath->text());

  // photo size
  config.writeEntry("PhotoSize", ListPhotoSizes->currentText());
  config.writeEntry("Copies", EditCopies->value());

}

void FrmPrintWizard::GrpOutputSettings_clicked(int id)
{
  this->nextButton()->setEnabled(false);
  // disable next buttons when appropriate
  if (id == GrpOutputSettings->id(RdoOutputPrinter))
    this->nextButton()->setEnabled(true);
  else
  if (id == GrpOutputSettings->id(RdoOutputFile))
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
}

void FrmPrintWizard::BtnBrowseOutputPath_clicked( void )
{
  QString newPath = KFileDialog::getExistingDirectory(EditOutputPath->text(), this, "Select Output Folder");
  if (newPath.isEmpty())
    return;
  // assume this directory exists
  EditOutputPath->setText(newPath);
  GrpOutputSettings_clicked(GrpOutputSettings->id(GrpOutputSettings->selected()));
}

void FrmPrintWizard::EditOutputPath_textChanged(const QString &)
{
  GrpOutputSettings_clicked(GrpOutputSettings->id(GrpOutputSettings->selected()));
}

void FrmPrintWizard::CmbPaperSize_activated( int index )
{
  KPrinter::PageSize pageSize = KPrinter::Letter;
  switch(index)
  {
    case 0 : pageSize = KPrinter::Letter;
             break;
    case 1 : pageSize = KPrinter::A4;
             break;
    case 2 : pageSize = KPrinter::A6;
             break;
  }
  initPhotoSizes(pageSize);
}

void FrmPrintWizard::BtnPrintOrderUp_clicked() {
  if (ListPrintOrder->currentItem() == 0)
    return;

  int currentIndex = ListPrintOrder->currentItem();
  QString item1 = ListPrintOrder->selectedItem()->text();
  QString item2 = ListPrintOrder->item(currentIndex - 1)->text();

  // swap these items
  ListPrintOrder->changeItem(item2, currentIndex);
  ListPrintOrder->changeItem(item1, currentIndex - 1);

  // the list box items are swapped, now swap the items in the photo list
  TPhoto *photo1 = m_photos.at(currentIndex);
  TPhoto *photo2 = m_photos.at(currentIndex - 1);
  m_photos.remove(currentIndex - 1);
  m_photos.remove(currentIndex - 1);
  m_photos.insert(currentIndex - 1, photo2);
  m_photos.insert(currentIndex - 1, photo1);
  previewPhotos();
}

void FrmPrintWizard::BtnPrintOrderDown_clicked() {
  if (ListPrintOrder->currentItem() == (signed int)ListPrintOrder->count() - 1)
    return;

  int currentIndex = ListPrintOrder->currentItem();
  QString item1 = ListPrintOrder->selectedItem()->text();
  QString item2 = ListPrintOrder->item(currentIndex + 1)->text();

  // swap these items
  ListPrintOrder->changeItem(item2, currentIndex);
  ListPrintOrder->changeItem(item1, currentIndex + 1);

  // the list box items are swapped, now swap the items in the photo list
  TPhoto *photo1 = m_photos.at(currentIndex);
  TPhoto *photo2 = m_photos.at(currentIndex + 1);
  m_photos.remove(currentIndex);
  m_photos.remove(currentIndex);
  m_photos.insert(currentIndex, photo1);
  m_photos.insert(currentIndex, photo2);
  previewPhotos();
}
                                                                           
void FrmPrintWizard::BtnPreviewPageDown_clicked() {
  if (m_currentPreviewPage == 0)
    return;
  m_currentPreviewPage--;
  previewPhotos();
}
void FrmPrintWizard::BtnPreviewPageUp_clicked() {
  if (m_currentPreviewPage == getPageCount() - 1)
    return;
  m_currentPreviewPage++;
  previewPhotos();
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

void FrmPrintWizard::initPhotoSizes(KPrinter::PageSize pageSize)
{
  // don't refresh anything if we haven't changed page sizes.
  if (pageSize == m_pageSize)
    return;

  m_pageSize = pageSize;
  m_photoSizes.clear();
  // ====================== LETTER SIZE =====================
  if (pageSize == KPrinter::Letter)
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
    m_photoSizes.append(p);

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
    m_photoSizes.append(p);

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
    m_photoSizes.append(p);

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
    m_photoSizes.append(p);

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
    m_photoSizes.append(p);

    // thumbnails
    m_photoSizes.append(createPhotoGrid(8500, 11000, i18n("Thumbnails"), 5, 4));

    // small thumbnails
    m_photoSizes.append(createPhotoGrid(8500, 11000, i18n("Small Thumbnails"), 6, 5));

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
    m_photoSizes.append(p);
    
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
    m_photoSizes.append(p);
    

  } // letter

  // ====================== A4 SIZE =====================
  else if (pageSize == KPrinter::A4)
  {
    // A4 is 21 x 29.7cm
    TPhotoSize *p;
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
    m_photoSizes.append(p);

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
    m_photoSizes.append(p);

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
    m_photoSizes.append(p);

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
    m_photoSizes.append(p);

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
    m_photoSizes.append(p);
    
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
    m_photoSizes.append(p);

    // thumbnails
    m_photoSizes.append(createPhotoGrid(2100, 2970, i18n("Thumbnails"), 5, 4));

    // small thumbnails
    m_photoSizes.append(createPhotoGrid(2100, 2970, i18n("Small Thumbnails"), 6, 5));

  } // A4
  // ====================== A6 SIZE =====================
  else if (pageSize == KPrinter::A6)
  {
      // A6 is 10.5 x 14.8 cm
      // but let's pretend it is 10.2 x 15.3 cm
      TPhotoSize *p;
      // ========== 9x13
      p = new TPhotoSize;
      p->dpi = 0;
      p->autoRotate = true;
      p->label = i18n("9 x 13cm");
      // page size
      //    p->layouts.append(new QRect(0, 0, 1050, 1480));
      p->layouts.append(new QRect(0, 0, 1020, 1530));
      // photo layouts
      p->layouts.append(new QRect( 50,  100, 900, 1300));
      // add to the list
      m_photoSizes.append(p);
                             
      // ========== 10x15cm
      p = new TPhotoSize;
      p->dpi = 0;
      p->autoRotate = true;
      p->label = i18n("10 x 15cm");
      // page size
      //     p->layouts.append(new
      //     QRect(0, 0, 1050, 1480));
      p->layouts.append(new QRect(0, 0, 1020, 1530));
      // photo layouts
      p->layouts.append(new QRect( 0, 0, 1020, 1530));
      // add to the list
      m_photoSizes.append(p);
                                                         
      // thumbnails
      m_photoSizes.append(createPhotoGrid(1020, 1530, i18n("Thumbnails"), 5, 4));

      // small thumbnails
      m_photoSizes.append(createPhotoGrid(1020, 1530, i18n("Small Thumbnails"), 6, 5));

  } // A6
  // ====================== UNSUPPORTED SIZE =====================
  else
  {
    kdDebug( 51000 ) << "Initializing Unsupported page layouts\n";
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
    m_photoSizes.append(p);
  }

  // load the photo sizes into the listbox
  ListPhotoSizes->clear();
  for (TPhotoSize *s = m_photoSizes.first(); s != 0; s = m_photoSizes.next())
    ListPhotoSizes->insertItem(s->label);
  ListPhotoSizes->setCurrentItem(0);
}

}  // NameSpace KIPIPrintWizardPlugin

#include "frmprintwizard.moc"
