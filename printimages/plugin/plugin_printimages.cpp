/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2009-02-08
 * Description : a kipi plugin to print images
 *
 * Copyright 2009-2012 by Angelo Naselli <anaselli at linux dot it>
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

// To disable warnings under MSVC2008 about POSIX methods().
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#include "plugin_printimages.moc"

// C ANSI includes

extern "C"
{
#include <unistd.h>
}

// Qt includes

#include <QWidget>

// KDE includes

#include <kaction.h>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kgenericfactory.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>

// LibKIPI includes

#include <libkipi/imagecollection.h>
#include <libkipi/interface.h>
#include <libkipi/plugin.h>

// Local includes

#include "printhelper.h"
#include "wizard.h"

namespace KIPIPrintImagesPlugin
{

K_PLUGIN_FACTORY ( PrintImagesFactory, registerPlugin<Plugin_PrintImages>(); )
K_EXPORT_PLUGIN ( PrintImagesFactory ( "kipiplugin_printimages" ) )

Plugin_PrintImages::Plugin_PrintImages ( QObject* const parent, const QVariantList& /*args*/ )
    : Plugin ( PrintImagesFactory::componentData(), parent, "PrintImages" )
{
    kDebug(AREA_CODE_LOADING) << "Plugin_PrintImages plugin loaded" ;
}

void Plugin_PrintImages::setup( QWidget* widget )
{
    Plugin::setup ( widget );

    m_printImagesAction = actionCollection()->addAction ( "printimages" );
    m_printImagesAction->setText ( i18n ( "Print images" ) );
    m_printImagesAction->setIcon ( KIcon ( "document-print" ) );

    connect ( m_printImagesAction, SIGNAL (triggered(bool)),
              this, SLOT (slotPrintImagesActivate()) );

    addAction ( m_printImagesAction );

    m_printAssistantAction = actionCollection()->addAction ( "printassistant" );
    m_printAssistantAction->setText ( i18n ( "Print Assistant..." ) );
    m_printAssistantAction->setIcon ( KIcon ( "document-print" ) );
    addAction ( m_printAssistantAction );

    connect ( m_printAssistantAction, SIGNAL (triggered(bool)),
              this, SLOT (slotPrintAssistantActivate()) );

    m_interface = dynamic_cast<Interface*>(parent());

    if ( !m_interface )
    {
        kError() << "Kipi interface is null!";
        return;
    }

    ImageCollection selection = m_interface->currentSelection();
    m_printImagesAction->setEnabled ( selection.isValid() && !selection.images().isEmpty() );
    m_printAssistantAction->setEnabled ( selection.isValid() && !selection.images().isEmpty() );

    connect ( m_interface, SIGNAL (selectionChanged(bool)),
              m_printImagesAction, SLOT (setEnabled(bool)) );

    connect ( m_interface, SIGNAL (selectionChanged(bool)),
              m_printAssistantAction, SLOT (setEnabled(bool)) );
}

Plugin_PrintImages::~Plugin_PrintImages()
{
}

void Plugin_PrintImages::slotPrintImagesActivate()
{
    ImageCollection album = m_interface->currentSelection();

    if ( !album.isValid() )
        return;

    KUrl::List fileList = album.images();

    if ( fileList.isEmpty() )
    {
        KMessageBox::sorry ( kapp->activeWindow(), i18n ( "Please select one or more photos to print." ),
                            i18n ( "Print images" ) );
        return;
    }

    QWidget* parent = QApplication::activeWindow();
    PrintHelper printPlugin(parent);
    printPlugin.print(fileList);
}

void Plugin_PrintImages::slotPrintAssistantActivate()
{
    ImageCollection album = m_interface->currentSelection();

    if ( !album.isValid() )
        return;

    KUrl::List fileList = album.images();

    if (fileList.isEmpty())
    {
        KMessageBox::sorry(kapp->activeWindow(), i18n("Please select one or more photos to print."),
                           i18n("Print assistant"));
        return;
    }

    QWidget* parent = QApplication::activeWindow();
    Wizard printAssistant(parent);
    KStandardDirs dir;
    QString tempPath = dir.saveLocation("tmp", "kipi-printassistantdplugin-" + QString::number(getpid()) + '/');
    printAssistant.print(fileList, tempPath);

    if (printAssistant.exec() == QDialog::Rejected) return;
}

Category Plugin_PrintImages::category ( KAction* action ) const
{
    if ( action == m_printImagesAction || action == m_printAssistantAction )
        return ImagesPlugin;

    kWarning () << "Unrecognized action for plugin category identification";
    return ImagesPlugin; // no warning from compiler, please
}

} // namespace KIPIPrintImagesPlugin
