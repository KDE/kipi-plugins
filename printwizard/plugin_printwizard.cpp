/* ============================================================
 * File  : plugin_printwizard.cpp
 * Author: Todd Shoemaker <todd@theshoemakers.net>
 * Date  : 2003-09-30
 * Description :
 *
 * Copyright 2003 by Todd Shoemaker

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
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
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>

// Digikam includes

// Local includes

#include "plugin_printwizard.h"
#include "frmprintwizard.h"
#include <kurl.h>

K_EXPORT_COMPONENT_FACTORY( kipiplugin_printwizard,
                            KGenericFactory<Plugin_PrintWizard>("kipiplugin_printwizard"));

Plugin_PrintWizard::Plugin_PrintWizard(QObject *parent, const char*, const QStringList&)
    : KIPI::Plugin(parent, "PrintWizard")
{
#ifdef TEMPORARILY_REMOVED
    setInstance(KGenericFactory<Plugin_PrintWizard>::instance());
    setXMLFile("plugins/digikamplugin_printwizard.rc");
#endif
    KGlobal::locale()->insertCatalogue("digikamplugin_printwizard");

    kdDebug() << "Plugin_PrintWizard plugin loaded"
              << endl;

    printAction =  new KAction (i18n("Print Wizard..."),
                                "fileprint",
                                0,
                                this,
                                SLOT(slotActivate()),
                                actionCollection(),
                                "printwizard");

#ifdef TEMPORARILY_REMOVED
    printAction->setEnabled(false);

    connect(Digikam::AlbumManager::instance(),
            SIGNAL(signalAlbumItemsSelected(bool)),
            SLOT(slotItemsSelected(bool)));
#endif

    m_interface = dynamic_cast< KIPI::Interface* >( parent );
}


Plugin_PrintWizard::~Plugin_PrintWizard()
{
}


void Plugin_PrintWizard::slotActivate()
{
    KIPI::ImageCollection album = m_interface->currentAlbum();

    // PENDING(blackie) how do we check for an album being selected?
#ifdef TEMPORARILY_REMOVED
    if (!album)
    {
        KMessageBox::sorry(0, i18n("Please select one or more photos to print."),
                           i18n("Print Wizard"));
        return;
    }
#endif

    KURL::List fileList = album.images();

    if (fileList.count() == 0)
    {
        KMessageBox::sorry(0, i18n("Please select one or more photos to print."),
                           i18n("Print Wizard"));
        return;
    }

    FrmPrintWizard frm;
    KStandardDirs dir;
    QString tempPath = dir.saveLocation("tmp", "digikam-printwizard-" + QString::number(getpid()) + "/");
    frm.print(fileList, tempPath);
    frm.exec();
}

void Plugin_PrintWizard::slotItemsSelected(bool selected)
{
    printAction->setEnabled(selected);
}

KIPI::Category Plugin_PrintWizard::category() const
{
    return KIPI::IMAGESPLUGIN;
}
