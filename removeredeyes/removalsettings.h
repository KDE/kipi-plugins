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

#ifndef REMOVALSETTINGS_H
#define REMOVALSETTINGS_H

// Qt includes.

#include <QString>

// KDE includes.

#include <kstandarddirs.h>
#include <kdebug.h>

namespace KIPIRemoveRedEyesPlugin
{

const QString STANDARD_CLASSIFIER = KGlobal::dirs()->findResource( "data",
                                    "kipiplugin_removeredeyes/removeredeyes_classifier_eye_20_20.xml");

class RemovalSettings
    {
    public:

        RemovalSettings()
        {

            useStandardClassifier   = true;
            useSimpleMode           = true;
            addKeyword              = false;
            minRoundness            = 0.0;
            scaleFactor             = 0.0;

            minBlobsize             = 0;
            neighborGroups          = 0;
            storageMode             = 0;
            unprocessedMode         = 0;
            simpleMode              = 0;
        };

        void debug()
        {
            kDebug(51000) << "***************" << endl;
            kDebug(51000) << "RemovalSettings" << endl;
            kDebug(51000) << "***************" << endl;
            kDebug(51000) << "useStandardClassifier = " << useStandardClassifier << endl;
            kDebug(51000) << "minRoundness          = " << minRoundness << endl;
            kDebug(51000) << "scaleFactor           = " << scaleFactor << endl;
            kDebug(51000) << "minBlobsize           = " << minBlobsize << endl;
            kDebug(51000) << "neighborGroups        = " << neighborGroups << endl;
            kDebug(51000) << "storageMode           = " << storageMode << endl;
            kDebug(51000) << "unprocessedMode       = " << unprocessedMode << endl;
            kDebug(51000) << "simpleMode            = " << simpleMode << endl;
            kDebug(51000) << "classifierFile        = " << classifierFile << endl;
            kDebug(51000) << "subfolderName         = " << subfolderName << endl;
            kDebug(51000) << "suffixName            = " << suffixName << endl;
            kDebug(51000) << "keywordName           = " << keywordName << endl;
        };

        bool        useStandardClassifier;
        bool        useSimpleMode;
        bool        addKeyword;

        double      minRoundness;
        double      scaleFactor;

        int         minBlobsize;
        int         neighborGroups;
        int         storageMode;
        int         unprocessedMode;
        int         simpleMode;

        QString     classifierFile;
        QString     subfolderName;
        QString     suffixName;
        QString     keywordName;
    };

} // namespace KIPIRemoveRedEyesPlugin

#endif // REMOVALSETTINGS_H
