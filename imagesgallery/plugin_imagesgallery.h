/* ============================================================
 * File  : plugin_imagesgallery.h
 * Author: Gilles Caulier <caulier dot gilles at free.fr>
 * Date  : 2003-09-05
 * Description : Images gallery HTML export Digikam plugin
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

#ifndef PLUGIN_IMAGESGALLERY_H
#define PLUGIN_IMAGESGALLERY_H

#include <libkipi/plugin.h>


class Plugin_Imagesgallery : public KIPI::Plugin
{
Q_OBJECT

public:
  Plugin_Imagesgallery(QObject *parent, const char* name, const QStringList &args);
  virtual ~Plugin_Imagesgallery();
  KIPI::Category category() const;
  virtual void setup( QWidget* widget );

public slots:
  void slotActivate();
};


#endif // PLUGIN_IMAGESGALLERY_H
