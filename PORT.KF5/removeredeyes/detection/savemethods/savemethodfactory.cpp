/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-01-18
 * Description : factory to create save method objects
 *
 * Copyright (C) 2009 by Andi Clemens <andi dot clemens at googlemail dot com>
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

#include "savemethodfactory.h"

// Local includes

#include "storagesettingsbox.h"

namespace KIPIRemoveRedEyesPlugin
{

SaveMethod* SaveMethodFactory::create(int type)
{
    SaveMethod* saveMethod = 0;

    switch (type)
    {
        case StorageSettingsBox::Subfolder:
            saveMethod = new SaveSubfolder();
            break;
        case StorageSettingsBox::Prefix:
            saveMethod = new SavePrefix();
            break;
        case StorageSettingsBox::Suffix:
            saveMethod = new SaveSuffix();
            break;
        case StorageSettingsBox::Overwrite:
            saveMethod = new SaveOverwrite();
            break;
    }

    return saveMethod;
}

} // namespace KIPIRemoveRedEyesPlugin
