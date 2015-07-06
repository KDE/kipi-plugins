/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-05-28
 * Description : a KIPI plugin to export pics through DLNA technology.
 *
 * Copyright (C) 2012 by Smit Mehta <smit dot meh at gmail dot com>
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

#ifndef WIZARD_H
#define WIZARD_H

// Local includes

#include "kpimageslist.h"
#include "kptooldialog.h"

using namespace KIPIPlugins;

namespace KIPIDLNAExportPlugin
{

class Wizard : public KPWizardDialog
{
    Q_OBJECT

public:

    explicit Wizard(QWidget* const parent);
    ~Wizard();

    void next();

private Q_SLOTS:

    void updateCollectionSelectorPageValidity();
    void getImagesFromCollection();
    void getDirectoriesFromCollection();
    void changeBackButtonState(bool state);
    void deleteDlnaObjects();

private:

    class Private;
    Private* const d;
};

} // namespace KIPIDLNAExportPlugin

#endif /* WIZARD_H */
