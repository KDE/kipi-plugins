/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-04-19
 * Description : time adjust settings container.
 *
 * Copyright (C) 2012-2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef TIMEADJUSTSETTING_H
#define TIMEADJUSTSETTING_H

namespace KIPITimeAdjustPlugin
{

/** Container that store all timestamp adjustments.
 */
class TimeAdjustSettings
{

public:

    enum UseDateSource
    {
        APPDATE = 0,
        FILEDATE,
        METADATADATE,
        CUSTOMDATE
    };

    enum UseMetaDateType
    {
        EXIFIPTCXMP = 0,
        EXIFCREATED,
        EXIFORIGINAL,
        EXIFDIGITIZED,
        IPTCCREATED,
        XMPCREATED
    };

    enum UseFileDateType
    {
        FILELASTMOD = 0,
        FILECREATED
    };

    enum AdjType
    {
        COPYVALUE = 0,
        ADDVALUE,
        SUBVALUE
    };

public:

    TimeAdjustSettings()
    {
        updAppDate     = false;
        updEXIFModDate = false;
        updEXIFOriDate = false;
        updEXIFDigDate = false;
        updEXIFThmDate = false;
        updIPTCDate    = false;
        updXMPDate     = false;
        updFileName    = false;
        updFileModDate = false;

        dateSource     = APPDATE;
        metadataSource = EXIFIPTCXMP;
        fileDateSource = FILELASTMOD;
    };

    ~TimeAdjustSettings()
    {
    };

    /// Check if at least one option is selected
    bool atLeastOneUpdateToProcess() const
    {
        return (updAppDate     ||
                updFileModDate ||
                updEXIFModDate ||
                updEXIFOriDate ||
                updEXIFDigDate ||
                updEXIFThmDate ||
                updIPTCDate    ||
                updXMPDate     ||
                updFileName);
    }

public:

    bool updAppDate;
    bool updEXIFModDate;
    bool updEXIFOriDate;
    bool updEXIFDigDate;
    bool updEXIFThmDate;
    bool updIPTCDate;
    bool updXMPDate;
    bool updFileName;
    bool updFileModDate;

    int  dateSource;
    int  metadataSource;
    int  fileDateSource;
};

// -------------------------------------------------------------------

/** Container that hold the time difference for clock photo dialog.
 */
class DeltaTime
{

public:

    DeltaTime()
    {
        deltaNegative = false;
        deltaDays     = 0;
        deltaHours    = 0;
        deltaMinutes  = 0;
        deltaSeconds  = 0;
    };

    ~DeltaTime()
    {
    };

    /// Check if at least one option is selected
    bool isNull() const
    {
        return (deltaDays    == 0 &&
                deltaHours   == 0 &&
                deltaMinutes == 0 &&
                deltaSeconds == 0);
    }

public:

    bool deltaNegative;

    int  deltaDays;
    int  deltaHours;
    int  deltaMinutes;
    int  deltaSeconds;
};

}  // namespace KIPITimeAdjustPlugin

#endif  // TIMEADJUSTSETTING_H
