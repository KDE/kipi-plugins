/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-11
 * Description : a plugin to edit pictures metadata
 *
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "plugin_metadataedit.h"
#include "plugin_metadataedit.moc"

// KDE includes.

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

// LibKIPI includes.

#include <libkipi/imagecollection.h>
#include <libkipi/interface.h>

// LibKExiv2 includes.

#include <libkexiv2/kexiv2.h>

// Local includes.

#include "commenteditdialog.h"
#include "commentremovedialog.h"
#include "exifeditdialog.h"
#include "iptceditdialog.h"
#include "xmpeditdialog.h"

K_PLUGIN_FACTORY( MetadataEditFactory, registerPlugin<Plugin_MetadataEdit>(); )
K_EXPORT_PLUGIN ( MetadataEditFactory("kipiplugin_metadataedit") )

Plugin_MetadataEdit::Plugin_MetadataEdit(QObject *parent, const QVariantList&)
                   : KIPI::Plugin(MetadataEditFactory::componentData(), parent, "MetadataEdit")
{
    kDebug( 51001 ) << "Plugin_MetadataEdit plugin loaded";
}

void Plugin_MetadataEdit::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );

    m_actionMetadataEdit = new KActionMenu(KIcon("metadataedit"), i18n("Metadata"), actionCollection());
    m_actionMetadataEdit->setObjectName("metadataedit");
    addAction(m_actionMetadataEdit);

    KAction *editEXIF = new KAction(i18n("Edit EXIF..."), actionCollection());
    editEXIF->setObjectName("editexif");
    connect(editEXIF, SIGNAL(triggered(bool)),
            this, SLOT(slotEditExif()));
    m_actionMetadataEdit->addAction(editEXIF);

    KAction *removeEXIF = new KAction(i18n("Remove EXIF..."), actionCollection());
    removeEXIF->setObjectName("removeexif");
    connect(removeEXIF, SIGNAL(triggered(bool)),
            this, SLOT(slotRemoveExif()));
    m_actionMetadataEdit->addAction(removeEXIF);

    KAction *importEXIF = new KAction(i18n("Import EXIF..."), actionCollection());
    importEXIF->setObjectName("importexif");
    connect(importEXIF, SIGNAL(triggered(bool)),
            this, SLOT(slotImportExif()));
    m_actionMetadataEdit->addAction(importEXIF);

    // -----------------------------------------------------

    m_actionMetadataEdit->menu()->addSeparator();

    KAction *editIPTC = new KAction(i18n("Edit IPTC..."), actionCollection());
    editIPTC->setObjectName("editiptc");
    connect(editIPTC, SIGNAL(triggered(bool)),
            this, SLOT(slotEditIptc()));
    m_actionMetadataEdit->addAction(editIPTC);

    KAction *removeIPTC = new KAction(i18n("Remove IPTC..."), actionCollection());
    removeIPTC->setObjectName("removeiptc");
    connect(removeIPTC, SIGNAL(triggered(bool)),
            this, SLOT(slotRemoveIptc()));
    m_actionMetadataEdit->addAction(removeIPTC);

    KAction *importIPTC = new KAction(i18n("Import IPTC..."), actionCollection());
    importIPTC->setObjectName("importiptc");
    connect(importIPTC, SIGNAL(triggered(bool)),
            this, SLOT(slotImportIptc()));
    m_actionMetadataEdit->addAction(importIPTC);

    // -----------------------------------------------------

    m_actionMetadataEdit->menu()->addSeparator();

    KAction *editXMP = new KAction(i18n("Edit XMP..."), actionCollection());
    editXMP->setObjectName("editxmp");
    connect(editXMP, SIGNAL(triggered(bool)),
            this, SLOT(slotEditXmp()));
    m_actionMetadataEdit->addAction(editXMP);

    KAction *removeXMP = new KAction(i18n("Remove XMP..."), actionCollection());
    removeXMP->setObjectName("removexmp");
    connect(removeXMP, SIGNAL(triggered(bool)),
            this, SLOT(slotRemoveXmp()));
    m_actionMetadataEdit->addAction(removeXMP);

    KAction *importXMP = new KAction(i18n("Import XMP..."), actionCollection());
    importXMP->setObjectName("importxmp");
    connect(importXMP, SIGNAL(triggered(bool)),
            this, SLOT(slotImportXmp()));
    m_actionMetadataEdit->addAction(importXMP);

    // -----------------------------------------------------

    m_actionMetadataEdit->menu()->addSeparator();

    KAction *editComments = new KAction(i18n("Edit Captions..."), actionCollection());
    editComments->setObjectName("editcomments");
    connect(editComments, SIGNAL(triggered(bool)),
            this, SLOT(slotEditComments()));
    m_actionMetadataEdit->addAction(editComments);

    KAction *removeComments = new KAction(i18n("Remove Captions..."), actionCollection());
    removeComments->setObjectName("removecomments");
    connect(removeComments, SIGNAL(triggered(bool)),
            this, SLOT(slotRemoveComments()));
    m_actionMetadataEdit->addAction(removeComments);

    addAction( m_actionMetadataEdit );

    m_interface = dynamic_cast< KIPI::Interface* >( parent() );
    if ( !m_interface )
    {
        kError( 51000 ) << "Kipi interface is null!" << endl;
        return;
    }

    KIPI::ImageCollection selection = m_interface->currentSelection();
    m_actionMetadataEdit->setEnabled( selection.isValid() && !selection.images().isEmpty() );

    connect(m_interface, SIGNAL(selectionChanged(bool)),
            m_actionMetadataEdit, SLOT(setEnabled(bool)));
}

void Plugin_MetadataEdit::slotEditExif()
{
    KIPI::ImageCollection images = m_interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    KIPIMetadataEditPlugin::EXIFEditDialog dialog(kapp->activeWindow(), images.images(), m_interface);
    dialog.exec();
    m_interface->refreshImages(images.images());
}

void Plugin_MetadataEdit::slotRemoveExif()
{
    KIPI::ImageCollection images = m_interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    if (KMessageBox::warningYesNo(
                     kapp->activeWindow(),
                     i18n("EXIF metadata will be permanently removed from all current selected pictures.\n"
                          "Do you want to continue ?"),
                     i18n("Remove EXIF Metadata")) != KMessageBox::Yes)
        return;

    KUrl::List  imageURLs = images.images();
    KUrl::List  updatedURLs;
    QStringList errorFiles;

    for( KUrl::List::iterator it = imageURLs.begin() ;
         it != imageURLs.end(); ++it)
    {
        KUrl url = *it;
        bool ret = false;

        if (KExiv2Iface::KExiv2::canWriteExif(url.path()))
        {
            ret = true;
            KExiv2Iface::KExiv2 exiv2Iface;
            ret &= exiv2Iface.load(url.path());
            ret &= exiv2Iface.clearExif();
            ret &= exiv2Iface.save(url.path());
        }

        if (!ret)
            errorFiles.append(url.fileName());
        else
            updatedURLs.append(url);
    }

    // We use kipi interface refreshImages() method to tell to host than
    // metadata from pictures have changed and need to be re-read.

    m_interface->refreshImages(updatedURLs);

    if (!errorFiles.isEmpty())
    {
        KMessageBox::errorList(
                    kapp->activeWindow(),
                    i18n("Unable to remove EXIF metadata from:"),
                    errorFiles,
                    i18n("Remove EXIF Metadata"));
    }
}

void Plugin_MetadataEdit::slotImportExif()
{
    KIPI::ImageCollection images = m_interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    KUrl importEXIFFile = KFileDialog::getOpenUrl(KGlobalSettings::documentPath(),
                                                  QString::null, kapp->activeWindow(),
                                                  i18n("Select File to Import EXIF metadata") );
    if( importEXIFFile.isEmpty() )
       return;

    KExiv2Iface::KExiv2 exiv2Iface;
    if (!exiv2Iface.load(importEXIFFile.path()))
    {
        KMessageBox::error(kapp->activeWindow(),
                           i18n("Cannot load metadata from \"%1\"", importEXIFFile.fileName()),
                           i18n("Import EXIF Metadata"));
        return;
    }

    QByteArray exifData = exiv2Iface.getExif();
    if (exifData.isEmpty())
    {
        KMessageBox::error(kapp->activeWindow(),
                           i18n("\"%1\" do not have EXIF metadata", importEXIFFile.fileName()),
                           i18n("Import EXIF Metadata"));
        return;
    }

    if (KMessageBox::warningYesNo(
                     kapp->activeWindow(),
                     i18n("EXIF metadata from current selected pictures will be permanently "
                          "replaced by the EXIF content of \"%1\".\n"
                          "Do you want to continue ?", importEXIFFile.fileName()),
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

        if (KExiv2Iface::KExiv2::canWriteExif(url.path()))
        {
            ret = true;
            KExiv2Iface::KExiv2 exiv2Iface;
            ret &= exiv2Iface.load(url.path());
            ret &= exiv2Iface.setExif(exifData);
            ret &= exiv2Iface.save(url.path());
        }

        if (!ret)
            errorFiles.append(url.fileName());
        else
            updatedURLs.append(url);
    }

    // We use kipi interface refreshImages() method to tell to host than
    // metadata from pictures have changed and need to be re-read.

    m_interface->refreshImages(updatedURLs);

    if (!errorFiles.isEmpty())
    {
        KMessageBox::errorList(
                    kapp->activeWindow(),
                    i18n("Unable to set EXIF metadata from:"),
                    errorFiles,
                    i18n("Import EXIF Metadata"));
    }
}

void Plugin_MetadataEdit::slotEditIptc()
{
    KIPI::ImageCollection images = m_interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    KIPIMetadataEditPlugin::IPTCEditDialog dialog(kapp->activeWindow(), images.images(), m_interface);
    dialog.exec();
    m_interface->refreshImages(images.images());
}

void Plugin_MetadataEdit::slotRemoveIptc()
{
    KIPI::ImageCollection images = m_interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    if (KMessageBox::warningYesNo(
                     kapp->activeWindow(),
                     i18n("IPTC metadata will be permanently removed from all current selected pictures.\n"
                          "Do you want to continue ?"),
                     i18n("Remove IPTC Metadata")) != KMessageBox::Yes)
        return;

    KUrl::List  imageURLs = images.images();
    KUrl::List  updatedURLs;
    QStringList errorFiles;

    for( KUrl::List::iterator it = imageURLs.begin() ;
         it != imageURLs.end(); ++it)
    {
        KUrl url = *it;
        bool ret = false;

        if (KExiv2Iface::KExiv2::canWriteIptc(url.path()))
        {
            ret = true;
            KExiv2Iface::KExiv2 exiv2Iface;
            ret &= exiv2Iface.load(url.path());
            ret &= exiv2Iface.clearIptc();
            ret &= exiv2Iface.save(url.path());
        }

        if (!ret)
            errorFiles.append(url.fileName());
        else
            updatedURLs.append(url);
    }

    // We use kipi interface refreshImages() method to tell to host than
    // metadata from pictures have changed and need to be re-read.

    m_interface->refreshImages(updatedURLs);

    if (!errorFiles.isEmpty())
    {
        KMessageBox::errorList(
                    kapp->activeWindow(),
                    i18n("Unable to remove IPTC metadata from:"),
                    errorFiles,
                    i18n("Remove IPTC Metadata"));
    }
}

void Plugin_MetadataEdit::slotImportIptc()
{
    KIPI::ImageCollection images = m_interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    KUrl importIPTCFile = KFileDialog::getOpenUrl(KGlobalSettings::documentPath(),
                                                  QString::null, kapp->activeWindow(),
                                                  i18n("Select File to Import IPTC metadata") );
    if( importIPTCFile.isEmpty() )
       return;

    KExiv2Iface::KExiv2 exiv2Iface;
    if (!exiv2Iface.load(importIPTCFile.path()))
    {
        KMessageBox::error(kapp->activeWindow(),
                           i18n("Cannot load metadata from \"%1\"", importIPTCFile.fileName()),
                           i18n("Import IPTC Metadata"));
        return;
    }

    QByteArray iptcData = exiv2Iface.getIptc();
    if (iptcData.isEmpty())
    {
        KMessageBox::error(kapp->activeWindow(),
                           i18n("\"%1\" do not have IPTC metadata", importIPTCFile.fileName()),
                           i18n("Import IPTC Metadata"));
        return;
    }

    if (KMessageBox::warningYesNo(
                     kapp->activeWindow(),
                     i18n("IPTC metadata from current selected pictures will be permanently "
                          "replaced by the IPTC content of \"%1\".\n"
                          "Do you want to continue ?", importIPTCFile.fileName()),
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

        if (KExiv2Iface::KExiv2::canWriteIptc(url.path()))
        {
            ret = true;
            KExiv2Iface::KExiv2 exiv2Iface;
            ret &= exiv2Iface.load(url.path());
            ret &= exiv2Iface.setIptc(iptcData);
            ret &= exiv2Iface.save(url.path());
        }

        if (!ret)
            errorFiles.append(url.fileName());
        else
            updatedURLs.append(url);
    }

    // We use kipi interface refreshImages() method to tell to host than
    // metadata from pictures have changed and need to be re-read.

    m_interface->refreshImages(updatedURLs);

    if (!errorFiles.isEmpty())
    {
        KMessageBox::errorList(
                    kapp->activeWindow(),
                    i18n("Unable to set IPTC metadata from:"),
                    errorFiles,
                    i18n("Import IPTC Metadata"));
    }
}

void Plugin_MetadataEdit::slotEditXmp()
{
    KIPI::ImageCollection images = m_interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    KIPIMetadataEditPlugin::XMPEditDialog dialog(kapp->activeWindow(), images.images(), m_interface);
    dialog.exec();
    m_interface->refreshImages(images.images());
}

void Plugin_MetadataEdit::slotRemoveXmp()
{
    KIPI::ImageCollection images = m_interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    if (KMessageBox::warningYesNo(
                     kapp->activeWindow(),
                     i18n("XMP metadata will be permanently removed from all current selected pictures.\n"
                          "Do you want to continue ?"),
                     i18n("Remove XMP Metadata")) != KMessageBox::Yes)
        return;

    KUrl::List  imageURLs = images.images();
    KUrl::List  updatedURLs;
    QStringList errorFiles;

    for( KUrl::List::iterator it = imageURLs.begin() ;
         it != imageURLs.end(); ++it)
    {
        KUrl url = *it;
        bool ret = false;

        if (KExiv2Iface::KExiv2::canWriteXmp(url.path()))
        {
            ret = true;
            KExiv2Iface::KExiv2 exiv2Iface;
            ret &= exiv2Iface.load(url.path());
            ret &= exiv2Iface.clearXmp();
            ret &= exiv2Iface.save(url.path());
        }

        if (!ret)
            errorFiles.append(url.fileName());
        else
            updatedURLs.append(url);
    }

    // We use kipi interface refreshImages() method to tell to host than
    // metadata from pictures have changed and need to be re-read.

    m_interface->refreshImages(updatedURLs);

    if (!errorFiles.isEmpty())
    {
        KMessageBox::errorList(
                    kapp->activeWindow(),
                    i18n("Unable to remove XMP metadata from:"),
                    errorFiles,
                    i18n("Remove XMP Metadata"));
    }
}

void Plugin_MetadataEdit::slotImportXmp()
{
    KIPI::ImageCollection images = m_interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    KUrl importXMPFile = KFileDialog::getOpenUrl(KGlobalSettings::documentPath(),
                                                 QString::null, kapp->activeWindow(),
                                                 i18n("Select File to Import XMP metadata") );
    if( importXMPFile.isEmpty() )
       return;

    KExiv2Iface::KExiv2 exiv2Iface;
    if (!exiv2Iface.load(importXMPFile.path()))
    {
        KMessageBox::error(kapp->activeWindow(),
                           i18n("Cannot load metadata from \"%1\"", importXMPFile.fileName()),
                           i18n("Import XMP Metadata"));
        return;
    }

    QByteArray xmpData = exiv2Iface.getXmp();
    if (xmpData.isEmpty())
    {
        KMessageBox::error(kapp->activeWindow(),
                           i18n("\"%1\" do not have XMP metadata", importXMPFile.fileName()),
                           i18n("Import XMP Metadata"));
        return;
    }

    if (KMessageBox::warningYesNo(
                     kapp->activeWindow(),
                     i18n("XMP metadata from current selected pictures will be permanently "
                          "replaced by the XMP content of \"%1\".\n"
                          "Do you want to continue ?", importXMPFile.fileName()),
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

        if (KExiv2Iface::KExiv2::canWriteXmp(url.path()))
        {
            ret = true;
            KExiv2Iface::KExiv2 exiv2Iface;
            ret &= exiv2Iface.load(url.path());
            ret &= exiv2Iface.setXmp(xmpData);
            ret &= exiv2Iface.save(url.path());
        }

        if (!ret)
            errorFiles.append(url.fileName());
        else
            updatedURLs.append(url);
    }

    // We use kipi interface refreshImages() method to tell to host than
    // metadata from pictures have changed and need to be re-read.

    m_interface->refreshImages(updatedURLs);

    if (!errorFiles.isEmpty())
    {
        KMessageBox::errorList(
                    kapp->activeWindow(),
                    i18n("Unable to set XMP metadata from:"),
                    errorFiles,
                    i18n("Import XMP Metadata"));
    }
}

void Plugin_MetadataEdit::slotEditComments()
{
    KIPI::ImageCollection images = m_interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    KIPIMetadataEditPlugin::CommentEditDialog dlg(kapp->activeWindow());

    if (dlg.exec() != KMessageBox::Ok)
        return;

    KUrl::List  imageURLs = images.images();
    KUrl::List  updatedURLs;
    QStringList errorFiles;

    for( KUrl::List::iterator it = imageURLs.begin() ;
         it != imageURLs.end(); ++it)
    {
        KUrl url = *it;
        bool ret = true;

        KIPI::ImageInfo info = m_interface->info(url);
        info.setDescription(dlg.getComments());

        KExiv2Iface::KExiv2 exiv2Iface;
        ret &= exiv2Iface.load(url.path());

        if (ret)
        {
            if (dlg.syncJFIFCommentIsChecked() && exiv2Iface.canWriteComment(url.path()))
                ret &= exiv2Iface.setComments(dlg.getComments().toUtf8());

            if (dlg.syncEXIFCommentIsChecked() && exiv2Iface.canWriteExif(url.path()))
                ret &= exiv2Iface.setExifComment(dlg.getComments());

            if (exiv2Iface.supportXmp() && dlg.syncXMPCaptionIsChecked() && exiv2Iface.canWriteXmp(url.path()))
            {
                ret &= exiv2Iface.setXmpTagStringLangAlt("Xmp.dc.description", dlg.getComments(),
                                                         QString(), false);

                ret &= exiv2Iface.setXmpTagStringLangAlt("Xmp.exif.UserComment", dlg.getComments(),
                                                         QString(), false);
            }

            if (dlg.syncIPTCCaptionIsChecked() && exiv2Iface.canWriteIptc(url.path()))
                ret &= exiv2Iface.setIptcTagString("Iptc.Application2.Caption", dlg.getComments());

            ret &= exiv2Iface.save(url.path());
        }

        if (!ret)
            errorFiles.append(url.fileName());
        else
            updatedURLs.append(url);
    }

    // We use kipi interface refreshImages() method to tell to host than
    // metadata from pictures have changed and need to be re-read.

    m_interface->refreshImages(updatedURLs);

    if (!errorFiles.isEmpty())
    {
        KMessageBox::informationList(
                     kapp->activeWindow(),
                     i18n("Unable to set captions as image metadata from:"),
                     errorFiles,
                     i18n("Edit Image Caption"));
    }
}

void Plugin_MetadataEdit::slotRemoveComments()
{
    KIPI::ImageCollection images = m_interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    KIPIMetadataEditPlugin::CommentRemoveDialog dlg(kapp->activeWindow());

    if (dlg.exec() != KMessageBox::Ok)
        return;

    KUrl::List  imageURLs = images.images();
    KUrl::List  updatedURLs;
    QStringList errorFiles;

    for( KUrl::List::iterator it = imageURLs.begin() ;
         it != imageURLs.end(); ++it)
    {
        KUrl url = *it;
        bool ret = true;

        if (dlg.removeHOSTCommentIsChecked())
        {
            KIPI::ImageInfo info = m_interface->info(url);
            info.setDescription(QString::null);
        }

        KExiv2Iface::KExiv2 exiv2Iface;
        ret &= exiv2Iface.load(url.path());
        if (ret)
        {
            if (dlg.removeJFIFCommentIsChecked() && exiv2Iface.canWriteComment(url.path()))
                ret &= exiv2Iface.setComments(QByteArray());

            if (dlg.removeEXIFCommentIsChecked() && exiv2Iface.canWriteExif(url.path()))
                ret &= exiv2Iface.removeExifTag("Exif.Photo.UserComment");

            if (exiv2Iface.supportXmp() && dlg.removeXMPCaptionIsChecked() && exiv2Iface.canWriteXmp(url.path()))
            {
                ret &= exiv2Iface.removeXmpTag("Xmp.dc.description");
                ret &= exiv2Iface.removeXmpTag("Xmp.exif.UserComment");
            }

            if (dlg.removeIPTCCaptionIsChecked() && exiv2Iface.canWriteIptc(url.path()))
                ret &= exiv2Iface.removeIptcTag("Iptc.Application2.Caption");

            ret &= exiv2Iface.save(url.path());
        }

        if (!ret)
            errorFiles.append(url.fileName());
        else
            updatedURLs.append(url);
    }

    // We use kipi interface refreshImages() method to tell to host than
    // metadata from pictures have changed and need to be re-read.

    m_interface->refreshImages(updatedURLs);

    if (!errorFiles.isEmpty())
    {
        KMessageBox::informationList(
                     kapp->activeWindow(),
                     i18n("Unable to remove caption as image metadata from:"),
                     errorFiles,
                     i18n("Remove Image Caption"));
    }
}

KIPI::Category Plugin_MetadataEdit::category( KAction* action ) const
{
    if ( action == m_actionMetadataEdit )
       return KIPI::ImagesPlugin;

    kWarning( 51000 ) << "Unrecognized action for plugin category identification";
    return KIPI::ImagesPlugin; // no warning from compiler, please
}
