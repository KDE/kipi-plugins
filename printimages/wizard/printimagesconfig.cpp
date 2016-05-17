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

#include "printimagesconfig.h"

// Qt includes

#include <qglobal.h>
#include <QFile>

namespace KIPIPrintImagesPlugin
{

class PrintImagesConfigHelper
{
public:

    PrintImagesConfigHelper()
        : q(0)
    {
    }

    ~PrintImagesConfigHelper()
    {
        delete q;
    }

    PrintImagesConfig* q;
};


Q_GLOBAL_STATIC(PrintImagesConfigHelper, s_globalPrintImagesConfig)

PrintImagesConfig* PrintImagesConfig::self()
{
    if (!s_globalPrintImagesConfig()->q)
    {
        new PrintImagesConfig;
        s_globalPrintImagesConfig()->q->read();
    }

    return s_globalPrintImagesConfig()->q;
}

PrintImagesConfig::PrintImagesConfig()
    : KConfigSkeleton( QLatin1String( "kipiprintimagesrc" ) )
{
    Q_ASSERT(!s_globalPrintImagesConfig()->q);

    s_globalPrintImagesConfig()->q = this;
    setCurrentGroup( QLatin1String( "Print" ) );

    KConfigSkeleton::ItemInt* itemPrintPosition = new KConfigSkeleton::ItemInt( currentGroup(), QLatin1String( "PrintPosition" ), mPrintPosition, Qt::AlignHCenter | Qt::AlignVCenter);
    addItem( itemPrintPosition, QLatin1String( "PrintPosition" ) );

    QList<KConfigSkeleton::ItemEnum::Choice> valuesPrintScaleMode;
    {
        KConfigSkeleton::ItemEnum::Choice choice;
        choice.name = QLatin1String("PrintOptionsPage::NoScale");
        valuesPrintScaleMode.append( choice );
    }

    {
        KConfigSkeleton::ItemEnum::Choice choice;
        choice.name = QLatin1String("PrintOptionsPage::ScaleToPage");
        valuesPrintScaleMode.append( choice );
    }

    {
        KConfigSkeleton::ItemEnum::Choice choice;
        choice.name = QLatin1String("PrintOptionsPage::ScaleToCustomSize");
        valuesPrintScaleMode.append( choice );
    }

    KConfigSkeleton::ItemEnum* itemPrintScaleMode = new KConfigSkeleton::ItemEnum( currentGroup(), QLatin1String( "PrintScaleMode" ), mPrintScaleMode, valuesPrintScaleMode, KIPIPrintImagesPlugin::PrintOptionsPage::ScaleToPage );
    addItem( itemPrintScaleMode, QLatin1String( "PrintScaleMode" ) );

    KConfigSkeleton::ItemBool* itemPrintEnlargeSmallerImages = new KConfigSkeleton::ItemBool( currentGroup(), QLatin1String( "PrintEnlargeSmallerImages" ), mPrintEnlargeSmallerImages, false );
    addItem( itemPrintEnlargeSmallerImages, QLatin1String( "PrintEnlargeSmallerImages" ) );

    KConfigSkeleton::ItemDouble* itemPrintWidth = new KConfigSkeleton::ItemDouble( currentGroup(), QLatin1String( "PrintWidth" ), mPrintWidth, 15.0 );
    addItem( itemPrintWidth, QLatin1String( "PrintWidth" ) );

    KConfigSkeleton::ItemDouble* itemPrintHeight = new KConfigSkeleton::ItemDouble( currentGroup(), QLatin1String( "PrintHeight" ), mPrintHeight, 10.0 );
    addItem( itemPrintHeight, QLatin1String( "PrintHeight" ) );

    QList<KConfigSkeleton::ItemEnum::Choice> valuesPrintUnit;
    {
        KConfigSkeleton::ItemEnum::Choice choice;
        choice.name = QLatin1String("PrintOptionsPage::Millimeters");
        valuesPrintUnit.append( choice );
    }

    {
        KConfigSkeleton::ItemEnum::Choice choice;
        choice.name = QLatin1String("PrintOptionsPage::Centimeters");
        valuesPrintUnit.append( choice );
    }

    {
        KConfigSkeleton::ItemEnum::Choice choice;
        choice.name = QLatin1String("PrintOptionsPage::Inches");
        valuesPrintUnit.append( choice );
    }

    KConfigSkeleton::ItemEnum* itemPrintUnit = new KConfigSkeleton::ItemEnum( currentGroup(), QLatin1String( "PrintUnit" ), mPrintUnit, valuesPrintUnit, KIPIPrintImagesPlugin::PrintOptionsPage::Centimeters );
    addItem( itemPrintUnit, QLatin1String( "PrintUnit" ) );

    KConfigSkeleton::ItemBool* itemPrintKeepRatio = new KConfigSkeleton::ItemBool( currentGroup(), QLatin1String( "PrintKeepRatio" ), mPrintKeepRatio, true );
    addItem( itemPrintKeepRatio, QLatin1String( "PrintKeepRatio" ) );

    KConfigSkeleton::ItemBool*itemPrintAutoRotate = new KConfigSkeleton::ItemBool( currentGroup(), QLatin1String( "PrintAutoRotate" ), mPrintAutoRotate, false );
    addItem( itemPrintAutoRotate, QLatin1String( "PrintAutoRotate" ) );
}

PrintImagesConfig::~PrintImagesConfig()
{
    s_globalPrintImagesConfig()->q = 0;
}

} // namespace KIPIPrintImagesPlugin
