/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2010-02-15
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2010 by Jens Mueller <tschenser at gmx dot de>
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

#ifndef PICASAWEBIMGLIST_H
#define PICASAWEBIMGLIST_H

#ifndef IMAGESLIST_H
#include "imageslist.h"
#endif


using namespace KIPI;

namespace KIPIPicasawebExportPlugin
{
    
class PicasawebImageDialogPrivate;    

class PicasawebImagesList : public KIPIPlugins::ImagesList
{
    Q_OBJECT

public:

    explicit PicasawebImagesList(Interface* iface, QWidget* parent = 0, int iconSize = -1);
    virtual ~PicasawebImagesList();

protected Q_SLOTS:

    virtual void slotAddItems();

};

class PicasawebImageDialog
{

public:

    PicasawebImageDialog(QWidget* parent, KIPI::Interface* iface);
    ~PicasawebImageDialog();

    KUrl::List urls() const;
    
private:

    PicasawebImageDialogPrivate* const d;
};

}  // namespace KIPIPicasawebExportPlugin


#endif // PICASAWEBIMGLIST_H
