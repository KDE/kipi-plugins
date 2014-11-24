/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-04-04
 * Description : A KIPI plugin to generate HTML image galleries
 *
 * Copyright (C) 2006-2010 by Aurelien Gateau <aurelien dot gateau at free.fr>
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

// vim: set tabstop=4 shiftwidth=4 noexpandtab:

#ifndef WIZARD_H
#define WIZARD_H

// Local includes

#include "kptooldialog.h"

namespace KIPI
{
    class Interface;
}

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIHTMLExport
{

class GalleryInfo;

/**
 * The wizard used by the user to select the various settings.
 */
class Wizard : public KPWizardDialog
{
    Q_OBJECT

public:

    Wizard(QWidget* const parent, GalleryInfo* constinfo);
    ~Wizard();

protected Q_SLOTS:

    virtual void accept();

private Q_SLOTS:

    void updateCollectionSelectorPageValidity();
    void updateFinishPageValidity();
    void slotThemeSelectionChanged();

private:

    struct Private;
    Private* d;
};

} // namespace KIPIHTMLExport

#endif /* WIZARD_H */
