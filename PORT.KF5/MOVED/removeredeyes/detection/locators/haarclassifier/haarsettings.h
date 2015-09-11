/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-06-08
 * Description : settings object for the correction process
 *
 * Copyright (C) 2008-2009 by Andi Clemens <andi dot clemens at googlemail dot com>
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
#include <QStandardPaths>

// Local includes

#include "kipiplugins_debug.h"

namespace KIPIRemoveRedEyesPlugin
{

const QString STANDARD_CLASSIFIER = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                           "kipiplugin_removeredeyes/removeredeyes_classifier_eye_20_20.xml");

class HaarSettings
{
public:

    HaarSettings() :
        useStandardClassifier(true),
        useSimpleMode(true),
        addKeyword(true),
        minRoundness(0.0),
        scaleFactor(0.0),
        minBlobsize(0),
        neighborGroups(0),
        storageMode(0),
        unprocessedMode(0),
        simpleMode(0)
    {
    };

    void debug()
    {
        qCDebug(KIPIPLUGINS_LOG) << "***************";
        qCDebug(KIPIPLUGINS_LOG) << "HaarSettings";
        qCDebug(KIPIPLUGINS_LOG) << "***************";
        qCDebug(KIPIPLUGINS_LOG) << "useStandardClassifier = " << useStandardClassifier;
        qCDebug(KIPIPLUGINS_LOG) << "minRoundness          = " << minRoundness;
        qCDebug(KIPIPLUGINS_LOG) << "scaleFactor           = " << scaleFactor;
        qCDebug(KIPIPLUGINS_LOG) << "minBlobsize           = " << minBlobsize;
        qCDebug(KIPIPLUGINS_LOG) << "neighborGroups        = " << neighborGroups;
        qCDebug(KIPIPLUGINS_LOG) << "simpleMode            = " << simpleMode;
        qCDebug(KIPIPLUGINS_LOG) << "classifierFile        = " << classifierFile;
    };

public:

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
