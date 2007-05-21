/* ============================================================
 * Author: Todd Shoemaker <todd@theshoemakers.net>
 * Date  : 2003-09-30
 * Description :
 *
 * Copyright 2003 by Todd Shoemaker <todd@theshoemakers.net>

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
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
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kurl.h>

// Local includes

#include "frmprintwizard.h"
#include "plugin_printwizard.h"
#include "plugin_printwizard.moc"

typedef KGenericFactory<Plugin_PrintWizard> Factory;

K_EXPORT_COMPONENT_FACTORY( kipiplugin_printwizard,
                            Factory("kipiplugin_printwizard"))

Plugin_PrintWizard::Plugin_PrintWizard(QObject *parent, const char*, const QStringList&)
    : KIPI::Plugin(Factory::instance(), parent, "PrintWizard")
{
    kdDebug( 51001 ) << "Plugin_PrintWizard plugin loaded"
                     << endl;
}

void Plugin_PrintWizard::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );

    m_printAction =  new KAction (i18n("Print Wizard..."),
                                  "fileprint",
                                  CTRL+Key_P,
                                  this,
                                  SLOT(slotActivate()),
                                  actionCollection(),
                                  "printwizard");

    addAction( m_printAction );

    m_interface = dynamic_cast< KIPI::Interface* >( parent() );
        
    if ( !m_interface ) 
    {
       kdError( 51000 ) << "Kipi interface is null!" << endl;
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

    KURL::List fileList = album.images();

    if (fileList.count() == 0)
    {
        KMessageBox::sorry(kapp->activeWindow(), i18n("Please select one or more photos to print."),
                           i18n("Print Wizard"));
        return;
    }

    KIPIPrintWizardPlugin::FrmPrintWizard frm(kapp->activeWindow());
    KStandardDirs dir;
    QString tempPath = dir.saveLocation("tmp", "kipi-printwizardplugin-" + QString::number(getpid()) + "/");
    frm.print(fileList, tempPath);
    frm.exec();
}

KIPI::Category Plugin_PrintWizard::category( KAction* action ) const
{
    if ( action == m_printAction )
       return KIPI::IMAGESPLUGIN;
    
    kdWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::IMAGESPLUGIN; // no warning from compiler, please
}

