/* ============================================================
 * File  : resizeimage.cpp
 * Author: Gilles Caulier <caulier dot gilles at free.fr>
 * Date  : 2004-02-26
 * Description : Multithreaded image resize process
 *
 * Copyright 2004 by Gilles Caulier

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */


// Include files for Qt

#include <qapplication.h>
#include <qobject.h>
#include <qimage.h>

// Include files for KDE

#include <kdebug.h>
#include <kstandarddirs.h>
#include <kglobalsettings.h>

// Local includes

#include "resizeimage.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////

ResizeImage::ResizeImage(QObject *parent,
                         const QString Path, const QString Directory,
                         const QString ImageFormat, const QString ImageNameFormat, int *Width,
                         int *Height, int SizeFactor, bool ColorDepthChange, int ColorDepthValue,
                         bool CompressionSet, int ImageCompression, bool *threadDone, bool *useBrokenImage)
            : QThread(), parent_(parent)

{
    Path_             = Path;
    Directory_        = Directory;
    ImageFormat_      = ImageFormat;
    ImageNameFormat_  = ImageNameFormat;
    Width_            = Width;
    Height_           = Height;
    SizeFactor_       = SizeFactor;
    ColorDepthChange_ = ColorDepthChange;
    ColorDepthValue_  = ColorDepthValue;
    CompressionSet_   = CompressionSet;
    ImageCompression_ = ImageCompression;
    threadDone_       = threadDone;
    useBrokenImage_   = useBrokenImage;
    QThread::start();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

ResizeImage::~ResizeImage()
{
    wait();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void ResizeImage::run(void)
{
    QImage img;
    bool ValRet;
    *useBrokenImage_ = false;

    ValRet = img.load(Path_);

    if ( ValRet == false )        // cannot load the src image.
       {
       KGlobal::dirs()->addResourceType("digikam_imagebroken", KGlobal::dirs()->kde_default("data")
                                        + "digikam/data");
       QString dir = KGlobal::dirs()->findResourceDir("digikam_imagebroken", "image_broken.png");
       dir = dir + "image_broken.png";
       qDebug("Loading %s failed ! Using %s instead...", Path_.ascii(), dir.ascii() );
       ValRet = img.load(dir);   // load broken image icon...
       *useBrokenImage_ = true;
       }

    if ( ValRet == true )
       {
       int w = img.width();
       int h = img.height();

       if (SizeFactor_ == -1)      // Use original image size.
            SizeFactor_= w;

       // scale to pixie size
       // kdDebug( 51000 ) << "w: " << w << " h: " << h << endl;
       // Resizing if to big

       if( w > SizeFactor_ || h > SizeFactor_ )
           {
           if( w > h )
               {
               h = (int)( (double)( h * SizeFactor_ ) / w );

               if ( h == 0 ) h = 1;

               w = SizeFactor_;
               Q_ASSERT( h <= SizeFactor_ );
               }
           else
               {
               w = (int)( (double)( w * SizeFactor_ ) / h );

               if ( w == 0 ) w = 1;

               h = SizeFactor_;
               Q_ASSERT( w <= SizeFactor_ );
               }

           const QImage scaleImg(img.smoothScale( w, h ));

           if ( scaleImg.width() != w || scaleImg.height() != h )
               {
               kdDebug( 51000 ) << "Resizing failed. Aborting." << endl;
               *threadDone_ = false;
               return;
               }

           img = scaleImg;

           if ( ColorDepthChange_ == true )
               {
               const QImage depthImg(img.convertDepth( ColorDepthValue_ ));
               img = depthImg;
               }
           }

       kdDebug( 51000 ) << "Saving resized image to: " << Directory_ + ImageFormat_  << endl;

       if ( CompressionSet_ == true )
          {
          if ( !img.save(Directory_ + ImageNameFormat_, ImageFormat_.latin1(), ImageCompression_) )
             {
             qDebug("Saving failed with specific compression value. Aborting.");
             *threadDone_ = false;
             return;
             }
          }
       else
          {
          if ( !img.save(Directory_ + ImageNameFormat_, ImageFormat_.latin1(), -1) )
             {
             qDebug("Saving failed with no compression value. Aborting.");
             *threadDone_ = false;
             return;
             }
          }

       *Width_      = w;
       *Height_     = h;
       *threadDone_ = true;
       return;
       }

    *threadDone_ = false;
    return;
}

