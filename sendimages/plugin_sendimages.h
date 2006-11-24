/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at free.fr>
 * Date   : 2003-10-01
 * Description : a kipi plugin for e-mailing images
 * 
 * Copyright 2003-2005 by Gilles Caulier
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

#ifndef PLUGIN_SENDIMAGES_H
#define PLUGIN_SENDIMAGES_H

// KIPI includes

#include <libkipi/plugin.h>

class QCustomEvent;

class KAction;
class KIPISendimagesPlugin::SendImages;

namespace KIPI
{
    class BatchProgressDialog;
}

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
   void slotAcceptedConfigDlg();

private:

   void customEvent(QCustomEvent *event);
   
private:

   int                               m_current;
   int                               m_total;

   KAction                          *m_action_sendimages;
   
   KIPI::BatchProgressDialog        *m_progressDlg;
   
   KIPISendimagesPlugin::SendImages *m_sendImagesOperation;
};

#endif // PLUGIN_SENDIMAGES_H 
