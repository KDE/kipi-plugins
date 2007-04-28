//////////////////////////////////////////////////////////////////////////////
//
//    PLUGIN_CDARCHIVING.H
//
//    Copyright (C) 2003-2004 Gilles Caulier <caulier dot gilles at gmail dot com>
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
//    Foundation, Inc., 51 Franklin Street, Fifth Floor, Cambridge, MA 02110-1301, USA.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef PLUGIN_CDARCHIVING_H
#define PLUGIN_CDARCHIVING_H

// LibKIPI includes.

#include <libkipi/plugin.h>

class QCustomEvent;

class KAction;

namespace KIPI
{
class BatchProgressDialog;
}

namespace KIPICDArchivingPlugin
{
class CDArchiving;
class BatchProgressDialog;
}

class Plugin_CDArchiving : public KIPI::Plugin
{
   Q_OBJECT

public:
   Plugin_CDArchiving(QObject *parent, const char* name, const QStringList &args);
   virtual ~Plugin_CDArchiving();
   virtual KIPI::Category category( KAction* action ) const;
   virtual void setup( QWidget* widget );

public slots:
   void slotActivate();
   void slotCancel();

private:
   int                                         m_current;
   int                                         m_total;
   
   KIPI::BatchProgressDialog                  *m_progressDlg;

   KIPICDArchivingPlugin::CDArchiving         *m_cdarchiving;
   KAction                                    *m_action_cdarchiving;
   
   void customEvent(QCustomEvent *event);
};


#endif /* PLUGIN_CDARCHIVING_H */
