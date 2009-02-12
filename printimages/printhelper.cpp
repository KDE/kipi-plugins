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

// C++ includes.

#include <memory>

// Qt includes.

#include <QCheckBox>
#include <QPainter>
#include <QPrinter>
#include <QPrintDialog>
#include <QList>
// KDE includes.

#include <klocale.h>
#include <kdeprintdialog.h>
#include <kdebug.h>

// Local includes.
#include "tphoto.h"
// #include "printoptionspage.h"
// #include "icctransform.h"

namespace KIPIPrintImagesPlugin
{

  struct PrintHelperPrivate
  {
    QWidget* mParent;
    QList<TPhoto*>     m_photos;
#if 0
    QSize adjustSize ( PrintOptionsPage* optionsPage, DImg &doc, int printerResolution, const QSize & viewportSize )
    {
      QSize size = doc.size();
      PrintOptionsPage::ScaleMode scaleMode = optionsPage->scaleMode();
      if ( scaleMode == PrintOptionsPage::ScaleToPage )
      {
        bool imageBiggerThanPaper =
            size.width() > viewportSize.width()
            || size.height() > viewportSize.height();

        if ( imageBiggerThanPaper || optionsPage->enlargeSmallerImages() )
        {
          size.scale ( viewportSize, Qt::KeepAspectRatio );
        }

      }
      else if ( scaleMode == PrintOptionsPage::ScaleToCustomSize )
      {
        double wImg = optionsPage->scaleWidth();
        double hImg = optionsPage->scaleHeight();
        size.setWidth ( int ( wImg * printerResolution ) );
        size.setHeight ( int ( hImg * printerResolution ) );

      }
      else
      {
        // No scale
        const double INCHES_PER_METER = 100. / 2.54;
        QImage img = doc.copyQImage();
        int dpmX = img.dotsPerMeterX();
        int dpmY = img.dotsPerMeterY();
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


    QPoint adjustPosition ( PrintOptionsPage* optionsPage, const QSize& imageSize, const QSize & viewportSize )
    {
      Qt::Alignment alignment = optionsPage->alignment();
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

    void adjustImage ( PrintOptionsPage* optionsPage,  DImg& img)
    {
      if (optionsPage->colorManaged())
      {
        IccTransform *transform = new IccTransform();

        if (img.getICCProfil().isNull())
        {
          transform->setProfiles( optionsPage->inProfilePath(), optionsPage->outputProfilePath() );
        }
        else
        {
          transform->setProfiles(optionsPage->outputProfilePath());
        }

        transform->apply( img );
      }
    }
#endif //0
  };


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
    //doc.loadFullImage();
    //doc.waitUntilLoaded();
    QPrinter printer;

    for(int i=0; i < d->m_photos.count(); i++)
      if (d->m_photos.at(i))
        delete d->m_photos.at(i);
    d->m_photos.clear();

    for(int i=0; i < fileList.count(); i++)
    {
      TPhoto *photo = new TPhoto(150);
      photo->filename = fileList[i];
      d->m_photos.append(photo);
    }
    
    
    
//     PrintOptionsPage* optionsPage = new PrintOptionsPage(d->mParent, doc.size());
//     optionsPage->loadConfig();
    std::auto_ptr<QPrintDialog> dialog ( new QPrintDialog(&printer,  d->mParent) );
//     std::auto_ptr<QPrintDialog> dialog (
//         KdePrint::createPrintDialog ( &printer,
// //                                       QList<QWidget*>() << optionsPage,
//                                       d->mParent )
//     );
    dialog->setWindowTitle ( i18n ( "Kipi-plugins image printing" ) );
    bool wantToPrint = dialog->exec();

//     optionsPage->saveConfig();
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
    
    for(int i=0; i < fileList.count(); i++)
    {
      //TODO
      QImage image = d->m_photos.at(i)->loadPhoto(); 
      kDebug() << "Img size " << image.size() << " viewportSize " << rect.size();
      // trying to fix size at the moment
      // QImage::scaled bug? i can't see what manual says for Qt::KeepAspectRatio,
      // calculating new viewportSize
#ifdef not_def
      int iW=image.size().width();
      int iH=image.size().height();
      int vW=rect.size().width();
      int vH=rect.size().height();
      if (vH < vW)
      {
        int w=iW*vH/iH;
        image = image.scaled(w,vH, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        kDebug() << " w x h " << w <<"x" << vH;
        painter.setViewport ( 0, 0, w, vH );
      }
      else
      {
        int h=iH*vW/iW;
        image = image.scaled(vW,h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        kDebug() << " w x h " << vW <<"x" << h;
        painter.setViewport ( 0, 0, vW, h );
      }
#endif //not_def
      
#if 0
      
      QPainter painter(&printer);
      QRect rect = painter.viewport();
      QSize size = imageLabel->pixmap()->size();
      size.scale(rect.size(), Qt::KeepAspectRatio);
      painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
      painter.setWindow(imageLabel->pixmap()->rect());
      painter.drawPixmap(0, 0, *imageLabel->pixmap());
      
      if (image.size().height() >  image.size().width())
      {
        image = image.scaledToHeight(rect.size().height(), Qt::SmoothTransformation);
      }
      else
      {
        image = image.scaledToWidth(rect.size().width(), Qt::SmoothTransformation);
      }
#endif

      QSize size = image.size();
      size.scale(rect.size(), Qt::KeepAspectRatio);
      painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
      painter.setWindow ( image.rect() );
      painter.drawImage ( 0, 0, image );
      if ((i+1) < fileList.count())
        printer.newPage();
    }
  }

} // namespace
