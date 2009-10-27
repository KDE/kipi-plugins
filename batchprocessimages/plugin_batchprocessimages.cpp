/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2004-10-01
 * Description : a kipi plugin to batch process images
 *
 * Copyright (C) 2004-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "plugin_batchprocessimages.h"
#include "plugin_batchprocessimages.moc"

// C ANSI includes

extern "C"
{
#include <unistd.h>
}

// Qt includes

#include <QImage>
#include <QPointer>

// KDE includes

#include <kaction.h>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kcomponentdata.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klibloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <ktemporaryfile.h>

// KIPI includes

#include <libkipi/interface.h>

// Local includes

#include "borderimagesdialog.h"
#include "colorimagesdialog.h"
#include "convertimagesdialog.h"
#include "effectimagesdialog.h"
#include "filterimagesdialog.h"
#include "recompressimagesdialog.h"
#include "renameimagesdialog.h"
#include "resizeimagesdialog.h"


K_PLUGIN_FACTORY(BatchProcessImagesFactory, registerPlugin<Plugin_BatchProcessImages>();)
K_EXPORT_PLUGIN(BatchProcessImagesFactory("kipiplugin_batchprocessimages"))

Plugin_BatchProcessImages::Plugin_BatchProcessImages(QObject *parent, const QVariantList&)
                         : KIPI::Plugin(BatchProcessImagesFactory::componentData(),
                                        parent, "BatchProcessImages")
{
    kDebug(AREA_CODE_LOADING) << "Plugin_BatchProcessImages plugin loaded" ;
}

void Plugin_BatchProcessImages::setup(QWidget* widget)
{
    KIPI::Plugin::setup(widget);

    m_action_borderimages = actionCollection()->addAction("batch_border_images", this, SLOT(slotActivate()));
    m_action_borderimages->setIcon(KIcon("borderimages"));
    m_action_borderimages->setText(i18n("Border Images..."));

    m_action_colorimages = actionCollection()->addAction("batch_color_images", this, SLOT(slotActivate()));
    m_action_colorimages->setIcon(KIcon("colorimages"));
    m_action_colorimages->setText(i18n("Color Images..."));

    m_action_convertimages = actionCollection()->addAction("batch_convert_images", this, SLOT(slotActivate()));
    m_action_convertimages->setIcon(KIcon("convertimages"));
    m_action_convertimages->setText(i18n("Convert Images..."));

    m_action_effectimages = actionCollection()->addAction("batch_effect_images", this, SLOT(slotActivate()));
    m_action_effectimages->setIcon(KIcon("effectimages"));
    m_action_effectimages->setText(i18n("Image Effects..."));

    m_action_filterimages = actionCollection()->addAction("batch_filter_images", this, SLOT(slotActivate()));
    m_action_filterimages->setIcon(KIcon("filterimages"));
    m_action_filterimages->setText(i18n("Filter Images..."));

    m_action_renameimages = actionCollection()->addAction("batch_rename_images", this, SLOT(slotActivate()));
    m_action_renameimages->setIcon(KIcon("renameimages"));
    m_action_renameimages->setText(i18n("Rename Images..."));

    m_action_recompressimages = actionCollection()->addAction("batch_recompress_images", this, SLOT(slotActivate()));
    m_action_recompressimages->setIcon(KIcon("recompressimages"));
    m_action_recompressimages->setText(i18n("Recompress Images..."));

    m_action_resizeimages = actionCollection()->addAction("batch_resize_images", this, SLOT(slotActivate()));
    m_action_resizeimages->setIcon(KIcon("resizeimages"));
    m_action_resizeimages->setText(i18n("Resize Images..."));

    addAction(m_action_borderimages);
    addAction(m_action_colorimages);
    addAction(m_action_convertimages);
    addAction(m_action_effectimages);
    addAction(m_action_filterimages);
    addAction(m_action_renameimages);
    addAction(m_action_recompressimages);
    addAction(m_action_resizeimages);

    KIPI::Interface* interface = dynamic_cast< KIPI::Interface* >(parent());

    if (!interface)
    {
        kError() << "Kipi interface is null!" ;
        return;
    }

    KIPI::ImageCollection images = interface->currentAlbum();
    bool enable                  = images.isValid() && !images.images().isEmpty();

    m_action_borderimages->setEnabled(enable);
    m_action_colorimages->setEnabled(enable);
    m_action_convertimages->setEnabled(enable);
    m_action_effectimages->setEnabled(enable);
    m_action_filterimages->setEnabled(enable);
    m_action_renameimages->setEnabled(enable);
    m_action_recompressimages->setEnabled(enable);
    m_action_resizeimages->setEnabled(enable);

    connect(interface, SIGNAL(currentAlbumChanged(bool)),
            m_action_borderimages, SLOT(setEnabled(bool)));

    connect(interface, SIGNAL(currentAlbumChanged(bool)),
            m_action_colorimages, SLOT(setEnabled(bool)));

    connect(interface, SIGNAL(currentAlbumChanged(bool)),
            m_action_convertimages, SLOT(setEnabled(bool)));

    connect(interface, SIGNAL(currentAlbumChanged(bool)),
            m_action_effectimages, SLOT(setEnabled(bool)));

    connect(interface, SIGNAL(currentAlbumChanged(bool)),
            m_action_filterimages, SLOT(setEnabled(bool)));

    connect(interface, SIGNAL(currentAlbumChanged(bool)),
            m_action_renameimages, SLOT(setEnabled(bool)));

    connect(interface, SIGNAL(currentAlbumChanged(bool)),
            m_action_recompressimages, SLOT(setEnabled(bool)));

    connect(interface, SIGNAL(currentAlbumChanged(bool)),
            m_action_resizeimages, SLOT(setEnabled(bool)));
}

Plugin_BatchProcessImages::~Plugin_BatchProcessImages()
{
}

void Plugin_BatchProcessImages::slotActivate()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>(parent());

    if (!interface)
    {
        kError() << "Kipi interface is null!" ;
        return;
    }

    KIPI::ImageCollection images = interface->currentSelection();

    if (!images.isValid())
        return;

    if (images.images().isEmpty())
        images = interface->currentAlbum();

    if (!images.isValid())
        return;

    if (images.images().isEmpty())
    {
        KMessageBox::sorry(kapp->activeWindow(),
                           i18n("Please select an album or a selection of images."));
        return;
    }

    KUrl::List urlList = images.images();

    QString from(sender()->objectName());

    if (from == "batch_convert_images")
    {
        m_ConvertImagesDialog = new KIPIBatchProcessImagesPlugin::ConvertImagesDialog(urlList, interface,
                kapp->activeWindow());
        m_ConvertImagesDialog->show();
    }
    else if (from == "batch_rename_images")
    {
        QPointer<KIPIBatchProcessImagesPlugin::RenameImagesDialog> dlg;
        dlg = new KIPIBatchProcessImagesPlugin::RenameImagesDialog(urlList, interface, kapp->activeWindow());
        dlg->exec();
        delete dlg;
    }
    else if (from == "batch_border_images")
    {
        m_BorderImagesDialog = new KIPIBatchProcessImagesPlugin::BorderImagesDialog(urlList, interface,
                kapp->activeWindow());
        m_BorderImagesDialog->show();
    }
    else if (from == "batch_color_images")
    {
        m_ColorImagesDialog = new KIPIBatchProcessImagesPlugin::ColorImagesDialog(urlList, interface,
                kapp->activeWindow());
        m_ColorImagesDialog->show();
    }
    else if (from == "batch_filter_images")
    {
        m_FilterImagesDialog = new KIPIBatchProcessImagesPlugin::FilterImagesDialog(urlList, interface,
                kapp->activeWindow());
        m_FilterImagesDialog->show();
    }
    else if (from == "batch_effect_images")
    {
        m_EffectImagesDialog = new KIPIBatchProcessImagesPlugin::EffectImagesDialog(urlList, interface,
                kapp->activeWindow());
        m_EffectImagesDialog->show();
    }
    else if (from == "batch_recompress_images")
    {
        m_RecompressImagesDialog = new KIPIBatchProcessImagesPlugin::RecompressImagesDialog(urlList,
                interface, kapp->activeWindow());
        m_RecompressImagesDialog->show();
    }
    else if (from == "batch_resize_images")
    {
        m_ResizeImagesDialog = new KIPIBatchProcessImagesPlugin::ResizeImagesDialog(urlList, interface,
                kapp->activeWindow());
        m_ResizeImagesDialog->show();
    }
    else
    {
        kWarning() << "The impossible happened... unknown batch action specified";
        return;
    }
}

KIPI::Category Plugin_BatchProcessImages::category(KAction* action) const
{
    if (action == m_action_borderimages)
        return KIPI::BatchPlugin;
    else if (action == m_action_colorimages)
        return KIPI::BatchPlugin;
    else if (action == m_action_convertimages)
        return KIPI::BatchPlugin;
    else if (action == m_action_effectimages)
        return KIPI::BatchPlugin;
    else if (action == m_action_filterimages)
        return KIPI::BatchPlugin;
    else if (action == m_action_renameimages)
        return KIPI::BatchPlugin;
    else if (action == m_action_recompressimages)
        return KIPI::BatchPlugin;
    else if (action == m_action_resizeimages)
        return KIPI::BatchPlugin;

    kWarning() << "Unrecognized action for plugin category identification";
    return KIPI::BatchPlugin; // no warning from compiler, please
}
