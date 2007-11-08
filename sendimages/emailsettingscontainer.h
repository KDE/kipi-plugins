/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2007-11-07
 * Description : e-mail settings container.
 *
 * Copyright (C) 2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef EMAILSETTINGSCONTAINER_H
#define EMAILSETTINGSCONTAINER_H

// Qt includes.

#include <QString>

// Local includes.

#include "emailpage.h"

namespace KIPISendimagesPlugin
{

class EmailSettingsContainer
{

public:
    
    EmailSettingsContainer()
    {
        addCommentsAndTags = false;
        imagesChangeProp   = false;
        attachmentLimit    = 17;
        imageCompression   = 75;
        emailProgram       = EmailPage::KMAIL;
        imageSize          = EmailPage::MEDIUM;
        imageFormat        = EmailPage::JPEG;
    };
    
    ~EmailSettingsContainer(){};

public:

    bool                   addCommentsAndTags;
    bool                   imagesChangeProp;

    int                    attachmentLimit;
    int                    imageCompression;

    QString                thunderbirdPath;

    EmailPage::EmailClient emailProgram;

    EmailPage::ImageSize   imageSize;

    EmailPage::ImageFormat imageFormat;
};

}  // namespace KIPISendimagesPlugin

#endif  // EMAILSETTINGSCONTAINER_H
