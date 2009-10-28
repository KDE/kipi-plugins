/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-06-08
 * Description : settings object for the correction process
 *
 * Copyright (C) 2008-2009 by Andi Clemens <andi dot clemens at gmx dot net>
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

#ifndef HAARSETTINGS_H
#define HAARSETTINGS_H

// Qt includes

#include <QString>

// KDE includes

#include <kstandarddirs.h>
#include <kdebug.h>

namespace KIPIRemoveRedEyesPlugin
{

const QString STANDARD_CLASSIFIER = KGlobal::dirs()->findResource( "data",
                                    "kipiplugin_removeredeyes/removeredeyes_classifier_eye_20_20.xml");

class HaarSettings
    {
    public:

        HaarSettings()
        {

            useStandardClassifier = true;
            useSimpleMode         = true;
            minRoundness          = 0.0;
            scaleFactor           = 0.0;
            minBlobsize           = 0;
            neighborGroups        = 0;
            simpleMode            = 0;
        };

        void debug()
        {
            kDebug() << "***************";
            kDebug() << "HaarSettings";
            kDebug() << "***************";
            kDebug() << "useStandardClassifier = " << useStandardClassifier;
            kDebug() << "minRoundness          = " << minRoundness;
            kDebug() << "scaleFactor           = " << scaleFactor;
            kDebug() << "minBlobsize           = " << minBlobsize;
            kDebug() << "neighborGroups        = " << neighborGroups;
            kDebug() << "simpleMode            = " << simpleMode;
            kDebug() << "classifierFile        = " << classifierFile;
        };

        bool    useStandardClassifier;
        bool    useSimpleMode;
        bool    addKeyword;

        double  minRoundness;
        double  scaleFactor;

        int     minBlobsize;
        int     neighborGroups;
        int     storageMode;
        int     unprocessedMode;
        int     simpleMode;

        QString classifierFile;
        QString extraName;
        QString keywordName;
    };

} // namespace KIPIRemoveRedEyesPlugin

#endif // HAARSETTINGS_H
