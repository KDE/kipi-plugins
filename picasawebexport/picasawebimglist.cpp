/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-02-01
 * Description : a kipi plugin to export images to Picasa web service
 *
 * Copyright (C) 2010 by Jens Mueller <tschenser at gmx dot de>
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

#include "picasawebimglist.moc"


// Qt includes

#include <QPointer>

// KDE includes

#include <kdebug.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <kimageio.h>
#include <kglobalsettings.h>
#include "imagedialog.h"

// LibKDcraw includes

#include <libkdcraw/kdcraw.h>

using namespace KDcrawIface;
namespace KIPIPicasawebExportPlugin
{

static const char picasaweb_video_extentions[] = "*.3gp *.avi *.asf *.mov *.wmv *.mpg *.mp4 *.m2t *.mmv *.m2ts *.3gpp";

PicasawebImagesList::PicasawebImagesList(Interface *iface, QWidget* parent, int iconSize)
  : ImagesList(iface, parent, iconSize)
{

}

PicasawebImagesList::~PicasawebImagesList()
{
}

void PicasawebImagesList::slotAddItems()
{
    PicasawebImageDialog dlg(this, iface());
    KUrl::List urls = dlg.urls();
    if (!urls.isEmpty())
        slotAddImages(urls);

    emit signalImageListChanged();
}

// ------------------------------------------------------------------------

class PicasawebImageDialogPrivate
{

public:

    PicasawebImageDialogPrivate()
    {
    }

    KUrl::List       urls;
};

// ------------------------------------------------------------------------

PicasawebImageDialog::PicasawebImageDialog(QWidget* parent, KIPI::Interface* iface)
    : d(new PicasawebImageDialogPrivate)
{
    QStringList patternList;
    QString     allPictures;
    QString     allSupportedFiles;

    patternList = KImageIO::pattern(KImageIO::Reading).split('\n', QString::SkipEmptyParts);

    // All Images from list must been always the first entry given by KDE API
    allPictures = patternList[0];

    allPictures.insert(allPictures.indexOf("|"), QString(KDcraw::rawFiles()) + QString(" *.JPE *.TIF"));
    patternList.removeAll(patternList[0]);
    patternList.prepend(allPictures);

    allSupportedFiles = i18n("%1|All supported files", 
                               QString("*.") + QString(KImageIO::types(KImageIO::Reading).join(" *."))
                             + QString(" ")  + QString(KDcraw::rawFiles())
                             + QString(" *.JPE *.TIF ")
                             + QString(picasaweb_video_extentions));

    patternList.prepend(allSupportedFiles);
    kDebug() << allSupportedFiles;

    // Added RAW file formats supported by dcraw program like a type mime.
    // Nota: we cannot use here "image/x-raw" type mime from KDE because it uncomplete
    // or unavailable(see file #121242 in B.K.O).
    patternList.append(i18n("%1|Camera RAW files", QString(KDcraw::rawFiles())));

    patternList.append(i18n("%1|Video files", QString(picasaweb_video_extentions)));

    QString fileFormats = patternList.join("\n");

    QPointer<KFileDialog> dlg = new KFileDialog(iface ? iface->currentAlbum().path().path()
                                                         : KGlobalSettings::documentPath(),
                                                fileFormats, parent);
    KIPIPlugins::ImageDialogPreview *preview = new KIPIPlugins::ImageDialogPreview(iface, dlg);
    dlg->setPreviewWidget(preview);
    dlg->setOperationMode(KFileDialog::Opening);

    dlg->setMode( KFile::Files );
    dlg->setWindowTitle(i18n("Select Images"));
    dlg->exec();
    d->urls = dlg->selectedUrls();

    delete dlg;
}

PicasawebImageDialog::~PicasawebImageDialog()
{
    delete d;
}

KUrl::List PicasawebImageDialog::urls() const
{
    return d->urls;
}


} // namespace KIPIPicasawebExportPlugin
