/* ===========================================================
 * File   : plugin_diroperation.h
 *
 * Authors: Gregory KOKANOSKY <gregory dot kokanosky at free.fr>
 *          Gilles Caulier <caulier dot gilles at free.fr>
 *
 * Date   : 01/2004
 *
 * Description : Open Album in Konqueror/Nautilus
 *
 * Copyright 2004 by Gregory KOKANOSKY and Gilles CAULIER
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

  #ifndef PLUGIN_DIROPERATION_H
  #define PLUGIN_DIROPERATION_H

  // Include files for KDE

  #include <kprocess.h>

  // KIPI includes

  #include <libkipi/plugin.h>

  class KActionMenu;
  class KAction;


  class Plugin_DirOperations : public KIPI::Plugin
  {
  Q_OBJECT

   public:

   Plugin_DirOperations(QObject *parent,
                      const char* name,
                      const QStringList &args);
   virtual KIPI::Category category() const;
   virtual void setup( QWidget* widget );

   private slots:

   void slotOpenInKonqui();
   void slotOpenInNautilus();

   void slotAlbumSelected( bool b );


   private:
   KActionMenu    *m_action_OpenIn;
   KProcess       *m_browserProc;
  };

  #endif // PLUGIN_DIROPERATION_H
