/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2009-11-13
 * Description : a plugin to blend bracketed images.
 *
 * Copyright (C) 2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef RAW_PAGE_H
#define RAW_PAGE_H

// Local includes

#include "wizardpage.h"

// KDE includes

#include <kurl.h>

// Libkdcraw includes

#include <libkdcraw/rawdecodingsettings.h>

using namespace KIPIPlugins;
using namespace KDcrawIface;

namespace KIPIExpoBlendingPlugin
{

class RawPagePriv;

class RawPage : public WizardPage
{

  public:

    RawPage(KAssistantDialog* dlg);
    ~RawPage();

    RawDecodingSettings rawDecodingSettings() const;

private:

    RawPagePriv* const d;
};

}   // namespace KIPIExpoBlendingPlugin

#endif /* RAW_PAGE_H */
