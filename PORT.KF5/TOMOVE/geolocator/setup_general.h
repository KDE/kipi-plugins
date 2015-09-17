/** ===========================================================
 * @file
 *
 * This file is a part of kipi-plugins project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2010-08-27
 * @brief  Setup widget for geo correlator.
 *
 * @author Copyright (C) 2010 by Michael G. Hansen
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

#ifndef SETUP_GENERAL_H
#define SETUP_GENERAL_H

// local includes

#include "setup.h"

namespace KIPIGeolocatorPlugin
{

class SetupGeneral : public SetupTemplate
{
    Q_OBJECT

public:

    explicit SetupGeneral(QWidget* const parent = 0);
    ~SetupGeneral();

public Q_SLOTS:

    void slotApplySettings();

private:

    void readSettings();

private:

    class Private;
    Private* const d;
};

} /* namespace KIPIGeolocatorPlugin */

#endif /* SETUP_GENERAL_H */
