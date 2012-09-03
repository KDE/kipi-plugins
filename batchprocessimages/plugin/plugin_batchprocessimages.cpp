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
#include <libkipi/imagecollection.h>

// Local includes

#include "borderimagesdialog.h"
#include "colorimagesdialog.h"
#include "convertimagesdialog.h"
#include "effectimagesdialog.h"
#include "filterimagesdialog.h"
#include "recompressimagesdialog.h"
#include "renameimagesdialog.h"
#include "resizeimagesdialog.h"

namespace KIPIBatchProcessImagesPlugin
{

K_PLUGIN_FACTORY(BatchProcessImagesFactory, registerPlugin<Plugin_BatchProcessImages>();)
K_EXPORT_PLUGIN(BatchProcessImagesFactory("kipiplugin_batchprocessimages"))

Plugin_BatchProcessImages::Plugin_BatchProcessImages(QObject* const parent, const QVariantList&)
    : KIPI::Plugin(BatchProcessImagesFactory::componentData(),
                   parent, "BatchProcessImages")
{
    kDebug(AREA_CODE_LOADING) << "Plugin_BatchProcessImages plugin loaded" ;

    setUiBaseName("kipiplugin_batchprocessimagesui.rc");
    setupXML();
}

Plugin_BatchProcessImages::~Plugin_BatchProcessImages()
{
}

void Plugin_BatchProcessImages::setup(QWidget* const widget)
{

    KIPI::Plugin::setup(widget);

    setupActions();

    m_iface = interface();
    if (!m_iface)
    {
        kError() << "Kipi interface is null!" ;
        return;
    }

    KIPI::ImageCollection images = m_iface->currentAlbum();
    bool enable                  = images.isValid() && !images.images().isEmpty();

    m_action_borderimages->setEnabled(enable);

    connect(m_iface, SIGNAL(currentAlbumChanged(bool)),
            m_action_borderimages, SLOT(setEnabled(bool)));

    m_action_colorimages->setEnabled(enable);

    connect(m_iface, SIGNAL(currentAlbumChanged(bool)),
            m_action_colorimages, SLOT(setEnabled(bool)));

    m_action_convertimages->setEnabled(enable);

    connect(m_iface, SIGNAL(currentAlbumChanged(bool)),
            m_action_convertimages, SLOT(setEnabled(bool)));

    m_action_effectimages->setEnabled(enable);

    connect(m_iface, SIGNAL(currentAlbumChanged(bool)),
            m_action_effectimages, SLOT(setEnabled(bool)));

    m_action_filterimages->setEnabled(enable);

    connect(m_iface, SIGNAL(currentAlbumChanged(bool)),
            m_action_filterimages, SLOT(setEnabled(bool)));

    m_action_renameimages->setEnabled(enable);

    connect(m_iface, SIGNAL(currentAlbumChanged(bool)),
            m_action_renameimages, SLOT(setEnabled(bool)));

    m_action_recompressimages->setEnabled(enable);

    connect(m_iface, SIGNAL(currentAlbumChanged(bool)),
            m_action_recompressimages, SLOT(setEnabled(bool)));

    m_action_resizeimages->setEnabled(enable);

    connect(m_iface, SIGNAL(currentAlbumChanged(bool)),
            m_action_resizeimages, SLOT(setEnabled(bool)));
}

void Plugin_BatchProcessImages::setupActions()
{
    setDefaultCategory(BatchPlugin);

    m_action_borderimages = new KAction(this);
    m_action_borderimages->setIcon(KIcon("borderimages"));
    m_action_borderimages->setText(i18n("Border Images..."));
    m_action_borderimages->setEnabled(false);

    connect(m_action_borderimages, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction("batch_border_images", m_action_borderimages);

    m_action_colorimages = new KAction(this);
    m_action_colorimages->setIcon(KIcon("colorimages"));
    m_action_colorimages->setText(i18n("Color Images..."));
    m_action_colorimages->setEnabled(false);

    connect(m_action_colorimages, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction("batch_color_images", m_action_colorimages);

    m_action_convertimages = new KAction(this);
    m_action_convertimages->setIcon(KIcon("convertimages"));
    m_action_convertimages->setText(i18n("Convert Images..."));
    m_action_convertimages->setEnabled(false);

    connect(m_action_convertimages, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction("batch_convert_images", m_action_convertimages);

    m_action_effectimages = new KAction(this);
    m_action_effectimages->setIcon(KIcon("effectimages"));
    m_action_effectimages->setText(i18n("Image Effects..."));
    m_action_effectimages->setEnabled(false);

    connect(m_action_effectimages, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction("batch_effect_images", m_action_effectimages);

    m_action_filterimages = new KAction(this);
    m_action_filterimages->setIcon(KIcon("filterimages"));
    m_action_filterimages->setText(i18n("Filter Images..."));

    connect(m_action_filterimages, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction("batch_filter_images", m_action_filterimages);

    m_action_renameimages = new KAction(this);
    m_action_renameimages->setIcon(KIcon("renameimages"));
    m_action_renameimages->setText(i18n("Rename Images..."));
    m_action_renameimages->setEnabled(false);

    connect(m_action_renameimages, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction("batch_rename_images", m_action_renameimages);

    m_action_recompressimages = new KAction(this);
    m_action_recompressimages->setIcon(KIcon("recompressimages"));
    m_action_recompressimages->setText(i18n("Recompress Images..."));
    m_action_recompressimages->setEnabled(false);

    connect(m_action_recompressimages, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction("batch_recompress_images", m_action_recompressimages);

    m_action_resizeimages = new KAction(this);
    m_action_resizeimages->setIcon(KIcon("resizeimages"));
    m_action_resizeimages->setText(i18n("Resize Images..."));
    m_action_resizeimages->setEnabled(false);

    connect(m_action_resizeimages, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction("batch_resize_images", m_action_resizeimages);
}

void Plugin_BatchProcessImages::slotActivate()
{
    if (!m_iface)
    {
        kError() << "Kipi interface is null!" ;
        return;
    }

    KIPI::ImageCollection images = m_iface->currentSelection();

    if (!images.isValid())
        return;

    if (images.images().isEmpty())
        images = m_iface->currentAlbum();

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
        m_ConvertImagesDialog = new ConvertImagesDialog(urlList, kapp->activeWindow());
        m_ConvertImagesDialog->show();
    }
    else if (from == "batch_rename_images")
    {
        QPointer<RenameImagesDialog> dlg;
        dlg = new RenameImagesDialog(urlList, kapp->activeWindow());
        dlg->exec();
        delete dlg;
    }
    else if (from == "batch_border_images")
    {
        m_BorderImagesDialog = new BorderImagesDialog(urlList, kapp->activeWindow());
        m_BorderImagesDialog->show();
    }
    else if (from == "batch_color_images")
    {
        m_ColorImagesDialog = new ColorImagesDialog(urlList, kapp->activeWindow());
        m_ColorImagesDialog->show();
    }
    else if (from == "batch_filter_images")
    {
        m_FilterImagesDialog = new FilterImagesDialog(urlList, kapp->activeWindow());
        m_FilterImagesDialog->show();
    }
    else if (from == "batch_effect_images")
    {
        m_EffectImagesDialog = new EffectImagesDialog(urlList, kapp->activeWindow());
        m_EffectImagesDialog->show();
    }
    else if (from == "batch_recompress_images")
    {
        m_RecompressImagesDialog = new RecompressImagesDialog(urlList, kapp->activeWindow());
        m_RecompressImagesDialog->show();
    }
    else if (from == "batch_resize_images")
    {
        m_ResizeImagesDialog = new ResizeImagesDialog(urlList, kapp->activeWindow());
        m_ResizeImagesDialog->show();
    }
    else
    {
        kWarning() << "The impossible happened... unknown batch action specified";
        return;
    }
}

}  // namespace KIPIBatchProcessImagesPlugin
