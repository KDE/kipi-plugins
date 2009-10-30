/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2002-12-09
 * Description : a kipi plugin to print images
 *
 * Copyright 2002-2003 by Todd Shoemaker <todd@theshoemakers.net>
 * Copyright 2007-2008 by Angelo Naselli <anaselli at linux dot it>
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

#ifndef TPHOTO_H
#define TPHOTO_H

// Qt includes
#include <QRect>
#include <QFont>
#include <QColor>

// KDE includes
#include <kurl.h>

namespace KExiv2Iface
{
class KExiv2;
}

namespace KIPIPrintImagesPlugin
{

  class AdditionalInfo
  {
    public:
      int mUnit;
      int mPrintPosition;
      int mScaleMode;
      bool mKeepRatio;
      bool mAutoRotate;
      double mPrintWidth, mPrintHeight;
      bool mEnlargeSmallerImages;
      AdditionalInfo() : mUnit(0), mPrintPosition(0), mScaleMode(0), mKeepRatio(true),
                         mAutoRotate(true), mPrintWidth(0.0), mPrintHeight(0.0),
                         mEnlargeSmallerImages(false)
                         {}
                         
      AdditionalInfo(const AdditionalInfo& ai)
      {
        mUnit          = ai.mUnit;
        mPrintPosition = ai.mPrintPosition;
        mScaleMode     = ai.mScaleMode;
        mKeepRatio     = ai.mKeepRatio;
        mAutoRotate    = ai.mAutoRotate;
        mPrintWidth    = ai.mPrintWidth;
        mPrintHeight   = ai.mPrintHeight;
        mEnlargeSmallerImages = ai.mEnlargeSmallerImages;
      }
  };

  class CaptionInfo
  {
    public:
      enum AvailableCaptions {
        NoCaptions = 0,
        FileNames,
        ExifDateTime,
        Comment,
        Free
      };
      
      AvailableCaptions m_caption_type;
      QFont             m_caption_font;
      QColor            m_caption_color;
      int               m_caption_size;
      QString           m_caption_text;

      CaptionInfo() : m_caption_type(NoCaptions), m_caption_font("Sans Serif"), m_caption_color(Qt::yellow),
                      m_caption_size(2),m_caption_text()
      {}
                         
      CaptionInfo(const CaptionInfo& ci)
      {
        m_caption_type   = ci.m_caption_type;
        m_caption_font   = ci.m_caption_font;
        m_caption_color  = ci.m_caption_color;
        m_caption_size   = ci.m_caption_size;
        m_caption_text   = ci.m_caption_text;
      }
      
      virtual ~CaptionInfo() {}
  };

  class TPhoto
  {

  public:

      TPhoto ( int thumbnailSize );
      TPhoto (const TPhoto& );
      ~TPhoto();

      KUrl filename; // full path

      QPixmap & thumbnail();
      QImage    loadPhoto();

      int m_thumbnailSize;

      int width();
      int height();
      QSize& size();

      QRect cropRegion;
      // to get first copy quickly
      bool first;
      // number of copies
      int copies;
      int rotation;
      AdditionalInfo *pAddInfo;
      CaptionInfo    *pCaptionInfo;

      double scaleWidth(double unitToInches);
      double scaleHeight(double unitToInches);

      KExiv2Iface::KExiv2 *exiv2Iface();

  private:

      void   loadCache();

  private:

      QPixmap             *m_thumbnail;
      QSize               *m_size;
      KExiv2Iface::KExiv2 *m_exiv2Iface;
  };

}  // NameSpace KIPIPrintImagesPlugin

#endif // TPHOTO_H
