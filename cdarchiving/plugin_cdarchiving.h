/* ============================================================
 * File  : plugin_cdarchiving.h
 * Author: Gilles Caulier <caulier dot gilles at free.fr>
 * Date  : 2003-09-05
 * Description : Albums CD archiving Digikam plugin
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

#ifndef PLUGIN_CDARCHIVING_H
#define PLUGIN_CDARCHIVING_H

#include <libkipi/plugin.h>
#include <kaction.h>

class CDArchiving;

class Plugin_CDArchiving : public KIPI::Plugin
{
  Q_OBJECT

public:
  Plugin_CDArchiving(QObject *parent, const char* name, const QStringList &args);
  virtual ~Plugin_CDArchiving();
  virtual KIPI::Category category() const;

  KAction *m_action_cdarchiving;

public slots:

  void slotActivate();

private:

    CDArchiving *m_cdarchiving;
};


#endif /* PLUGIN_CDARCHIVING_H */
