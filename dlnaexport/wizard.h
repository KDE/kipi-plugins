/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-01-11
 * Description : a kipi plugin to print images
 *
 * Copyright 2008-2012 by Angelo Naselli <anaselli at linux dot it>
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

// QT incudes

// Local includes

#include "kpimageslist.h"
#include "kptooldialog.h"

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIDLNAExportPlugin
{

class Wizard : public KPWizardDialog
{
    Q_OBJECT

public:

    Wizard(QWidget* const);
    ~Wizard();
    void next();
	void back();

private Q_SLOTS:
    
    void updateCollectionSelectorPageValidity();
	void getImagesFromCollection();
    
private:

    struct Private;
    Private* const d;
};

} // namespace KIPIDLNAExportPlugin

#endif /* WIZARD_H */
