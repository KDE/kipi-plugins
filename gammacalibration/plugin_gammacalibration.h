/* ============================================================
 * File   : plugin_gammacalibration.h
 *
 * Authors: Gilles Caulier <caulier dot gilles at free.fr>
 *
 * Date   : 01/2004
 *
 * Description : KDE Gamma correction call.
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
 * ============================================================ */

  #ifndef PLUGIN_GAMMACALIBRATION_H
  #define PLUGIN_GAMMACALIBRATION_H

  // KIPI includes

  #include <libkipi/plugin.h>

  class KAction;


  class Plugin_GammaCalibration : public KIPI::Plugin
  {
  Q_OBJECT

   public:

   Plugin_GammaCalibration(QObject *parent,
                           const char* name,
                           const QStringList &args);
   virtual KIPI::Category category( KAction* action ) const;
   virtual void setup( QWidget* widget );

   private slots:

   void slotActivate(void);
   
   private:
   
   KAction *m_action_gammaCalibration;
  };

  #endif // PLUGIN_GAMMACALIBRATION_H
