//////////////////////////////////////////////////////////////////////////////
//
//    PLUGIN_FINDIMAGES.H
//
//    Copyright (C) 2004 Gilles Caulier <caulier dot gilles at gmail dot com>
//    Copyright (C) 2004 Richard Groult <rgroult at jalix.org>
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
//    Foundation, Inc., 51 Franklin Steet, Fifth Floor, Cambridge, MA 02110-1301, USA.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef PLUGIN_FINDIMAGES_H
#define PLUGIN_FINDIMAGES_H

// LibKipi includes.

#include <libkipi/plugin.h>

class QCustomEvent;

class KActionMenu;
class KAction;

namespace KIPI
{
class BatchProgressDialog;
}

class FindDuplicateImages;

class Plugin_FindImages : public KIPI::Plugin
{
Q_OBJECT

public:

   Plugin_FindImages(QObject *parent, const char* name, const QStringList &args);
   ~Plugin_FindImages();

   virtual KIPI::Category category( KAction* action ) const;
   virtual void setup( QWidget* widget );

private:

   KAction                                             *m_action_findDuplicateImages;

protected:

   KIPIFindDupplicateImagesPlugin::FindDuplicateImages *m_findDuplicateOperation;

   KIPI::BatchProgressDialog                           *m_progressDlg;

   int                                                  m_total;

   void customEvent(QCustomEvent *event);

private slots:

   void slotFindDuplicateImages();
   void slotCancel();
};

#endif   // PLUGIN_FINDIMAGES_H
