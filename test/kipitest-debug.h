/* ============================================================
 *
 * Date        : 2010-02-16
 * Description : Special handler for debug output for kipi-test
 *
 * Copyright (C) 2010 by Michael G. Hansen <mike at mghansen dot de>
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

#ifndef KIPITEST_DEBUG_H
#define KIPITEST_DEBUG_H

// Qt includes

#include <QTextStream>

// KDE includes

#include <kdebug.h>

extern QTextStream qerr;

extern enum KipiTestDebugTarget {
    KipiTestDebugNone = 0,
    KipiTestDebugStdErr = 1,
    KipiTestDebugKDebug = 2
} kipiTestDebugTarget;

#define kipiDebug(debugtext) if (kipiTestDebugTarget == KipiTestDebugNone) { } \
                    else if (kipiTestDebugTarget == KipiTestDebugStdErr) { qerr << debugtext << "\n"; } \
                    else { kDebug() << debugtext; }

#endif /* KIPITEST_DEBUG_H */

