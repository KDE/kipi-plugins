//////////////////////////////////////////////////////////////////////////////
//
//    PLUGIN_IMAGESGALLERY.H
//
//    Copyright (C) 2003-2004 Gilles Caulier <caulier dot gilles at free.fr>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef PLUGIN_IMAGESGALLERY_H
#define PLUGIN_IMAGESGALLERY_H

// LibKIPI includes.

#include <libkipi/plugin.h>

class KAction;

namespace KIPI
{
class BatchProgressDialog;
}

namespace KIPIImagesGalleryPlugin
{
class KIGPDialog;
class ResizeImage;
}


class Plugin_Imagesgallery : public KIPI::Plugin
{
   Q_OBJECT

public:
   Plugin_Imagesgallery(QObject *parent, const char* name, const QStringList &args);
   virtual ~Plugin_Imagesgallery();
   
   KIPI::Category category( KAction* action ) const;
   virtual void setup( QWidget* widget );

public slots:
   void slotActivate();
   void slotCancel();

private:
   int                                           m_current;
   int                                           m_total;

   KIPIImagesGalleryPlugin::ImagesGallery       *m_gallery;
   
   KIPI::BatchProgressDialog                    *m_progressDlg;
   KAction                                      *m_actionImagesGallery;
   
   void customEvent(QCustomEvent *event);
};


#endif // PLUGIN_IMAGESGALLERY_H
