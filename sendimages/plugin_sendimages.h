/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-10-01
 * Description : a kipi plugin to e-mailing images
 *
 * Copyright (C) 2003-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef PLUGIN_SENDIMAGES_H
#define PLUGIN_SENDIMAGES_H

// KIPI includes

#include <libkipi/plugin.h>

class QCustomEvent;

class KAction;

class Plugin_SendImagesPriv;

namespace KIPI
{
    class BatchProgressDialog;
}

namespace KIPISendimagesPlugin
{
    class SendImages;
}

class Plugin_SendImages : public KIPI::Plugin
{

Q_OBJECT

public:

   Plugin_SendImages(QObject *parent, const QVariantList &args);
   ~Plugin_SendImages();
   
   virtual KIPI::Category category( KAction* action ) const;
   virtual void setup( QWidget* );
  
public slots:

   void slotActivate();
   void slotCancel();

private:

   void customEvent(QEvent *event);
   
private:

    Plugin_SendImagesPriv *d;
};

#endif // PLUGIN_SENDIMAGES_H 
