/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-11
 * Description : a plugin to edit pictures metadata
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// LibKExiv2 includes

#include <libkexiv2/version.h>
#include <libkexiv2/kexiv2.h>

// Local includes

#include "commenteditdialog.h"
#include "commentremovedialog.h"
#include "exifeditdialog.h"
#include "iptceditdialog.h"
#include "xmpeditdialog.h"

K_PLUGIN_FACTORY( MetadataEditFactory, registerPlugin<Plugin_MetadataEdit>(); )
K_EXPORT_PLUGIN ( MetadataEditFactory("kipiplugin_metadataedit") )

Plugin_MetadataEdit::Plugin_MetadataEdit(QObject *parent, const QVariantList&)
                   : KIPI::Plugin(MetadataEditFactory::componentData(), parent, "MetadataEdit"),
                     m_actionMetadataEdit(0),
                     m_interface(0),
                     m_lastSelectedDirectory()
{
    kDebug(AREA_CODE_LOADING) << "Plugin_MetadataEdit plugin loaded";
}

void Plugin_MetadataEdit::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );

    m_actionMetadataEdit = new KActionMenu(KIcon("metadataedit"), i18n("Metadata"), actionCollection());
    m_actionMetadataEdit->setObjectName("metadataedit");

    KAction *editEXIF = actionCollection()->addAction("editexif");
    editEXIF->setText(i18n("Edit EXIF..."));
    connect(editEXIF, SIGNAL(triggered(bool)),
            this, SLOT(slotEditExif()));
    m_actionMetadataEdit->addAction(editEXIF);

    KAction *removeEXIF = actionCollection()->addAction("removeexif");
    removeEXIF->setText(i18n("Remove EXIF..."));
    connect(removeEXIF, SIGNAL(triggered(bool)),
            this, SLOT(slotRemoveExif()));
    m_actionMetadataEdit->addAction(removeEXIF);

    KAction *importEXIF = actionCollection()->addAction("importexif");
    importEXIF->setText(i18n("Import EXIF..."));
    connect(importEXIF, SIGNAL(triggered(bool)),
            this, SLOT(slotImportExif()));
    m_actionMetadataEdit->addAction(importEXIF);

    // -----------------------------------------------------

    m_actionMetadataEdit->menu()->addSeparator();

    KAction *editIPTC = actionCollection()->addAction("editiptc");
    editIPTC->setText(i18n("Edit IPTC..."));
    connect(editIPTC, SIGNAL(triggered(bool)),
            this, SLOT(slotEditIptc()));
    m_actionMetadataEdit->addAction(editIPTC);

    KAction *removeIPTC = actionCollection()->addAction("removeiptc");
    removeIPTC->setText(i18n("Remove IPTC..."));
    connect(removeIPTC, SIGNAL(triggered(bool)),
            this, SLOT(slotRemoveIptc()));
    m_actionMetadataEdit->addAction(removeIPTC);

    KAction *importIPTC = actionCollection()->addAction("importiptc");
    importIPTC->setText(i18n("Import IPTC..."));
    connect(importIPTC, SIGNAL(triggered(bool)),
            this, SLOT(slotImportIptc()));
    m_actionMetadataEdit->addAction(importIPTC);

    // -----------------------------------------------------

    m_actionMetadataEdit->menu()->addSeparator();

    KAction *editXMP = actionCollection()->addAction("editxmp");
    editXMP->setText(i18n("Edit XMP..."));
    connect(editXMP, SIGNAL(triggered(bool)),
            this, SLOT(slotEditXmp()));
    m_actionMetadataEdit->addAction(editXMP);

    KAction *removeXMP = actionCollection()->addAction("removexmp");
    removeXMP->setText(i18n("Remove XMP..."));
    connect(removeXMP, SIGNAL(triggered(bool)),
            this, SLOT(slotRemoveXmp()));
    m_actionMetadataEdit->addAction(removeXMP);

    KAction *importXMP = actionCollection()->addAction("importxmp");
    importXMP->setText(i18n("Import XMP..."));
    connect(importXMP, SIGNAL(triggered(bool)),
            this, SLOT(slotImportXmp()));
    m_actionMetadataEdit->addAction(importXMP);

    // -----------------------------------------------------

    m_actionMetadataEdit->menu()->addSeparator();

    KAction *editComments = actionCollection()->addAction("editcomments");
    editComments->setText(i18n("Edit Captions..."));
    connect(editComments, SIGNAL(triggered(bool)),
            this, SLOT(slotEditComments()));
    m_actionMetadataEdit->addAction(editComments);

    KAction *removeComments = actionCollection()->addAction("removecomments");
    removeComments->setText(i18n("Remove Captions..."));
    connect(removeComments, SIGNAL(triggered(bool)),
            this, SLOT(slotRemoveComments()));
    m_actionMetadataEdit->addAction(removeComments);

    addAction( m_actionMetadataEdit );

    m_interface = dynamic_cast< KIPI::Interface* >( parent() );
    if ( !m_interface )
    {
        kError() << "Kipi interface is null!";
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

    QPointer<KIPIMetadataEditPlugin::EXIFEditDialog> dialog = new KIPIMetadataEditPlugin::EXIFEditDialog(
                                                                  kapp->activeWindow(), images.images(), m_interface);
    dialog->exec();
    m_interface->refreshImages(images.images());

    delete dialog;
}

void Plugin_MetadataEdit::slotRemoveExif()
{
    KIPI::ImageCollection images = m_interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    if (KMessageBox::warningYesNo(
                     kapp->activeWindow(),
                     i18n("EXIF metadata will be permanently removed from all currently selected pictures.\n"
                          "Do you want to continue?"),
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
            exiv2Iface.setWriteRawFiles(m_interface->hostSetting("WriteMetadataToRAW").toBool());

#if KEXIV2_VERSION >= 0x000600
            exiv2Iface.setUpdateFileTimeStamp(m_interface->hostSetting("WriteMetadataUpdateFiletimeStamp").toBool());
#endif

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

    // extract the path to the first image:
    if ( m_lastSelectedDirectory.isEmpty() )
    {
        m_lastSelectedDirectory = images.images().first().upUrl();
    }
    KUrl importEXIFFile = KFileDialog::getOpenUrl(m_lastSelectedDirectory,
                                                  QString::null, kapp->activeWindow(),
                                                  i18n("Select File to Import EXIF metadata") );
    if( importEXIFFile.isEmpty() )
       return;
    
    m_lastSelectedDirectory = importEXIFFile.upUrl();

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

        if (KExiv2Iface::KExiv2::canWriteExif(url.path()))
        {
            ret = true;
            KExiv2Iface::KExiv2 exiv2Iface;
            exiv2Iface.setWriteRawFiles(m_interface->hostSetting("WriteMetadataToRAW").toBool());

#if KEXIV2_VERSION >= 0x000600
            exiv2Iface.setUpdateFileTimeStamp(m_interface->hostSetting("WriteMetadataUpdateFiletimeStamp").toBool());
#endif

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

    QPointer<KIPIMetadataEditPlugin::IPTCEditDialog> dialog = new KIPIMetadataEditPlugin::IPTCEditDialog(
                                                                  kapp->activeWindow(), images.images(), m_interface);
    dialog->exec();
    m_interface->refreshImages(images.images());

    delete dialog;
}

void Plugin_MetadataEdit::slotRemoveIptc()
{
    KIPI::ImageCollection images = m_interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    if (KMessageBox::warningYesNo(
                     kapp->activeWindow(),
                     i18n("IPTC metadata will be permanently removed from all currently selected pictures.\n"
                          "Do you want to continue?"),
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
            exiv2Iface.setWriteRawFiles(m_interface->hostSetting("WriteMetadataToRAW").toBool());

#if KEXIV2_VERSION >= 0x000600
            exiv2Iface.setUpdateFileTimeStamp(m_interface->hostSetting("WriteMetadataUpdateFiletimeStamp").toBool());
#endif

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

    // extract the path to the first image:
    if ( m_lastSelectedDirectory.isEmpty() )
    {
        m_lastSelectedDirectory = images.images().first().upUrl();
    }
    KUrl importIPTCFile = KFileDialog::getOpenUrl(m_lastSelectedDirectory,
                                                  QString::null, kapp->activeWindow(),
                                                  i18n("Select File to Import IPTC metadata") );
    if( importIPTCFile.isEmpty() )
       return;
    
    m_lastSelectedDirectory = importIPTCFile.upUrl();

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

        if (KExiv2Iface::KExiv2::canWriteIptc(url.path()))
        {
            ret = true;
            KExiv2Iface::KExiv2 exiv2Iface;
            exiv2Iface.setWriteRawFiles(m_interface->hostSetting("WriteMetadataToRAW").toBool());

#if KEXIV2_VERSION >= 0x000600
            exiv2Iface.setUpdateFileTimeStamp(m_interface->hostSetting("WriteMetadataUpdateFiletimeStamp").toBool());
#endif

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

    QPointer<KIPIMetadataEditPlugin::XMPEditDialog> dialog = new KIPIMetadataEditPlugin::XMPEditDialog(
                                                                 kapp->activeWindow(), images.images(), m_interface);
    dialog->exec();
    m_interface->refreshImages(images.images());

    delete dialog;
}

void Plugin_MetadataEdit::slotRemoveXmp()
{
    KIPI::ImageCollection images = m_interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    if (KMessageBox::warningYesNo(
                     kapp->activeWindow(),
                     i18n("XMP metadata will be permanently removed from all of the currently selected pictures.\n"
                          "Do you want to continue?"),
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
            exiv2Iface.setWriteRawFiles(m_interface->hostSetting("WriteMetadataToRAW").toBool());

#if KEXIV2_VERSION >= 0x000600
            exiv2Iface.setUpdateFileTimeStamp(m_interface->hostSetting("WriteMetadataUpdateFiletimeStamp").toBool());
#endif

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

    // extract the path to the first image:
    if ( m_lastSelectedDirectory.isEmpty() )
    {
        m_lastSelectedDirectory = images.images().first().upUrl();
    }
    KUrl importXMPFile = KFileDialog::getOpenUrl(m_lastSelectedDirectory,
                                                 QString::null, kapp->activeWindow(),
                                                 i18n("Select File to Import XMP metadata") );
    if( importXMPFile.isEmpty() )
       return;
    
    m_lastSelectedDirectory = importXMPFile.upUrl();

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

        if (KExiv2Iface::KExiv2::canWriteXmp(url.path()))
        {
            ret = true;
            KExiv2Iface::KExiv2 exiv2Iface;
            exiv2Iface.setWriteRawFiles(m_interface->hostSetting("WriteMetadataToRAW").toBool());

#if KEXIV2_VERSION >= 0x000600
            exiv2Iface.setUpdateFileTimeStamp(m_interface->hostSetting("WriteMetadataUpdateFiletimeStamp").toBool());
#endif

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

    QString comment;
    if  (images.images().count() == 1)
    {
        KIPI::ImageInfo info = m_interface->info(images.images().first());
        comment              = info.description();
    }

    QPointer<KIPIMetadataEditPlugin::CommentEditDialog> dlg = new KIPIMetadataEditPlugin::CommentEditDialog(
                                                                  comment, kapp->activeWindow());

    if (dlg->exec() != KMessageBox::Ok)
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
        info.setDescription(dlg->getComments());

        KExiv2Iface::KExiv2 exiv2Iface;
        exiv2Iface.setWriteRawFiles(m_interface->hostSetting("WriteMetadataToRAW").toBool());

#if KEXIV2_VERSION >= 0x000600
        exiv2Iface.setUpdateFileTimeStamp(m_interface->hostSetting("WriteMetadataUpdateFiletimeStamp").toBool());
#endif

        ret &= exiv2Iface.load(url.path());

        if (ret)
        {
            if (dlg->syncJFIFCommentIsChecked() && exiv2Iface.canWriteComment(url.path()))
                ret &= exiv2Iface.setComments(dlg->getComments().toUtf8());

            if (dlg->syncEXIFCommentIsChecked() && exiv2Iface.canWriteExif(url.path()))
                ret &= exiv2Iface.setExifComment(dlg->getComments());

            if (exiv2Iface.supportXmp() && dlg->syncXMPCaptionIsChecked() && exiv2Iface.canWriteXmp(url.path()))
            {
                ret &= exiv2Iface.setXmpTagStringLangAlt("Xmp.dc.description", dlg->getComments(),
                                                         QString(), false);

                ret &= exiv2Iface.setXmpTagStringLangAlt("Xmp.exif.UserComment", dlg->getComments(),
                                                         QString(), false);

                ret &= exiv2Iface.setXmpTagStringLangAlt("Xmp.tiff.ImageDescription", dlg->getComments(),
                                                         QString(), false);
            }

            if (dlg->syncIPTCCaptionIsChecked() && exiv2Iface.canWriteIptc(url.path()))
                ret &= exiv2Iface.setIptcTagString("Iptc.Application2.Caption", dlg->getComments());

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

    delete dlg;
}

void Plugin_MetadataEdit::slotRemoveComments()
{
    KIPI::ImageCollection images = m_interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    QPointer<KIPIMetadataEditPlugin::CommentRemoveDialog> dlg = new KIPIMetadataEditPlugin::CommentRemoveDialog(
                                                                    kapp->activeWindow());

    if (dlg->exec() != KMessageBox::Ok)
        return;

    KUrl::List  imageURLs = images.images();
    KUrl::List  updatedURLs;
    QStringList errorFiles;

    for( KUrl::List::iterator it = imageURLs.begin() ;
         it != imageURLs.end(); ++it)
    {
        KUrl url = *it;
        bool ret = true;

        if (dlg->removeHOSTCommentIsChecked())
        {
            KIPI::ImageInfo info = m_interface->info(url);
            info.setDescription(QString::null);
        }

        KExiv2Iface::KExiv2 exiv2Iface;
        exiv2Iface.setWriteRawFiles(m_interface->hostSetting("WriteMetadataToRAW").toBool());

#if KEXIV2_VERSION >= 0x000600
        exiv2Iface.setUpdateFileTimeStamp(m_interface->hostSetting("WriteMetadataUpdateFiletimeStamp").toBool());
#endif

        ret &= exiv2Iface.load(url.path());
        if (ret)
        {
            if (dlg->removeJFIFCommentIsChecked() && exiv2Iface.canWriteComment(url.path()))
                ret &= exiv2Iface.setComments(QByteArray());

            if (dlg->removeEXIFCommentIsChecked() && exiv2Iface.canWriteExif(url.path()))
                ret &= exiv2Iface.removeExifTag("Exif.Photo.UserComment");

            if (exiv2Iface.supportXmp() && dlg->removeXMPCaptionIsChecked() && exiv2Iface.canWriteXmp(url.path()))
            {
                ret &= exiv2Iface.removeXmpTag("Xmp.dc.description");
                ret &= exiv2Iface.removeXmpTag("Xmp.exif.UserComment");
                ret &= exiv2Iface.removeXmpTag("Xmp.tiff.ImageDescription");
            }

            if (dlg->removeIPTCCaptionIsChecked() && exiv2Iface.canWriteIptc(url.path()))
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

    delete dlg;
}

KIPI::Category Plugin_MetadataEdit::category( KAction* action ) const
{
    if ( action == m_actionMetadataEdit )
       return KIPI::ImagesPlugin;

    kWarning() << "Unrecognized action for plugin category identification";
    return KIPI::ImagesPlugin; // no warning from compiler, please
}
