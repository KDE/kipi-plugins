//////////////////////////////////////////////////////////////////////////////
//
//    PLUGIN_FINDIMAGES.H
//
//    Copyright (C) 2004 Gilles Caulier <caulier dot gilles at free.fr>
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
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef PLUGIN_FINDIMAGES_H
#define PLUGIN_FINDIMAGES_H

#include <libkipi/plugin.h>

class QCustomEvent;
class QProgressDialog;

class KActionMenu;
class KAction;

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
   KActionMenu          *m_action_findImages;
   KAction              *m_action_findDuplicateImagesAlbums;

protected:
   void customEvent(QCustomEvent *event);

   FindDuplicateImages *findDuplicateOperation;
   QProgressDialog *m_progressDlg;
   int m_current, m_total;

private slots:
   void slotFindDuplicateImages();
   void slotCancel();

};

#endif   // PLUGIN_FINDIMAGES_H
