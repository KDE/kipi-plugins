/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2009-11-21
 * @brief  kipi host test application
 *
 * @author Copyright (C) 2009-2010 by Michael G. Hansen
 *         <a href="mailto:mike at mghansen dot de">mike at mghansen dot de</a>
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

#ifndef KIPITEST_DEBUG_H
#define KIPITEST_DEBUG_H

// Qt includes

#include <QTextStream>

// KDE includes

#include <kdebug.h>

extern QTextStream qerr;

extern enum KipiTestDebugTarget
{
    KipiTestDebugNone = 0,
    KipiTestDebugStdErr = 1,
    KipiTestDebugKDebug = 2
} kipiTestDebugTarget;

#define kipiDebug(debugtext)                                \
    if (kipiTestDebugTarget == KipiTestDebugNone)           \
    {                                                       \
    }                                                       \
    else if (kipiTestDebugTarget == KipiTestDebugStdErr)    \
    {                                                       \
        qerr << debugtext << "\n";                          \
    }                                                       \
    else                                                    \
    {                                                       \
        kDebug() << debugtext;                              \
    }                                                       \

#endif /* KIPITEST_DEBUG_H */
