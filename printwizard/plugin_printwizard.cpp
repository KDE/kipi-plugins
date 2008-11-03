/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-30-09
 * Description : a kipi plugin to print images
 *
 * Copyright 2003 by Todd Shoemaker <todd@theshoemakers.net>
 * Copyright 2007-2008 by Angelo Naselli <anaselli at linux dot it>
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

// C Ansi includes

extern "C"
{
#include <unistd.h>
}

// Include files for Qt

#include <qdir.h>

// Include files for KDE

#include <klocale.h>
#include <kaction.h>
#include <kapplication.h>
#include <kactioncollection.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kurl.h>
#include <libkipi/imagecollection.h>
#include <libkipi/interface.h>
#include <libkipi/plugin.h>

// Local includes

#include "wizard.h"
#include "plugin_printwizard.h"
#include "plugin_printwizard.moc"

K_PLUGIN_FACTORY( PrintAssistantFactory, registerPlugin<Plugin_PrintWizard>(); )
K_EXPORT_PLUGIN( PrintAssistantFactory("kipiplugin_printwizard") )

Plugin_PrintWizard::Plugin_PrintWizard(QObject *parent, const QVariantList &args)
                  : KIPI::Plugin(PrintAssistantFactory::componentData(), parent, "PrintWizard")
{
    kDebug(51001) << "Plugin_PrintWizard plugin loaded" << endl;
}

void Plugin_PrintWizard::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );

    m_printAction = new KAction (KIcon("document-print"), i18n("Print Assistant..."), actionCollection());
    m_printAction->setObjectName("printwizard");
    connect(m_printAction, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    addAction( m_printAction );

    m_interface = dynamic_cast< KIPI::Interface* >( parent() );

    if ( !m_interface ) 
    {
       kError(  ) << "Kipi interface is null!" << endl;
       return;
    }

    KIPI::ImageCollection selection = m_interface->currentSelection();
    m_printAction->setEnabled( selection.isValid() &&
                               !selection.images().isEmpty() );

    connect( m_interface, SIGNAL( selectionChanged( bool ) ),
             m_printAction, SLOT( setEnabled( bool ) )  );
}

Plugin_PrintWizard::~Plugin_PrintWizard()
{
}

void Plugin_PrintWizard::slotActivate()
{
    KIPI::ImageCollection album = m_interface->currentSelection();

    if ( !album.isValid() )
        return;

    KUrl::List fileList = album.images();

    if (fileList.count() == 0)
    {
        KMessageBox::sorry(kapp->activeWindow(), i18n("Please select one or more photos to print."),
                           i18n("Print Wizard"));
        return;
    }

    QWidget* parent=QApplication::activeWindow();
    KIPIPrintWizardPlugin::Wizard printWizard(parent, m_interface);
    if (printWizard.exec()==QDialog::Rejected) return;
#if 0
    KStandardDirs dir;
    QString tempPath = dir.saveLocation("tmp", "kipi-printwizardplugin-" + QString::number(getpid()) + "/");
    printWizard.print(fileList, tempPath);
    printWizard.exec();
#endif
}

KIPI::Category Plugin_PrintWizard::category( KAction* action ) const
{
    if ( action == m_printAction )
       return KIPI::ImagesPlugin;

    kWarning(51000) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::ImagesPlugin; // no warning from compiler, please
}
