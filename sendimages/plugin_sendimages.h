/* ============================================================
 * File  : plugin_sendimages.h
 * Author: Gilles Caulier <caulier dot gilles at free.fr>
 * Date  : 2003-11-04
 * Description : Digikam Send Mail Images Plugin.
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

#ifndef PLUGIN_SENDIMAGES_H
#define PLUGIN_SENDIMAGES_H

// Digikam includes

#include <digikam/plugin.h>

class KAction;
class SendImagesDialog;

class Plugin_SendImages : public Digikam::Plugin
{
Q_OBJECT

public:
  Plugin_SendImages(QObject *parent, const char* name, const QStringList &args);
  virtual ~Plugin_SendImages();

public slots:
  void slotActivate();

private:
  KAction            *m_action_sendimages;
  SendImagesDialog   *m_sendImagesDialog;
};


#endif /* PLUGIN_SENDIMAGES_H */
