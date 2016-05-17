/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-02-08
 * Description : a kipi plugin to print images
 *
 * Copyright 2009-2012 by Angelo Naselli <anaselli at linux dot it>
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

#ifndef _PRINT_IMAGES_CONFIG_H_
#define _PRINT_IMAGES_CONFIG_H_

// Qt includes

#include <QCoreApplication>
#include <QDebug>

// KDE includes

#include <kconfigskeleton.h>

// Local includes

#include "printoptionspage.h"

namespace KIPIPrintImagesPlugin
{

class PrintImagesConfig : public KConfigSkeleton
{
    public:

        static PrintImagesConfig* self();
        ~PrintImagesConfig();

        /**
         * Set PrintPosition
         */
        static void setPrintPosition( int v )
        {
            if (!self()->isImmutable( QString::fromLatin1( "PrintPosition" ) ))
                self()->mPrintPosition = v;
        }

        /**
         * Get PrintPosition
         */
        static int printPosition()
        {
            return self()->mPrintPosition;
        }

        /**
         * Set PrintScaleMode
         */
        static void setPrintScaleMode( PrintOptionsPage::ScaleMode v )
        {
            if (!self()->isImmutable( QString::fromLatin1( "PrintScaleMode" ) ))
                self()->mPrintScaleMode = v;
        }

        /**
         * Get PrintScaleMode
         */
        static PrintOptionsPage::ScaleMode printScaleMode()
        {
            return static_cast<PrintOptionsPage::ScaleMode>(self()->mPrintScaleMode);
        }

        /**
         * Set PrintEnlargeSmallerImages
         */
        static void setPrintEnlargeSmallerImages( bool v )
        {
            if (!self()->isImmutable( QString::fromLatin1( "PrintEnlargeSmallerImages" ) ))
                self()->mPrintEnlargeSmallerImages = v;
        }

        /**
         * Get PrintEnlargeSmallerImages
         */
        static bool printEnlargeSmallerImages()
        {
            return self()->mPrintEnlargeSmallerImages;
        }

        /**
         * Set PrintWidth
         */
        static void setPrintWidth( double v )
        {
            if (!self()->isImmutable( QString::fromLatin1( "PrintWidth" ) ))
                self()->mPrintWidth = v;
        }

        /**
         * Get PrintWidth
         */
        static double printWidth()
        {
            return self()->mPrintWidth;
        }

        /**
         * Set PrintHeight
         */
        static void setPrintHeight( double v )
        {
            if (!self()->isImmutable( QString::fromLatin1( "PrintHeight" ) ))
                self()->mPrintHeight = v;
        }

        /**
         * Get PrintHeight
         */
        static double printHeight()
        {
            return self()->mPrintHeight;
        }

        /**
         * Set PrintUnit
         */
        static void setPrintUnit( PrintOptionsPage::Unit v )
        {
            if (!self()->isImmutable( QString::fromLatin1( "PrintUnit" ) ))
                self()->mPrintUnit = v;
        }

        /**
         * Get PrintUnit
         */
        static PrintOptionsPage::Unit printUnit()
        {
            return static_cast<PrintOptionsPage::Unit>(self()->mPrintUnit);
        }

        /**
         * Set PrintKeepRatio
        */
        static void setPrintKeepRatio( bool v )
        {
            if (!self()->isImmutable( QString::fromLatin1( "PrintKeepRatio" ) ))
                self()->mPrintKeepRatio = v;
        }

        /**
         * Get PrintKeepRatio
         */
        static bool printKeepRatio()
        {
            return self()->mPrintKeepRatio;
        }

        /**
         * Set PrintAutoRotate
         */
        static void setPrintAutoRotate( bool v )
        {
            if (!self()->isImmutable( QString::fromLatin1( "PrintAutoRotate" ) ))
                self()->mPrintAutoRotate = v;
        }

        /**
         * Get PrintAutoRotate
         */
        static bool printAutoRotate()
        {
            return self()->mPrintAutoRotate;
        }

    protected:

        PrintImagesConfig();
        friend class PrintImagesConfigHelper;

    protected:

        int     mPrintPosition;
        int     mPrintScaleMode;
        bool    mPrintEnlargeSmallerImages;
        double  mPrintWidth;
        double  mPrintHeight;
        int     mPrintUnit;
        bool    mPrintKeepRatio;
        bool    mPrintAutoRotate;
};

} // namespace KIPIPrintImagesPlugin

#endif // _PRINT_IMAGES_CONFIG_H_
