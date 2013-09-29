/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2004-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef PLUGIN_BATCHPROCESSIMAGES_H
#define PLUGIN_BATCHPROCESSIMAGES_H

// Qt includes

#include <QString>
#include <QVariant>

// KDE includes

#include <libkipi/plugin.h>

class KAction;

namespace KIPI
{
    class Interface;
}

namespace KIPIBatchProcessImagesPlugin
{

class BorderImagesDialog;
class ColorImagesDialog;
class ConvertImagesDialog;
class EffectImagesDialog;
class FilterImagesDialog;
class RecompressImagesDialog;
class RenameImagesDialog;
class ResizeImagesDialog;

class Plugin_BatchProcessImages : public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_BatchProcessImages(QObject* const parent, const QVariantList& args);
    virtual ~Plugin_BatchProcessImages();

    void setup(QWidget* const);

public Q_SLOTS:

    void slotActivate();

private:

    void setupActions();

private:

    KAction*                m_action_borderimages;
    KAction*                m_action_colorimages;
    KAction*                m_action_convertimages;
    KAction*                m_action_effectimages;
    KAction*                m_action_filterimages;
    KAction*                m_action_renameimages;
    KAction*                m_action_recompressimages;
    KAction*                m_action_resizeimages;

    BorderImagesDialog*     m_BorderImagesDialog;
    ColorImagesDialog*      m_ColorImagesDialog;
    ConvertImagesDialog*    m_ConvertImagesDialog;
    EffectImagesDialog*     m_EffectImagesDialog;
    FilterImagesDialog*     m_FilterImagesDialog;
    RecompressImagesDialog* m_RecompressImagesDialog;
    ResizeImagesDialog*     m_ResizeImagesDialog;

    KIPI::Interface*        m_iface;
};

}  // namespace KIPIBatchProcessImagesPlugin

#endif /* PLUGIN_BATCHPROCESSIMAGES_H */
