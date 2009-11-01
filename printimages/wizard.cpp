/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-01-11
 * Description : a kipi plugin to print images
 *
 * Copyright 2008-2009 by Angelo Naselli <anaselli at linux dot it>
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

// STD

#include <memory>

// Qt includes

#include <QFileInfo>
#include <QPainter>
#include <QPalette>
#include <QtGlobal>
#include <QPrinter>
#include <QPrintDialog>
#include <QPageSetupDialog>
#include <QPrinterInfo>
#include <QProgressDialog>
#include <QDomDocument>

// KDE includes

#include <kapplication.h>
#include <kconfigdialogmanager.h>
#include <khelpmenu.h>
#include <kmenu.h>
#include <kpushbutton.h>
#include <ktoolinvocation.h>
#include <kfile.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kdeprintdialog.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kconfig.h>
#include <kdesktopfile.h>

// libkipi includes

#include <libkipi/imagecollectionselector.h>
#include <libkipi/interface.h>

// libkexiv2 includes

#include <libkexiv2/kexiv2.h>

// Local includes

#include "kpaboutdata.h"
#include "ui_croppage.h"
#include "ui_infopage.h"
#include "ui_photopage.h"
#include "tphoto.h"
#include "utils.h"
#include "templateicon.h"

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

const char* infoPageName=  I18N_NOOP("Select printing information");
const char* photoPageName=  I18N_NOOP("Select page layout");
const char* cropPageName=  I18N_NOOP("Crop photos");

typedef WizardPage<Ui_InfoPage>  InfoPage;
typedef WizardPage<Ui_PhotoPage> PhotoPage;
typedef WizardPage<Ui_CropPage>  CropPage;


// Wizard implementation
struct Wizard::Private
{
  InfoPage  *mInfoPage;
  PhotoPage *mPhotoPage;
  CropPage  *mCropPage;

  KPushButton  *m_helpButton;
  
  KIPI::ImageCollectionSelector* mCollectionSelector;
  KIPI::Interface* mInterface;

  KIPIPlugins::KPAboutData* mAbout;

  QSizeF             m_pageSize;
  QList<TPhoto*>     m_photos;
  QList<TPhotoSize*> m_photoSizes;
  int                m_infopage_currentPhoto;
  int                m_currentPreviewPage;
  int                m_currentCropPhoto;
  bool               m_cancelPrinting;
  QString            m_tempPath;
  QStringList        m_gimpFiles;
  QString            m_savedPhotoSize;

  //QPrintDialog      *m_printDialog;
  QPageSetupDialog    *m_pDlg;
  QPrinter            *m_printer;
  QList<QPrinterInfo>  m_printerList;
};

Wizard::Wizard ( QWidget* parent, KIPI::Interface* interface )
      : KAssistantDialog ( parent )
{
  d=new Private;
  d->mInterface    = interface;
  //d->m_printDialog = NULL;
  d->m_pDlg        = NULL;
  d->m_printer     = NULL;
  d->m_infopage_currentPhoto = 0;

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
  d->mAbout->addAuthor ( ki18n ( "Andreas Trink" ), ki18n ( "Contributor" ),
                          "atrink@nociaro.org" );

  d->mInfoPage  = new InfoPage ( this, i18n ( infoPageName ) );
  d->mPhotoPage = new PhotoPage ( this, i18n ( photoPageName ) );
  d->mCropPage  = new CropPage ( this, i18n ( cropPageName ) ) ;

  d->m_helpButton = button ( Help );
  KHelpMenu* helpMenu = new KHelpMenu ( this, d->mAbout, false );
  helpMenu->menu()->removeAction ( helpMenu->menu()->actions().first() );
  QAction *handbook   = new QAction ( i18n ( "Handbook" ), this );

//     // create a QButtonGroup to manage button ids
//     d->m_outputSettings = new QButtonGroup ( this );
//     d->m_outputSettings->addButton ( d->mInfoPage->RdoOutputPrinter, ToPrinter );
//     d->m_outputSettings->addButton ( d->mInfoPage->RdoOutputGimp,    ToGimp );
//     d->m_outputSettings->addButton ( d->mInfoPage->RdoOutputFile,    ToFile );

  //TODO
  d->m_pageSize = QSizeF(-1,-1); // select a different page to force a refresh in initPhotoSizes.

  QList<QPrinterInfo>::iterator it;
  d->m_printerList = QPrinterInfo::availablePrinters ();
  kDebug() << " printers: " << d->m_printerList.count();
  //d->mInfoPage->m_printer_choice->setInsertPolicy(QComboBox::InsertAtTop/*QComboBox::InsertAlphabetically*/); 
  
  for ( it = d->m_printerList.begin();
        it != d->m_printerList.end(); ++it )
  {
    kDebug() << " printer: " << it->printerName ();
    d->mInfoPage->m_printer_choice->addItem(it->printerName ());
  }

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
  connect ( d->mInfoPage->m_FreeCaptionFormat , SIGNAL ( editingFinished () ),
            this, SLOT ( infopage_updateCaptions ( ) ) );
  connect ( d->mInfoPage->m_sameCaption , SIGNAL ( stateChanged (int) ),
            this, SLOT ( infopage_updateCaptions ( ) ) );
  connect ( d->mInfoPage->m_font_name , SIGNAL (  currentFontChanged ( const QFont & ) ),
            this, SLOT ( infopage_updateCaptions ( ) ) );
  connect ( d->mInfoPage->m_font_size , SIGNAL ( valueChanged(int) ),
            this, SLOT ( infopage_updateCaptions ( ) ) );
  connect ( d->mInfoPage->m_font_color , SIGNAL ( changed (const QColor &) ),
            this, SLOT ( infopage_updateCaptions ( ) ) );
  
  connect ( d->mInfoPage->m_setDefault , SIGNAL ( clicked () ),
            this, SLOT ( saveCaptionSettings ( ) ) );
  
  // printer
  connect (d->mInfoPage->m_printer_choice, SIGNAL (activated ( const QString & ) ),
            this, SLOT ( outputChanged ( const QString & ) ) );

  connect ( d->mInfoPage->m_preview_right, SIGNAL ( clicked() ),
            this, SLOT ( infopage_selectNext()) );

  connect ( d->mInfoPage->m_preview_left, SIGNAL ( clicked() ),
            this, SLOT ( infopage_selectPrev()) );
            
  connect ( d->mInfoPage->m_increase_copies, SIGNAL ( clicked() ),
            this, SLOT ( infopage_increaseCopies() ) );
  
  connect ( d->mInfoPage->m_decrease_copies, SIGNAL ( clicked() ),
            this, SLOT ( infopage_decreaseCopies() ) );
  
  connect ( d->mInfoPage->m_PictureInfo, SIGNAL ( itemSelectionChanged() ),
            this, SLOT ( infopage_imageSelected() ) );
  
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

  connect ( d->mPhotoPage->ListPhotoSizes, SIGNAL ( currentRowChanged ( int ) ),
            this, SLOT ( ListPhotoSizes_selected() ) );

  // don't crop
  connect ( d->mCropPage->m_disableCrop, SIGNAL ( stateChanged ( int ) ),
            this, SLOT ( crop_selection ( int ) ) );

  // remove a page
  connect ( this, SIGNAL ( pageRemoved ( KPageWidgetItem * ) ),
            this, SLOT ( PageRemoved ( KPageWidgetItem * ) ) );

  connect ( d->mInfoPage->m_pagesetup, SIGNAL ( clicked () ),
            this, SLOT ( pagesetupclicked () ) );

  d->m_currentPreviewPage = 0;
  d->m_currentCropPhoto   = 0;
  d->m_cancelPrinting     = false;

  helpMenu->menu()->insertAction ( helpMenu->menu()->actions().first(), handbook );
  d->m_helpButton->setMenu ( helpMenu->menu() );
}

Wizard::~Wizard()
{
  // TODO private object could be deleted inside private destructor
  delete d->mAbout;
  delete d->m_pDlg;
  delete d->m_printer;
  for ( int i=0; i < d->m_photos.count(); i++ )
    if ( d->m_photos.at ( i ) )
      delete d->m_photos.at ( i );
  d->m_photos.clear();
  delete d;
}

// create a MxN grid of photos, fitting on the page
void createPhotoGrid ( TPhotoSize *p, int pageWidth, int pageHeight, int rows, int columns,  TemplateIcon *iconpreview )
{
  int MARGIN = ( int ) ( ( pageWidth + pageHeight ) / 2 * 0.04 + 0.5 );
  int GAP = MARGIN / 4;
  int photoWidth = ( pageWidth - ( MARGIN * 2 ) - ( ( columns-1 ) * GAP ) ) / columns;
  int photoHeight = ( pageHeight - ( MARGIN * 2 ) - ( ( rows-1 ) * GAP ) ) / rows;

  int row = 0;
  for ( int y=MARGIN; row < rows && y < pageHeight - MARGIN; y += photoHeight + GAP )
  {
    int col = 0;
    for ( int x=MARGIN; col < columns && x < pageWidth - MARGIN; x += photoWidth + GAP )
    {
      p->layouts.append ( new QRect ( x, y, photoWidth, photoHeight ) );
      iconpreview->fillRect( x, y, photoWidth, photoHeight, Qt::color1 );
      col++;
    }
    row++;
  }
}

void Wizard::print ( KUrl::List fileList, QString tempPath )
{
  for ( int i=0; i < d->m_photos.count(); i++ )
    if ( d->m_photos.at ( i ) )
      delete d->m_photos.at ( i );
  d->m_photos.clear();
  d->mPhotoPage->ListPrintOrder->clear();
  d->mInfoPage->m_PictureInfo->setRowCount(fileList.count());
  for ( int i=0; i < fileList.count(); i++ )
  {
    TPhoto *photo = new TPhoto ( 150 );
    photo->filename = fileList[i];
    photo->first = true;
    d->m_photos.append ( photo );
    //FileName
    QTableWidgetItem *newItem = new QTableWidgetItem(photo->filename.fileName());
    d->mInfoPage->m_PictureInfo->setItem(i, 0, newItem); //setItem(row, column, newItem);
    // Number of copies
    newItem = new QTableWidgetItem(tr("%1").arg(photo->copies));
    d->mInfoPage->m_PictureInfo->setItem(i, 1, newItem);
    // load the print order listbox
//TODO      d->mPhotoPage->ListPrintOrder->addItem ( photo->filename.fileName() );
  }
//TODO    d->mPhotoPage->ListPrintOrder->setCurrentRow ( 0, QItemSelectionModel::Select );
  
  d->mInfoPage->m_PictureInfo->setCurrentCell(0,0);
  
  d->m_tempPath = tempPath;
//TODO    d->mPhotoPage->LblPhotoCount->setText ( QString::number ( d->m_photos.count() ) );

// TODO move right to pageChanged()
  d->mCropPage->BtnCropPrev->setEnabled ( false );

  if ( d->m_photos.count() == 1 )
    d->mCropPage->BtnCropNext->setEnabled ( false );

  // setCurrentPage should emit currentPageChanged nut it seems not to at the moment
  // setCurrentPage(d->mInfoPage->page());
  currentPageChanged(d->mInfoPage->page(), NULL);

}


void Wizard::parseTemplateFile( QString fn, QSizeF pageSize )
{
  QDomDocument doc("mydocument"); 
  kDebug() << " XXX: " <<  fn;

  if(fn.isEmpty()) {
    return;
  }
  QFile file( fn ); 

  if (!file.open(QIODevice::ReadOnly)) 
    return; 

  if (!doc.setContent(&file)) { 
    file.close(); 
    return; 
  } 
  file.close();

  TPhotoSize *p;
  
  // print out the element names of all elements that are direct children
  // of the outermost element.
  QDomElement docElem = doc.documentElement();
  kDebug() << docElem.tagName(); // the node really is an element.

  QSizeF size;
  QString unit;
  int scaleValue;
  QDomNode n = docElem.firstChild();
  while(!n.isNull())
  {
    size = QSizeF(0,0);
    scaleValue = 10; // 0.1 mm
    QDomElement e = n.toElement(); // try to convert the node to an element.
    if(!e.isNull())
    {
      if( e.tagName() == "paper" )
      {
        size = QSizeF(e.attribute("width", "0").toFloat(), e.attribute("height", "0").toFloat());
        unit = e.attribute("unit", "mm");
        kDebug() <<  e.tagName() << " name=" << e.attribute("name","??")
        << " size= " << size
        << " unit= " << unit;


        if (size == QSizeF(0.0,0.0) && size == pageSize)
        {
          // skipping templates without page size since pageSize is not set
          n = n.nextSibling();
          continue;
        }
        else if (unit != "mm" && size != QSizeF(0.0,0.0)) // "cm", "inches" or "inch"
        {
          // convert to mm
          if (unit == "inches" ||  unit == "inch")
          {
            size *= 25.4;
            scaleValue = 1000;
            kDebug() << "template size " << size << " page size " << pageSize;
          }
          else if (unit == "cm")
          {
            size *= 10;
            scaleValue = 100;
              kDebug() << "template size " << size << " page size " << pageSize;
          }
          else
          {
            kWarning() << "Wrong unit " << unit << " skipping layout";
            n = n.nextSibling();
            continue;
          }
        }

        static const float round_value = 0.01F;
        if (size == QSizeF(0,0) )
        {
          size = pageSize;
          unit = "mm";
        }
        else if (pageSize     != QSizeF(0,0) &&
                  (size.height() > (pageSize.height() +round_value) ||
                  size.width()  > (pageSize.width() +round_value)))
        {
          kDebug() << "skipping size " << size << " page size " << pageSize;
          // skipping layout it can't fit
          n = n.nextSibling();
          continue;
        }

        // Next templates are good we restore size to its unit
//           if (unit != "mm")
//           {
//             size = QSizeF(e.attribute("width", "0").toFloat(), e.attribute("height", "0").toFloat());
//           }

        kDebug() << "layout size " << size << " page size " << pageSize;

        QDomNode np = e.firstChild();
        while(!np.isNull())
        {
          QDomElement ep = np.toElement(); // try to convert the node to an element.
          if(!ep.isNull())
          {
            if( ep.tagName() == "template" )
            {
              p = new TPhotoSize;

              QSizeF sizeManaged;
              // set page size
              if (pageSize == QSizeF(0,0))
              {
                sizeManaged = size * scaleValue;
                //p->layouts.append ( new QRect ( 0, 0, size.width()*scaleValue, size.height()*scaleValue ) );
              }
              else if (unit == "inches" || unit == "inch")
              {
                sizeManaged = pageSize * scaleValue / 25.4;
//                   p->layouts.append ( new QRect ( 0, 0, (pageSize.width()*scaleValue/25.4),
//                                                   (pageSize.height()*scaleValue/25.4) ) );
              }
              else
              {
                sizeManaged = pageSize * 10;
//                   p->layouts.append ( new QRect ( 0, 0, pageSize.width()*10, pageSize.height()*10 ) );
              }
              
              p->layouts.append ( new QRect ( 0, 0, (int)sizeManaged.width(), (int)sizeManaged.height() ) );
              // create a small preview of the template
              // TODO check if iconsize here is useless
              TemplateIcon iconpreview( 80, sizeManaged.toSize() );
              iconpreview.begin();
              
              QString desktopFileName = QString("kipiplugin_printimages/templates/") +
              QString(ep.attribute("name","XXX")) + ".desktop";
              
              kDebug() <<  "template desktop file name" << desktopFileName;
              
              const QStringList list=KGlobal::dirs()->findAllResources("data", desktopFileName);
              QStringList::ConstIterator it=list.constBegin(), end=list.constEnd();
              if (it != end)
                p->label = KDesktopFile(*it).readName();
              else
              {
                p->label = ep.attribute("name","XXX");  // FIXME i18n()
                kWarning() << "missed template tranlation " << desktopFileName;
              }
              p->dpi = ep.attribute("dpi","0").toInt();
              p->autoRotate = (ep.attribute("autorotate","false") == "true") ? true : false;
              
              QDomNode nt = ep.firstChild();
              while(!nt.isNull())
              {
                QDomElement et = nt.toElement(); // try to convert the node to an element.
                if(!et.isNull())
                {
                  if( et.tagName() == "photo" )
                  {
                    float value = et.attribute("width","0").toFloat();
                    int width   = (int)(value == 0 ? size.width() : value)*scaleValue;
                    value       = et.attribute("height","0").toFloat();
                    int height  = (int)(value == 0 ? size.height() : value)*scaleValue;
                    int photoX  = (int)(et.attribute("x","0").toFloat()*scaleValue);
                    int photoY  = (int)(et.attribute("y","0").toFloat()*scaleValue);
                    p->layouts.append( new QRect ( photoX,
                                                    photoY,
                                                    width, height) );
                    iconpreview.fillRect( photoX, photoY, width, height, Qt::color1 );                               
                  }
                  else if( et.tagName() == "photogrid" )
                  {
                    float value    = et.attribute("pageWidth","0").toFloat();
                    int pageWidth  = (int)(value == 0 ? size.width() : value)*scaleValue;
                    value          = et.attribute("pageHeight","0").toFloat();
                    int pageHeight = (int)(value == 0 ? size.height() : value)*scaleValue;
                    int rows       = et.attribute("rows","0").toInt();
                    int columns    = et.attribute("columns","0").toInt();
                    if (rows >0 && columns >0 )
                    {
                      createPhotoGrid(p, pageWidth,
                                        pageHeight,
                                        rows,
                                        columns,
                                        &iconpreview);
                    }
                    else
                    {
                      kWarning() << " Wrong grid configuration, rows " << rows <<
                      ", columns " << columns;
                    }
                  }
                  else {
                    kDebug() << "    " <<  et.tagName();
                  }
                }
                nt = nt.nextSibling();
              }
              iconpreview.end();
              p->icon = iconpreview.getIcon();
              
              d->m_photoSizes.append ( p );
            }
            else
            {
              kDebug() << "? " <<  ep.tagName() << " attr=" << ep.attribute("name","??");
            }
            
          }
          np = np.nextSibling();
        }
        
      }
      else
      {
        kDebug() << "??" << e.tagName() << " name=" << e.attribute("name","??");
      }
    }
    n = n.nextSibling();
  }
}


void Wizard::initPhotoSizes ( QSizeF pageSize )
{
  kDebug() << "New page size " << pageSize
                << ", old page size " << d->m_pageSize;
  
  // don't refresh anything if we haven't changed page sizes.
  if ( pageSize == d->m_pageSize )
    return;

  d->m_pageSize = pageSize;

  // cleaning m_pageSize memory before invoking clear()
  for ( int i=0; i < d->m_photoSizes.count(); i++ )
    if ( d->m_photoSizes.at ( i ) )
      delete d->m_photoSizes.at ( i );
  d->m_photoSizes.clear();

  // get template-files and parse them
  const QStringList list=KGlobal::dirs()->findAllResources("data", 
        "kipiplugin_printimages/templates/*.xml");

  foreach( const QString& fn, list) {
      kDebug() << " LIST: " <<  fn;
      parseTemplateFile( fn, pageSize );
  }

  kDebug() << "d->m_photoSizes.count()=" << d->m_photoSizes.count();
  kDebug() << "d->m_photoSizes.isEmpty()=" << d->m_photoSizes.isEmpty();

  if( d->m_photoSizes.isEmpty() ) {
    kDebug() << "Empty photoSize-list, create default size\n";
    // There is no valid page size yet.  Create a default page (B10) to prevent crashes.
    TPhotoSize *p;
    p = new TPhotoSize;
    p->dpi = 0;
    p->autoRotate = false;
    p->label = i18n ( "Unsupported Paper Size" );
    // page size: B10 (32 x 45 mm)
    p->layouts.append ( new QRect ( 0, 0, 3200, 4500 ) );
    p->layouts.append ( new QRect ( 0, 0, 3200, 4500 ) );
    // add to the list
    d->m_photoSizes.append ( p );
  }    
        
  // load the photo sizes into the listbox
  d->mPhotoPage->ListPhotoSizes->blockSignals ( true );
  d->mPhotoPage->ListPhotoSizes->clear();
  QList<TPhotoSize*>::iterator it;
  for ( it = d->m_photoSizes.begin(); it != d->m_photoSizes.end(); ++it )
  {
    TPhotoSize *s = static_cast<TPhotoSize*> ( *it );
    if ( s ) 
    {
      QListWidgetItem *pWItem = new QListWidgetItem ( s->label );
      pWItem->setIcon( s->icon );
      d->mPhotoPage->ListPhotoSizes->addItem ( pWItem );
    }
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

void Wizard::printCaption ( QPainter &p, TPhoto* photo, int captionW, int captionH, QString caption )
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

  QFont font ( photo->pCaptionInfo->m_caption_font );
  font.setStyleHint ( QFont::SansSerif );
  font.setPixelSize ( ( int ) ( captionH * FONT_HEIGHT_RATIO ) );
  font.setWeight ( QFont::Normal );

  QFontMetrics fm ( font );
  int pixelsHigh = fm.height();

  p.setFont ( font );
  p.setPen ( photo->pCaptionInfo->m_caption_color );
  kDebug() << "Number of lines " << ( int ) captionByLines.count() ;

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



QString Wizard::captionFormatter (TPhoto *photo)
{
  if (!photo->pCaptionInfo)
    return QString ();
  
  QString format;
  switch ( photo->pCaptionInfo->m_caption_type )
  {
    case CaptionInfo::FileNames:
      format = "%f";
      break;
    case CaptionInfo::ExifDateTime:
      format = "%d";
      break;
    case CaptionInfo::Comment:
      format = "%c";
      break;
    case CaptionInfo::Free:
      format =  photo->pCaptionInfo->m_caption_text;
      break;
    default:
      kWarning () << "UNKNOWN caption type " << photo->pCaptionInfo->m_caption_type; 
      break;
  }

  QFileInfo fi ( photo->filename.path() );
  QString resolution;
  QSize imageSize =  photo->exiv2Iface()->getImageDimensions();
  if ( imageSize.isValid() )
  {
    resolution = QString ( "%1x%2" ).arg ( imageSize.width() ).arg ( imageSize.height() );
  }
  format.replace ( "\\n", "\n" );

  // %f filename
  // %c comment
  // %d date-time
  // %t exposure time
  // %i iso
  // %r resolution
  // %a aperture
  // %l focal length
  format.replace ( "%f", fi.fileName() );
  format.replace ( "%c", photo->exiv2Iface()->getExifComment() );
  format.replace ( "%d", KGlobal::locale()->formatDateTime ( photo->exiv2Iface()->getImageDateTime() ) );
  format.replace ( "%t", photo->exiv2Iface()->getExifTagString ( "Exif.Photo.ExposureTime" ) );
  format.replace ( "%i", photo->exiv2Iface()->getExifTagString ( "Exif.Photo.ISOSpeedRatings" ) );
  format.replace ( "%r", resolution );
  format.replace ( "%a", photo->exiv2Iface()->getExifTagString ( "Exif.Photo.FNumber" ) );
  format.replace ( "%l", photo->exiv2Iface()->getExifTagString ( "Exif.Photo.FocalLength" ) );

  return format;
}

bool Wizard::paintOnePage ( QPainter &p, QList<TPhoto*> photos, QList<QRect*> layouts,
                              int &current, bool cropDisabled, bool useThumbnails )
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
    else if ( !cropDisabled)      //d->mCropPage->m_disableCrop->isChecked() )
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
    if ( cropDisabled) //->mCropPage->m_disableCrop->isChecked() )
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

    if ( photo->pCaptionInfo && photo->pCaptionInfo->m_caption_type != CaptionInfo::NoCaptions)
    {
      p.save();
      QString caption;
      caption = captionFormatter(photo);
      kDebug() << "Caption " << caption ;

      // draw the text at (0,0), but we will translate and rotate the world
      // before drawing so the text will be in the correct location
      // next, do we rotate?
      int captionW = w-2;
      double ratio =   photo->pCaptionInfo->m_caption_size * 0.01;
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
      kDebug() << "rotation " << photo->rotation << " orientation " << orientatation ;
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


void Wizard::updateCropFrame ( TPhoto *photo, int photoIndex )
{
  TPhotoSize *s = d->m_photoSizes.at ( d->mPhotoPage->ListPhotoSizes->currentRow() );
  d->mCropPage->cropFrame->init ( photo, getLayout ( photoIndex )->width(), getLayout ( photoIndex )->height(), s->autoRotate );
  d->mCropPage->LblCropPhoto->setText ( i18n ( "Photo %1 of %2", photoIndex + 1, QString::number ( d->m_photos.count() ) ) );
}

// update the pages to be printed and preview first/last pages
void Wizard::previewPhotos()
{
  //Change cursor to waitCursor during transition
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  
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
  paintOnePage ( p, d->m_photos, s->layouts, current, d->mCropPage->m_disableCrop->isChecked(), true );
  p.end();
  d->mPhotoPage->BmpFirstPagePreview->setPixmap ( QPixmap::fromImage(img) );
  d->mPhotoPage->LblPreview->setText ( i18n ( "Page %1 of %2", d->m_currentPreviewPage + 1, getPageCount() ) );

  manageBtnPreviewPage();
  manageBtnPrintOrder();
  QApplication::restoreOverrideCursor();
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

void Wizard::infopage_enableButtons()
{
    if (d->m_photos.size() == 1)
    {
        d->mInfoPage->m_preview_left->setEnabled(false);
        d->mInfoPage->m_preview_right->setEnabled(false);
    }
    else if (d->m_infopage_currentPhoto == 0)
    {
        d->mInfoPage->m_preview_left->setEnabled(false);
        d->mInfoPage->m_preview_right->setEnabled(true);
    }
    else if (d->m_infopage_currentPhoto == d->m_photos.size()-1)
    {
        d->mInfoPage->m_preview_right->setEnabled(false);
        d->mInfoPage->m_preview_left->setEnabled(true);
    }
    else
    {
        d->mInfoPage->m_preview_left->setEnabled(true);
        d->mInfoPage->m_preview_right->setEnabled(true);
    }
}

void Wizard::infopage_setCaptionButtons()
{
  if (d->m_photos.size())
  {
    TPhoto *pPhoto = d->m_photos.at(d->m_infopage_currentPhoto);
    if (pPhoto && !d->mInfoPage->m_sameCaption->isChecked())
    {
      infopage_blockCaptionButtons();
      if (pPhoto->pCaptionInfo)
      {
        d->mInfoPage->m_font_color->setColor(pPhoto->pCaptionInfo->m_caption_color);
        d->mInfoPage->m_font_size->setValue(pPhoto->pCaptionInfo->m_caption_size);
        d->mInfoPage->m_font_name->setCurrentFont(pPhoto->pCaptionInfo->m_caption_font);
        d->mInfoPage->m_captions->setCurrentIndex(int(pPhoto->pCaptionInfo->m_caption_type));
        d->mInfoPage->m_FreeCaptionFormat->setText(pPhoto->pCaptionInfo->m_caption_text);
      }
      else
      {
        infopage_readCaptionSettings();
        captionChanged ( d->mInfoPage->m_captions->currentText() );
      }
      infopage_blockCaptionButtons(false);
    }
  }
}


void Wizard::infopage_imageSelected()
{
  d->mInfoPage->m_PictureInfo->blockSignals(true);
  kDebug() << " current row now is " << d->mInfoPage->m_PictureInfo->currentRow();
  d->m_infopage_currentPhoto = d->mInfoPage->m_PictureInfo->currentRow();
  d->mInfoPage->m_PictureInfo->setCurrentCell(d->m_infopage_currentPhoto,0);
  d->mInfoPage->m_PictureInfo->blockSignals(false);
  
  infopage_setCaptionButtons();   
  infopage_imagePreview();
  infopage_enableButtons();
}


void Wizard::infopage_imagePreview()
{
  //Change cursor to waitCursor during transition
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  if (d->m_photos.size())
  {
    TPhoto *pPhoto = d->m_photos.at(d->m_infopage_currentPhoto);
    //d->mInfoPage->m_preview->setPixmap ( QPixmap::fromImage( pPhoto->loadPhoto().scaled(d->mInfoPage->m_preview->size(), Qt::KeepAspectRatio)) );
    
    //create a fake layout to get paintOnePage working
    QImage img ( d->mInfoPage->m_preview->size(), QImage::Format_ARGB32_Premultiplied );
    //QPixmap pixmap ( d->mInfoPage->m_preview->size() );
    QList<QRect*> layouts;
    QRect previewRect =  d->mInfoPage->m_preview->rect();
    layouts.append(&previewRect);
    layouts.append(&previewRect);
    pPhoto->cropRegion.setRect ( -1, -1, -1, -1 );
    pPhoto->rotation = 0;
    int w = previewRect.width();
    int h = previewRect.height();
    d->mCropPage->cropFrame->init ( pPhoto, w, h, true, false );
    QList<TPhoto*> photos;
    photos.append(pPhoto);
    int current = 0;
    QPainter p;
    
    p.begin ( &img);
    p.fillRect ( previewRect, Qt::transparent );
    p.setCompositionMode( QPainter::CompositionMode_SourceOver );

    paintOnePage (p, photos, layouts, current, true, false);
    p.end();
    d->mInfoPage->m_preview->setPixmap ( QPixmap::fromImage(img));
    d->mInfoPage->m_preview->update();
  }
  
  QApplication::restoreOverrideCursor();
}

void Wizard::infopage_selectNext()
{
  if (d->m_infopage_currentPhoto+1 < d->m_photos.size())
      d->m_infopage_currentPhoto++;

  d->mInfoPage->m_PictureInfo->blockSignals(true);
  d->mInfoPage->m_PictureInfo->setCurrentCell(d->m_infopage_currentPhoto,0);
  d->mInfoPage->m_PictureInfo->blockSignals(false);

  infopage_setCaptionButtons();
  infopage_imagePreview();
  infopage_enableButtons();
}

void Wizard::infopage_selectPrev()
{
  if (d->m_infopage_currentPhoto-1 >= 0)
      d->m_infopage_currentPhoto--;

  d->mInfoPage->m_PictureInfo->blockSignals(true);
  d->mInfoPage->m_PictureInfo->setCurrentCell(d->m_infopage_currentPhoto,0);
  d->mInfoPage->m_PictureInfo->blockSignals(false);

  infopage_setCaptionButtons();    
  infopage_imagePreview();
  infopage_enableButtons();
}

void Wizard::infopage_decreaseCopies()
{
  if (d->m_photos.size())
  {
    TPhoto *pPhoto = d->m_photos.at(d->m_infopage_currentPhoto);
    if (pPhoto->copies>1)
    {
      pPhoto->copies--;
    
      d->mInfoPage->m_PictureInfo->blockSignals(true);
      QTableWidgetItem* newItem = new QTableWidgetItem(tr("%1").arg(pPhoto->copies));
      d->mInfoPage->m_PictureInfo->setItem(d->m_infopage_currentPhoto, 1, newItem);
      d->mInfoPage->m_PictureInfo->blockSignals(false);
      //d->mInfoPage->m_preview->setPixmap ( QPixmap::fromImage( pPhoto->loadPhoto().scaled(d->mInfoPage->m_preview->size(),Qt::KeepAspectRatioByExpanding)) );
    }
  }
}

void Wizard::infopage_increaseCopies()
{
  if (d->m_photos.size())
  {
    TPhoto *pPhoto = d->m_photos.at(d->m_infopage_currentPhoto);
    pPhoto->copies++;
    
    d->mInfoPage->m_PictureInfo->blockSignals(true);
    QTableWidgetItem* newItem = new QTableWidgetItem(tr("%1").arg(pPhoto->copies));
    d->mInfoPage->m_PictureInfo->setItem(d->m_infopage_currentPhoto, 1, newItem);
    d->mInfoPage->m_PictureInfo->blockSignals(false);
    //d->mInfoPage->m_preview->setPixmap ( QPixmap::fromImage( pPhoto->loadPhoto().scaled(d->mInfoPage->m_preview->size(),Qt::KeepAspectRatioByExpanding)) );
  }
}

// Wizard SLOTS
void Wizard::slotHelp()
{
  KToolInvocation::invokeHelp ( "printwizard","kipi-plugins" );
}

void  Wizard::pageChanged ( KPageWidgetItem *current, KPageWidgetItem *before )
{
  //Change cursor to waitCursor during transition
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  if ( before )
  {
    saveSettings ( before->name() );
    kDebug() << " before " << before->name();
  }
  
  kDebug() << " current " << current->name();
  readSettings ( current->name() );
  
  if ( current->name() == i18n ( infoPageName ) )
  {
    // set to first photo
    d->m_infopage_currentPhoto = 0;
    
    QList<TPhoto*> photoList;
    kDebug() << "(1) n. photos: " << d->m_photos.count();
    for ( int i=0; i < d->m_photos.count(); i++)
    {
      TPhoto *pCurrentPhoto = d->m_photos.at ( i );
      kDebug() << "current photo " << pCurrentPhoto->filename.fileName();
      if (pCurrentPhoto)
      {
        if (pCurrentPhoto->first)
        {
          //add back
          photoList.append ( pCurrentPhoto );
        }
        else
        {
          // remove copies
          delete d->m_photos.at ( i );
        }
      }
    }
    // restore original photo list but preserve number of copies
    d->m_photos.clear();
    d->m_photos << photoList;
    photoList.clear();
    kDebug() << "(2) n. photos: " << d->m_photos.count();

    infopage_updateCaptions();    
    infopage_enableButtons();
  }
  else if ( current->name() == i18n ( photoPageName ) )
  {
    d->mPhotoPage->ListPrintOrder->blockSignals(true);
    d->mPhotoPage->ListPrintOrder->clear();
    for ( int i=0; i < d->m_photos.count(); )
    {
      TPhoto *pCurrentPhoto = d->m_photos.at ( i ); 
      if ( pCurrentPhoto )
      {
        d->mPhotoPage->ListPrintOrder->insertItem ( i, pCurrentPhoto->filename.fileName() );
        // adding copies
        for ( int adding = pCurrentPhoto->copies-1; adding >0 ;adding-- )
        {
          TPhoto *pPhoto = new TPhoto ( *pCurrentPhoto);
          pPhoto->first = false;
          d->m_photos.insert ( i, pPhoto );
          kDebug() << "FileName: " << pPhoto->filename.fileName();
          d->mPhotoPage->ListPrintOrder->insertItem ( i, pPhoto->filename.fileName() );
        }
        i+=pCurrentPhoto->copies;
      }
      else // useless
        i++;
    }
    d->mPhotoPage->LblPhotoCount->setText ( QString::number ( d->m_photos.count() ) );
    d->mPhotoPage->ListPrintOrder->setCurrentRow ( 0, QItemSelectionModel::Select );
    
    d->mPhotoPage->ListPrintOrder->blockSignals(false);
    
    // PhotoPage
    initPhotoSizes ( d->m_printer->paperSize(QPrinter::Millimeter) );
    // restore photoSize 
    QList<QListWidgetItem *> list = d->mPhotoPage->ListPhotoSizes->findItems ( d->m_savedPhotoSize, Qt::MatchExactly );
    if ( list.count() )
      d->mPhotoPage->ListPhotoSizes->setCurrentItem ( list[0] );
    else
      d->mPhotoPage->ListPhotoSizes->setCurrentRow ( 0 );
    // create our photo sizes list
    previewPhotos();
  }
  else if ( current->name() == i18n ( cropPageName ) )
  {
    d->m_currentCropPhoto = 0;
    TPhoto *photo = d->m_photos[d->m_currentCropPhoto];
    setBtnCropEnabled();
    this->update();
    updateCropFrame ( photo, d->m_currentCropPhoto );
  }
  QApplication::restoreOverrideCursor();
}

void Wizard::outputChanged ( const QString & text)
{
  if (text == i18n ( "Print to PDF" ) || 
      text == i18n ( "Print to JPG" ) || 
      text == i18n ( "Print to gimp" ) )
  {
    if (d->m_printer)
        delete d->m_printer;
    d->m_printer = new QPrinter();
    d->m_printer->setOutputFormat(QPrinter::PdfFormat);
  }
  else if (text == i18n ( "Print to PS" ))
  {
    if (d->m_printer)
        delete d->m_printer;
    d->m_printer = new QPrinter();
    d->m_printer->setOutputFormat(QPrinter::PostScriptFormat);
  }
  else // real printer
  {
    QList<QPrinterInfo>::iterator it;
    //m_printerList = QPrinterInfo::availablePrinters ();

    for ( it = d->m_printerList.begin();
          it != d->m_printerList.end(); ++it )
    {
      if (it->printerName () == text)
      {
          kDebug() << "Choosen printer: " << it->printerName ();
          if (d->m_printer)
            delete d->m_printer;
          d->m_printer = new QPrinter(*it);
      }
    }

    //d->m_printer->setPrinterName(text);
    d->m_printer->setOutputFormat(QPrinter::NativeFormat);
  }
  //default no margins
  d->m_printer->setFullPage ( true );
  d->m_printer->setPageMargins ( 0, 0, 0, 0, QPrinter::Millimeter );
//     this->setValid ( d->mCropPage->page(), true );
}


void Wizard::updateCaption(TPhoto *pPhoto)
{
  if (pPhoto) 
  {
    if (!pPhoto->pCaptionInfo &&
          d->mInfoPage->m_captions->currentIndex() != CaptionInfo::NoCaptions)
    {
      pPhoto->pCaptionInfo = new CaptionInfo();
    }
    else if (pPhoto->pCaptionInfo && 
              d->mInfoPage->m_captions->currentIndex() == CaptionInfo::NoCaptions)
    {
      delete pPhoto->pCaptionInfo;
      pPhoto->pCaptionInfo = NULL;
    }
    
    if (pPhoto->pCaptionInfo)
    {
      pPhoto->pCaptionInfo->m_caption_color = d->mInfoPage->m_font_color->color();
      pPhoto->pCaptionInfo->m_caption_size  = d->mInfoPage->m_font_size->value();
      pPhoto->pCaptionInfo->m_caption_font  = d->mInfoPage->m_font_name->currentFont();
      pPhoto->pCaptionInfo->m_caption_type  = (CaptionInfo::AvailableCaptions)d->mInfoPage->m_captions->currentIndex();
      pPhoto->pCaptionInfo->m_caption_text  = d->mInfoPage->m_FreeCaptionFormat->text();

    }
  }
}

void Wizard::infopage_updateCaptions()
{
  if (d->mInfoPage->m_sameCaption->isChecked())
  {
    QList<TPhoto*>::iterator it;
    for ( it = d->m_photos.begin(); it != d->m_photos.end(); ++it )
    {
      TPhoto *pPhoto = static_cast<TPhoto*> ( *it );
      updateCaption(pPhoto);    
    }
  }
  else
  {
    TPhoto *pPhoto = d->m_photos.at ( d->m_infopage_currentPhoto);
    updateCaption(pPhoto);
  }
  infopage_imagePreview();
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
  
  infopage_updateCaptions();
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

  kDebug() << "Selected photo " << currentIndex+1 << " of " << d->mPhotoPage->ListPrintOrder->count();


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

void Wizard::ListPhotoOrder_highlighted ( int /*index*/ )
{
  manageBtnPrintOrder();
}

void Wizard::ListPrintOrder_selected()
{
  manageBtnPrintOrder();
}

void Wizard::ListPhotoSizes_selected()
{
  previewPhotos();
}

void Wizard::BtnPrintOrderDown_clicked()
{
  int currentIndex = d->mPhotoPage->ListPrintOrder->currentRow();

  kDebug() << "Selected photo " << currentIndex+1 << " of " << d->mPhotoPage->ListPrintOrder->count();

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


  if ( pageName == i18n ( infoPageName ) )
  {
    // InfoPage

    // Printer
    group.writeEntry("Printer", d->mInfoPage->m_printer_choice->currentText ());
  }
  else if ( pageName == i18n ( photoPageName ) )
  {
    // PhotoPage
    // photo size
    group.writeEntry ( "PhotoSize", d->mPhotoPage->ListPhotoSizes->currentItem()->text() );
    group.writeEntry ( "IconSize",  d->mPhotoPage->ListPhotoSizes->iconSize());
  }
  else if ( pageName == i18n ( cropPageName ) )
  {
    // CropPage
    if (d->mInfoPage->m_printer_choice->currentText() == i18n("Print to JPG"))
    {
      // output path
      QString outputPath = d->mCropPage->m_outputPath->url().url();
      group.writePathEntry ( "OutputPath", outputPath );
    }
  }
}


void Wizard::infopage_readCaptionSettings()
{
  KConfig config ( "kipirc" );
  KConfigGroup group = config.group ( QString ( "PrintAssistant" ) );

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
}

void Wizard::readSettings ( QString pageName )
{
  KConfig config ( "kipirc" );
  KConfigGroup group = config.group ( QString ( "PrintAssistant" ) );

  if ( pageName == i18n ( infoPageName ) )
  {
    // InfoPage
    QString printerName = group.readEntry ( "Printer", i18n("Print to PDF") );
    int index = d->mInfoPage->m_printer_choice->findText(printerName);
    if(index !=-1)
    {
      d->mInfoPage->m_printer_choice->setCurrentIndex(index);
    }
    // init QPrinter
    outputChanged(d->mInfoPage->m_printer_choice->currentText());

    //initPhotoSizes ( d->m_printer.paperSize(QPrinter::Millimeter) );
    
    //caption
    infopage_readCaptionSettings();
    
    bool same_to_all = group.readEntry ( "SameCaptionToAll", 0 ) == 1;
    d->mInfoPage->m_sameCaption->setChecked(same_to_all);
    //enable right caption stuff
    captionChanged ( d->mInfoPage->m_captions->currentText() );

  }
  else if ( pageName == i18n ( photoPageName ) )
  {
    QSize iconSize = group.readEntry  ( "IconSize", QSize(24,24));
    d->mPhotoPage->ListPhotoSizes->setIconSize(iconSize);

    // photo size
    d->m_savedPhotoSize = group.readEntry ( "PhotoSize" );
  }
  else if ( pageName == i18n ( cropPageName ) )
  {
    // CropPage
    if (d->mInfoPage->m_printer_choice->currentText() == i18n("Print to JPG"))
    {
      // set the last output path
      KUrl outputPath; // force to get current directory as default
      outputPath = group.readPathEntry ( "OutputPath", outputPath.url() );
      d->mCropPage->m_outputPath->setUrl(outputPath);
      d->mCropPage->m_outputPath->setVisible(true);
      d->mCropPage->m_outputPath->setEnabled(true);
      KFile::Modes mode = KFile::Directory |
                    KFile::ExistingOnly;
      d->mCropPage->m_outputPath->setMode(mode);
    }
    else
    {
      d->mCropPage->m_outputPath->setVisible(false);
    }
  }   
}

void Wizard::printPhotos ( QList<TPhoto*> photos, QList<QRect*> layouts, QPrinter &printer )
{
  d->m_cancelPrinting = false;
  QProgressDialog pbar(this);
  pbar.setRange ( 0, photos.count() );
  KApplication::kApplication()->processEvents();

  QPainter p;
  p.begin ( &printer );

  int current = 0;

  bool printing = true;
  while ( printing )
  {
    printing = paintOnePage ( p, photos, layouts, current, d->mCropPage->m_disableCrop->isChecked() );
    if ( printing )
      printer.newPage();
    
    pbar.setValue(current );

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
      kDebug() << "Error running kjobviewr\n";
  LblPrintProgress->setText ( i18n ( "Complete. Click Finish to exit the Print Wizard." ) );
#endif
}

QStringList Wizard::printPhotosToFile ( QList<TPhoto*> photos, QString &baseFilename, TPhotoSize* layouts )
{
  Q_ASSERT ( layouts->layouts.count() > 1 );

  d->m_cancelPrinting = false;
  QProgressDialog pbar(this);
  pbar.setRange ( 0, photos.count() );
  
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
    QPixmap pixmap ( w, h );
    QPainter painter;
    painter.begin ( &pixmap );

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
        break;
      }
    }

    printing = paintOnePage ( painter, photos, layouts->layouts, current, d->mCropPage->m_disableCrop->isChecked() );

    painter.end();

    if ( saveFile )
    {
      files.append ( filename );
      if (!pixmap.save ( filename ))
      {
        KMessageBox::sorry ( this, 
                              i18n ( "Could not save file, please check your output entry." ) );
        break;
      }
    }
    pageCount++;

    pbar.setValue(current );

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
        kDebug() << "Error launching kjobviewr\n";
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
  kDebug() << page->name();
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

  if( d->mInfoPage->m_printer_choice->currentText() != i18n("Print to JPG") &&
      d->mInfoPage->m_printer_choice->currentText() != i18n("Print to gimp") )
  {
    // tell him again!
    d->m_printer->setFullPage ( true );

    qreal left, top, right, bottom;
    d->m_printer->getPageMargins (&left, &top, &right, &bottom, QPrinter::Millimeter );
      kDebug() << "Dialog exit, new margins: left " << left
      << " right " << right << " top " << top << " bottom " << bottom;
    std::auto_ptr<QPrintDialog> dialog ( KdePrint::createPrintDialog ( d->m_printer, this ) );
    dialog->setWindowTitle ( i18n ( "Print Image" ) );
    
    bool wantToPrint = dialog->exec() == QDialog::Accepted;
    if ( !wantToPrint )
    {
      KAssistantDialog::accept();
      return;
    }
    kDebug() << "paper page " << dialog->printer()->paperSize() ;
    
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    printPhotos ( d->m_photos, s->layouts, *d->m_printer );
    QApplication::restoreOverrideCursor();

  }
  else if ( d->mInfoPage->m_printer_choice->currentText() == i18n("Print to gimp") )
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
  else if ( d->mInfoPage->m_printer_choice->currentText() == i18n("Print to JPG"))
  {
    // now output the items
    //TODO manage URL
    QString path = d->mCropPage->m_outputPath->url().path() ;
    if (path.isEmpty())
    {
      KMessageBox::sorry ( this, 
                              i18n ( "Empty output path." ) );
      return;
    }
    if ( path.right ( 1 ) != "/" )
      path = path + "/";
    path = path + "kipi_printassistant_";
    kDebug() << path;
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    printPhotosToFile ( d->m_photos, path, s );
    QApplication::restoreOverrideCursor();
  }

  saveSettings ( currentPage ()->name() );

  KAssistantDialog::accept();
}

  void Wizard::pagesetupdialogexit()
{
  QPrinter *printer = d->m_pDlg->printer();

  kDebug() << "Dialog exit, new size " << printer->paperSize(QPrinter::Millimeter)
  << " internal size " << d->m_printer->paperSize(QPrinter::Millimeter);
  qreal left, top, right, bottom;
  d->m_printer->getPageMargins (&left, &top, &right, &bottom, QPrinter::Millimeter );
  kDebug() << "Dialog exit, new margins: left " << left
  << " right " << right << " top " << top << " bottom " << bottom;
  // next should be useless invoke once changing wizard page
  //initPhotoSizes ( d->m_printer.paperSize(QPrinter::Millimeter));

//     d->m_pageSize = d->m_printer.paperSize(QPrinter::Millimeter);
#ifdef NOT_YET
  kDebug() << " dialog exited num of copies: " << printer->numCopies ()
  << " inside:   " << d->m_printer->numCopies ();

  kDebug() << " dialog exited from : " << printer->fromPage ()
  << " to:   " << d->m_printer->toPage();
#endif
}

void Wizard::pagesetupclicked()
{
  if (d->m_pDlg)
    delete d->m_pDlg;
  d->m_pDlg = new QPageSetupDialog (d->m_printer, this);
  // TODO next line should work but it doesn't because of a QT bug
  //d->m_pDlg->open(this, SLOT(pagesetupdialogexit()));
  int ret = d->m_pDlg->exec();
  if ( ret == QDialog::Accepted )
  {
    pagesetupdialogexit();
  }   
}

void Wizard::infopage_blockCaptionButtons(bool block)
{
  d->mInfoPage->m_captions->blockSignals(block);
  d->mInfoPage->m_free_label->blockSignals(block);
  d->mInfoPage->m_sameCaption->blockSignals(block);
  d->mInfoPage->m_font_name->blockSignals(block);
  d->mInfoPage->m_font_size->blockSignals(block);
  d->mInfoPage->m_font_color->blockSignals(block);   
}

void Wizard::saveCaptionSettings()
{
  // Save the current settings
  KConfig config ( "kipirc" );
  KConfigGroup group = config.group ( QString ( "PrintAssistant" ) );
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
  // same to all
  group.writeEntry( "SameCaptionToAll", (d->mInfoPage->m_sameCaption->isChecked() ? 1 : 0));
}
  
} // namespace KIPIPrintImagesPlugin
