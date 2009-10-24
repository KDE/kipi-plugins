/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-02-08
 * Description : a kipi plugin to print images
 *
 * Copyright 2009 by Angelo Naselli <anaselli at linux dot it>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "printhelper.h"

// C++ includes

#include <memory>

// Qt includes

#include <QCheckBox>
#include <QPainter>
#include <QPrinter>
#include <QPrintDialog>
#include <QList>

// KDE includes

#include <klocale.h>
#include <kdeprintdialog.h>
#include <kdebug.h>

// Local includes

#include "tphoto.h"
#include "printoptionspage.h"
#include "atkinspagelayout.h"

namespace KIPIPrintImagesPlugin
{

class PrintHelperDialog : public QPrintDialog
{
private:

    PrintOptionsPage *pOptionsPage;

public:

    PrintHelperDialog(QPrinter *printer, PrintOptionsPage *optionsPage, QWidget *parent = 0);
    ~PrintHelperDialog() {};

public Q_SLOTS:

    void manageQPrintDialogChanges ( QPrinter * printer );
};

// -----------------------------------------------------------------

PrintHelperDialog::PrintHelperDialog(QPrinter *printer, PrintOptionsPage *optionsPage, QWidget *parent)
                 : QPrintDialog(printer, parent), pOptionsPage(optionsPage)
{
    connect(this, SIGNAL ( accepted ( QPrinter*) ),/* this,*/
            pOptionsPage, SLOT ( manageQPrintDialogChanges ( QPrinter * ) ) );
}

void PrintHelperDialog::manageQPrintDialogChanges ( QPrinter * /*printer*/ )
{
    kDebug() << "It has been called!";
}

class PrintHelperPrivate
{
public:

    QWidget*       mParent;
    QList<TPhoto*> m_photos;

    QSize adjustSize ( TPhoto &doc, int printerResolution, const QSize & viewportSize )
    {
        // can't be null here!
        Q_ASSERT (doc.pAddInfo != NULL);
        /*
        int mUnit;
        int mPrintPosition;
        int mScaleMode;
        bool mKeepRatio;
        bool mAutoRotate;
        double mPrintWidth, mPrintHeight;
        bool mEnlargeSmallerImages;
        */
        QSize size = doc.size();

        PrintOptionsPage::ScaleMode scaleMode = PrintOptionsPage::ScaleMode(doc.pAddInfo->mScaleMode);
        if ( scaleMode == PrintOptionsPage::ScaleToPage )
        {
            bool imageBiggerThanPaper = size.width() > viewportSize.width() ||
                                        size.height() > viewportSize.height();

            if ( imageBiggerThanPaper || doc.pAddInfo->mEnlargeSmallerImages )
            {
                size.scale ( viewportSize, Qt::KeepAspectRatio );
            }

        }
        else if ( scaleMode == PrintOptionsPage::ScaleToCustomSize )
        {
            PrintOptionsPage::Unit unit = PrintOptionsPage::Unit(doc.pAddInfo->mUnit);
            double wImg                 = doc.scaleWidth(PrintOptionsPage::unitToInches(unit));
            double hImg                 = doc.scaleHeight(PrintOptionsPage::unitToInches(unit));
            size.setWidth ( int ( wImg * printerResolution ) );
            size.setHeight ( int ( hImg * printerResolution ) );
        }
        else
        {
            // No scale
            const double INCHES_PER_METER = 100. / 2.54;
            QImage img                    = doc.loadPhoto();
            int dpmX                      = img.dotsPerMeterX();
            int dpmY                      = img.dotsPerMeterY();

            if ( dpmX > 0 && dpmY > 0 )
            {
                double wImg = double ( size.width() ) / double ( dpmX ) * INCHES_PER_METER;
                double hImg = double ( size.height() ) / double ( dpmY ) * INCHES_PER_METER;
                size.setWidth ( int ( wImg * printerResolution ) );
                size.setHeight ( int ( hImg * printerResolution ) );
            }
        }
        return size;
    }

    QPoint adjustPosition ( TPhoto &doc, const QSize& imageSize, const QSize & viewportSize )
    {
        // can't be null here!
        Q_ASSERT (doc.pAddInfo != NULL);
        Qt::Alignment alignment = Qt::Alignment(doc.pAddInfo->mPrintPosition);
        int posX, posY;

        if ( alignment & Qt::AlignLeft )
        {
            posX = 0;
        }
        else if ( alignment & Qt::AlignHCenter )
        {
            posX = ( viewportSize.width() - imageSize.width() ) / 2;
        }
        else
        {
            posX = viewportSize.width() - imageSize.width();
        }

        if ( alignment & Qt::AlignTop )
        {
            posY = 0;
        }
        else if ( alignment & Qt::AlignVCenter )
        {
            posY = ( viewportSize.height() - imageSize.height() ) / 2;
        }
        else
        {
            posY = viewportSize.height() - imageSize.height();
        }

        return QPoint ( posX, posY );
    }
};

// ----------------------------------------------------------------------------

PrintHelper::PrintHelper ( QWidget* parent , KIPI::Interface*)
           : d ( new PrintHelperPrivate )
{
    d->mParent = parent;
}

PrintHelper::~PrintHelper()
{
    delete d;
}

void PrintHelper::print ( KUrl::List fileList )
{
    QPrinter printer;

    for (int i=0; i < d->m_photos.count(); i++)
        if (d->m_photos.at(i))
            delete d->m_photos.at(i);

    d->m_photos.clear();

    for (int i=0; i < fileList.count(); i++)
    {
        TPhoto *photo   = new TPhoto(150);
        photo->filename = fileList[i];
        photo->pAddInfo = new AdditionalInfo();
        d->m_photos.append(photo);
    }

    PrintOptionsPage* optionsPage = new PrintOptionsPage(d->mParent, &d->m_photos);
    optionsPage->loadConfig();

    std::auto_ptr<PrintHelperDialog> dialog ( new PrintHelperDialog(&printer,
                                              optionsPage,
                                              d->mParent) );

    dialog->setOptionTabs(QList<QWidget*>() << optionsPage);

#if 0
    connect(dialog, SIGNAL ( accepted ( QPrinter*) ),
            optionsPage, SLOT ( ogChanges ( QPrinter * ) ) );

    std::auto_ptr<QPrintDialog> dialog (
        KdePrint::createPrintDialog ( &printer,
                                      QList<QWidget*>() << optionsPage,
                                      d->mParent )
    );
#endif

    dialog->setWindowTitle ( i18n ( "Kipi-plugins image printing" ) );
    bool wantToPrint = dialog->exec();

    //optionsPage->saveConfig();
    if ( !wantToPrint )
    {
        return;
    }

    QPainter painter ( &printer );
    QRect rect = painter.viewport();
    /*
        //---size.scale ( rect.size(), Qt::KeepAspectRatio );---
        QSize size = d->adjustSize ( optionsPage, doc, printer.resolution(), rect.size() );
        QPoint pos = d->adjustPosition ( optionsPage, size, rect.size() );
        d->adjustImage(optionsPage, doc);

        painter.setViewport ( pos.x(), pos.y(), size.width(), size.height() );
    */

    for (int i=0; i < fileList.count(); )
    {
        if (optionsPage->printUsingAtkinsLayout())
        {
          int pages = optionsPage->photoXPage();
          int j     = 0;
          // create AtkinsPageLayout
          AtkinsPageLayout layout(rect);
          // add all items
          for (j=0; i+j<fileList.count() && j<pages; j++)
          {
              layout.addLayoutItem(j, d->m_photos.at(i+j)->size());
          }
          // retrieve rectangles for all items
          for (j=0; i+j<fileList.count() && j<pages; j++)
          {
              QImage image = d->m_photos.at(i+j)->loadPhoto();
              painter.drawImage (layout.itemRect(j) , image );
          }
          i+=pages;
          if (i < fileList.count())
              printer.newPage();
        }
        else
        {
          TPhoto *pPhoto = d->m_photos.at(i);
          QImage image   = pPhoto->loadPhoto();
          kDebug() << "Img size " << image.size() << " viewportSize " << rect.size();

          // if horPages is > 0 vertPages is as well
          bool multipagePrinting = optionsPage->mp_horPages() > 0; 

          if (multipagePrinting)
          {
            int horPages = optionsPage->mp_horPages();
            int vertPages = optionsPage->mp_verPages();

            QRect imageRec = image.rect();
            int x1; int y1; int x2; int y2;
            imageRec.getCoords (&x1, &y1, &x2, &y2);
            kDebug() << "Img coords (" << x1 << ", " << y1 << ", " << x2 << ", " << y2 <<")";
            QRect destRec = QRect(QPoint(0,0), QPoint(x2/horPages,y2/vertPages));

            for (int px=1; px <= horPages; px++)
            {
              for (int py=1; py <= vertPages; py++)
              {
                int sx = ((px-1)*x2/horPages);
                int sy = ((py-1)*y2/vertPages);
                int ex = (px*x2/horPages);
                int ey = (py*y2/vertPages);
                kDebug() << "Img part coords (" << sx << ", " << sy << ", " << ex << ", " << ey <<")";
                QImage destImage = image.copy(QRect(QPoint(sx, sy), QPoint(ex, ey)));
                QSize destSize = destImage.size();
                destSize.scale(rect.size(), Qt::KeepAspectRatio);
                painter.setViewport(rect.x(), rect.y(), destSize.width(), destSize.height());
//                 painter.setViewport (destRec);
                painter.setWindow (destRec);
                painter.drawImage (0, 0, destImage);
//                 painter.drawImage ( /*destRec*/ QPoint(0,0),
//                                     image,
//                                      QRect(QPoint(sx, sy), QPoint(ex, ey)));

                if (!(px == horPages && py == vertPages))
                  printer.newPage();
              }
            }
          }
          else
          {
            // trying to fix size at the moment
            QSize size = d->adjustSize ( *pPhoto, printer.resolution(), rect.size() );
            QPoint pos = d->adjustPosition ( *pPhoto, size, rect.size() );

            if (pPhoto->pAddInfo->mAutoRotate)
            {
  //               printer.setOrientation( d->m_photos.at(i)->size().width() <= d->m_photos.at(i)->size().height() ? QPrinter::Portrait
                  printer.setOrientation( image.width() <= image.height() ? QPrinter::Portrait
                      : QPrinter::Landscape );
            }
            painter.setViewport ( pos.x(), pos.y(), size.width(), size.height() );

            painter.setWindow ( image.rect() );
            painter.drawImage ( 0, 0, image );
          }
          if ((++i) < fileList.count())
              printer.newPage();
        }
    }
}

} // namespace
