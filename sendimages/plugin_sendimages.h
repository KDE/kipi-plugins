//////////////////////////////////////////////////////////////////////////////
//
//    PLUGIN_SENDIMAGES.H
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

#ifndef PLUGIN_SENDIMAGES_H
#define PLUGIN_SENDIMAGES_H

// KIPI includes

#include <libkipi/plugin.h>

class QCustomEvent;
class QProgressDialog;

class KAction;
class KIPISendimagesPlugin::SendImages;

class Plugin_SendImages : public KIPI::Plugin
{
Q_OBJECT

public:
   Plugin_SendImages(QObject *parent, const char* name, const QStringList &args);
   ~Plugin_SendImages();
   
   virtual KIPI::Category category( KAction* action ) const;
   virtual void setup( QWidget* );
  
public slots:
   void slotActivate();
   void slotCancel();
   
private:
   int                               m_current;
   int                               m_total;
   QProgressDialog                  *m_progressDlg;
   KAction                          *m_action_sendimages;
   KIPISendimagesPlugin::SendImages *m_sendImagesOperation;

   void customEvent(QCustomEvent *event);

};


#endif // PLUGIN_SENDIMAGES_H 
