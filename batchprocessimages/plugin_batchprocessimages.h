//////////////////////////////////////////////////////////////////////////////
//
//    PLUGIN_BATCHPROCESSIMAGES.H
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

#ifndef PLUGIN_BATCHPROCESSIMAGES_H
#define PLUGIN_BATCHPROCESSIMAGES_H

// Include files for Qt

#include <qstring.h>

// Include files for KDE

#include <libkipi/plugin.h>

class KAction;

class BorderImagesDialog;
class ColorImagesDialog;
class ConvertImagesDialog;
class EffectImagesDialog;
class FilterImagesDialog;
class RenameImagesDialog;
class RecompressImagesDialog;
class ResizeImagesDialog;

class Plugin_BatchProcessImages : public KIPI::Plugin
{
Q_OBJECT

public:
  Plugin_BatchProcessImages(QObject *parent, const char* name, const QStringList &args);
  virtual ~Plugin_BatchProcessImages();
  virtual KIPI::Category category() const;
  virtual void setup( QWidget* );

public slots:
  void slotActivate();

private:
  KAction                *m_action_borderimages;
  KAction                *m_action_colorimages;
  KAction                *m_action_convertimages;
  KAction                *m_action_effectimages;
  KAction                *m_action_filterimages;
  KAction                *m_action_renameimages;
  KAction                *m_action_recompressimages;
  KAction                *m_action_resizeimages;


  BorderImagesDialog     *m_BorderImagesDialog;
  ColorImagesDialog      *m_ColorImagesDialog;
  ConvertImagesDialog    *m_ConvertImagesDialog;
  EffectImagesDialog     *m_EffectImagesDialog;
  FilterImagesDialog     *m_FilterImagesDialog;
  RenameImagesDialog     *m_RenameImagesDialog;
  RecompressImagesDialog *m_RecompressImagesDialog;
  ResizeImagesDialog     *m_ResizeImagesDialog;
};


#endif /* PLUGIN_BATCHPROCESSIMAGES_H */
