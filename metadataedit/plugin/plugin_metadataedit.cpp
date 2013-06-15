/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-10-11
 * Description : a plugin to edit pictures metadata
 *
 * Copyright (C) 2006-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2011-2012 by Victor Dodon <dodonvictor at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "plugin_metadataedit.moc"

// Qt includes

#include <QPointer>

// KDE includes

#include <kaction.h>
#include <kactioncollection.h>
#include <kactionmenu.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <kgenericfactory.h>
#include <kglobalsettings.h>
#include <klibloader.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>

// LibKIPI includes

#include <libkipi/imagecollection.h>
#include <libkipi/interface.h>
#include <libkipi/pluginloader.h>

// Local includes

#include "metadataedit.h"
#include "kpmetadata.h"

using namespace KIPIPlugins;

namespace KIPIMetadataEditPlugin
{

K_PLUGIN_FACTORY( MetadataEditFactory, registerPlugin<Plugin_MetadataEdit>(); )
K_EXPORT_PLUGIN ( MetadataEditFactory("kipiplugin_metadataedit") )

class Plugin_MetadataEdit::Plugin_MetadataEditPriv
{
public:

    Plugin_MetadataEditPriv()
    {
        actionMetadataEdit = 0;
        interface          = 0;
        parentWidget       = 0;
    }

    QWidget*   parentWidget;

    KAction*   actionMetadataEdit;

    Interface* interface;

    KUrl       lastSelectedDirectory;
};

Plugin_MetadataEdit::Plugin_MetadataEdit(QObject* const parent, const QVariantList&)
    : Plugin(MetadataEditFactory::componentData(), parent, "MetadataEdit"),
    d(new Plugin_MetadataEditPriv)
{
    kDebug(AREA_CODE_LOADING) << "Plugin_MetadataEdit plugin loaded";

    setUiBaseName("kipiplugin_metadataeditui.rc");
    setupXML();
}

Plugin_MetadataEdit::~Plugin_MetadataEdit()
{
    delete d;
}

void Plugin_MetadataEdit::setup(QWidget* const widget)
{
    d->parentWidget = widget;
    Plugin::setup(widget);

    setupActions();

    d->interface = interface();
    if (!d->interface)
    {
        kError() << "Kipi interface is null!";
        return;
    }

    ImageCollection selection = d->interface->currentSelection();
    d->actionMetadataEdit->setEnabled( selection.isValid() && !selection.images().isEmpty() );

    connect(d->interface, SIGNAL(selectionChanged(bool)),
            d->actionMetadataEdit, SLOT(setEnabled(bool)));
}

void Plugin_MetadataEdit::setupActions()
{
    setDefaultCategory(ImagesPlugin);

    // NOTE This is a workaround, maybe a better solution?
    if (PluginLoader::instance()->disabledPluginActions().contains("metadataedit"))
        return;

    d->actionMetadataEdit = actionCollection()->addAction("metadataedit");
    d->actionMetadataEdit->setText(i18n("&Metadata"));
    d->actionMetadataEdit->setIcon(KIcon("kipi-metadataedit"));
    d->actionMetadataEdit->setEnabled(false);

    KMenu* metadataEditMenu = new KMenu(d->parentWidget);
    d->actionMetadataEdit->setMenu(metadataEditMenu);

    KAction* metadataEdit = new KAction(this);
    metadataEdit->setText(i18n("Edit &All Metadata..."));
    connect(metadataEdit, SIGNAL(triggered(bool)),
            this,SLOT(slotEditAllMetadata()));
    metadataEditMenu->addAction(metadataEdit);

    addAction("editallmetadata", metadataEdit);

    // ---------------------------------------------------

    d->actionMetadataEdit->menu()->addSeparator();

    KAction* importEXIF = new KAction(this);
    importEXIF->setText(i18n("Import EXIF..."));
    connect(importEXIF, SIGNAL(triggered(bool)),
            this, SLOT(slotImportExif()));
    metadataEditMenu->addAction(importEXIF);

    addAction("importexif", importEXIF);

    KAction* importIPTC = new KAction(this);
    importIPTC->setText(i18n("Import IPTC..."));
    connect(importIPTC, SIGNAL(triggered(bool)),
            this, SLOT(slotImportIptc()));
    metadataEditMenu->addAction(importIPTC);

    addAction("importiptc", importIPTC);

    KAction* importXMP = new KAction(this);
    importXMP->setText(i18n("Import XMP..."));
    connect(importXMP, SIGNAL(triggered(bool)),
            this, SLOT(slotImportXmp()));
    metadataEditMenu->addAction(importXMP);

    addAction("importxmp", importXMP);
}

void Plugin_MetadataEdit::slotEditAllMetadata()
{
    ImageCollection images = d->interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    QPointer<MetadataEditDialog> dialog = new MetadataEditDialog(kapp->activeWindow(), images.images());
    dialog->exec();

    delete dialog;
}

void Plugin_MetadataEdit::slotImportExif()
{
    ImageCollection images = d->interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    // extract the path to the first image:
    if ( d->lastSelectedDirectory.isEmpty() )
    {
        d->lastSelectedDirectory = images.images().first().upUrl();
    }
    KUrl importEXIFFile = KFileDialog::getOpenUrl(d->lastSelectedDirectory,
                                                  QString(), kapp->activeWindow(),
                                                  i18n("Select File to Import EXIF metadata") );
    if( importEXIFFile.isEmpty() )
       return;

    d->lastSelectedDirectory = importEXIFFile.upUrl();

    KPMetadata meta;
    if (!meta.load(importEXIFFile.path()))
    {
        KMessageBox::error(kapp->activeWindow(),
                           i18n("Cannot load metadata from \"%1\"", importEXIFFile.fileName()),
                           i18n("Import EXIF Metadata"));
        return;
    }

#if KEXIV2_VERSION >= 0x010000
    QByteArray exifData = meta.getExifEncoded();
#else
    QByteArray exifData = meta.getExif();
#endif

    if (exifData.isEmpty())
    {
        KMessageBox::error(kapp->activeWindow(),
                           i18n("\"%1\" does not have EXIF metadata.", importEXIFFile.fileName()),
                           i18n("Import EXIF Metadata"));
        return;
    }

    if (KMessageBox::warningYesNo(
                     kapp->activeWindow(),
                     i18n("EXIF metadata from the currently selected pictures will be permanently "
                          "replaced by the EXIF content of \"%1\".\n"
                          "Do you want to continue?", importEXIFFile.fileName()),
                     i18n("Import EXIF Metadata")) != KMessageBox::Yes)
        return;

    KUrl::List  imageURLs = images.images();
    KUrl::List  updatedURLs;
    QStringList errorFiles;

    for( KUrl::List::iterator it = imageURLs.begin() ;
         it != imageURLs.end(); ++it)
    {
        KUrl url = *it;
        bool ret = false;

        if (KPMetadata::canWriteExif(url.path()))
        {
            ret = true;
            KPMetadata meta;

            ret &= meta.load(url.path());
            ret &= meta.setExif(exifData);
            ret &= meta.save(url.path());
        }

        if (!ret)
            errorFiles.append(url.fileName());
        else
            updatedURLs.append(url);
    }

    // We use kipi interface refreshImages() method to tell to host than
    // metadata from pictures have changed and need to be re-read.

    d->interface->refreshImages(updatedURLs);

    if (!errorFiles.isEmpty())
    {
        KMessageBox::errorList(
                    kapp->activeWindow(),
                    i18n("Unable to set EXIF metadata from:"),
                    errorFiles,
                    i18n("Import EXIF Metadata"));
    }
}

void Plugin_MetadataEdit::slotImportIptc()
{
    ImageCollection images = d->interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    // extract the path to the first image:
    if ( d->lastSelectedDirectory.isEmpty() )
    {
        d->lastSelectedDirectory = images.images().first().upUrl();
    }
    KUrl importIPTCFile = KFileDialog::getOpenUrl(d->lastSelectedDirectory,
                                                  QString(), kapp->activeWindow(),
                                                  i18n("Select File to Import IPTC metadata") );
    if( importIPTCFile.isEmpty() )
       return;

    d->lastSelectedDirectory = importIPTCFile.upUrl();

    KPMetadata meta;
    if (!meta.load(importIPTCFile.path()))
    {
        KMessageBox::error(kapp->activeWindow(),
                           i18n("Cannot load metadata from \"%1\"", importIPTCFile.fileName()),
                           i18n("Import IPTC Metadata"));
        return;
    }

    QByteArray iptcData = meta.getIptc();
    if (iptcData.isEmpty())
    {
        KMessageBox::error(kapp->activeWindow(),
                           i18n("\"%1\" does not have IPTC metadata.", importIPTCFile.fileName()),
                           i18n("Import IPTC Metadata"));
        return;
    }

    if (KMessageBox::warningYesNo(
                     kapp->activeWindow(),
                     i18n("IPTC metadata from the currently selected pictures will be permanently "
                          "replaced by the IPTC content of \"%1\".\n"
                          "Do you want to continue?", importIPTCFile.fileName()),
                     i18n("Import IPTC Metadata")) != KMessageBox::Yes)
        return;

    KUrl::List  imageURLs = images.images();
    KUrl::List  updatedURLs;
    QStringList errorFiles;

    for( KUrl::List::iterator it = imageURLs.begin() ;
         it != imageURLs.end(); ++it)
    {
        KUrl url = *it;
        bool ret = false;

        if (KPMetadata::canWriteIptc(url.path()))
        {
            ret = true;
            KPMetadata meta;

            ret &= meta.load(url.path());
            ret &= meta.setIptc(iptcData);
            ret &= meta.save(url.path());
        }

        if (!ret)
            errorFiles.append(url.fileName());
        else
            updatedURLs.append(url);
    }

    // We use kipi interface refreshImages() method to tell to host than
    // metadata from pictures have changed and need to be re-read.

    d->interface->refreshImages(updatedURLs);

    if (!errorFiles.isEmpty())
    {
        KMessageBox::errorList(
                    kapp->activeWindow(),
                    i18n("Unable to set IPTC metadata from:"),
                    errorFiles,
                    i18n("Import IPTC Metadata"));
    }
}

void Plugin_MetadataEdit::slotImportXmp()
{
    ImageCollection images = d->interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    // extract the path to the first image:
    if ( d->lastSelectedDirectory.isEmpty() )
    {
        d->lastSelectedDirectory = images.images().first().upUrl();
    }
    KUrl importXMPFile = KFileDialog::getOpenUrl(d->lastSelectedDirectory,
                                                 QString(), kapp->activeWindow(),
                                                 i18n("Select File to Import XMP metadata") );
    if( importXMPFile.isEmpty() )
       return;

    d->lastSelectedDirectory = importXMPFile.upUrl();

    KPMetadata meta;
    if (!meta.load(importXMPFile.path()))
    {
        KMessageBox::error(kapp->activeWindow(),
                           i18n("Cannot load metadata from \"%1\"", importXMPFile.fileName()),
                           i18n("Import XMP Metadata"));
        return;
    }

    QByteArray xmpData = meta.getXmp();
    if (xmpData.isEmpty())
    {
        KMessageBox::error(kapp->activeWindow(),
                           i18n("\"%1\" does not have XMP metadata.", importXMPFile.fileName()),
                           i18n("Import XMP Metadata"));
        return;
    }

    if (KMessageBox::warningYesNo(
                     kapp->activeWindow(),
                     i18n("XMP metadata from the currently selected pictures will be permanently "
                          "replaced by the XMP content of \"%1\".\n"
                          "Do you want to continue?", importXMPFile.fileName()),
                     i18n("Import XMP Metadata")) != KMessageBox::Yes)
        return;

    KUrl::List  imageURLs = images.images();
    KUrl::List  updatedURLs;
    QStringList errorFiles;

    for( KUrl::List::iterator it = imageURLs.begin() ;
         it != imageURLs.end(); ++it)
    {
        KUrl url = *it;
        bool ret = false;

        if (KPMetadata::canWriteXmp(url.path()))
        {
            ret = true;
            KPMetadata meta;

            ret &= meta.load(url.path());
            ret &= meta.setXmp(xmpData);
            ret &= meta.save(url.path());
        }

        if (!ret)
            errorFiles.append(url.fileName());
        else
            updatedURLs.append(url);
    }

    // We use kipi interface refreshImages() method to tell to host than
    // metadata from pictures have changed and need to be re-read.

    d->interface->refreshImages(updatedURLs);

    if (!errorFiles.isEmpty())
    {
        KMessageBox::errorList(
                    kapp->activeWindow(),
                    i18n("Unable to set XMP metadata from:"),
                    errorFiles,
                    i18n("Import XMP Metadata"));
    }
}

} // namespace KIPIMetadataEditPlugin
