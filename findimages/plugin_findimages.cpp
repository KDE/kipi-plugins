//////////////////////////////////////////////////////////////////////////////
//
//    PLUGIN_FINDIMAGES.CPP
//
//    Copyright (C) 2004 Gilles Caulier <caulier dot gilles at free.fr>
//    Copyright (C) 2004 Richard Groult <rgroult at jalix.org>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//////////////////////////////////////////////////////////////////////////////

// Qt includes.

#include <qfileinfo.h>

// KDE includes.

#include <klocale.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kapplication.h>

// Lib KIPI includes.

#include <libkipi/interface.h>
#include <libkipi/batchprogressdialog.h>

// Local include files

#include "finddupplicateimages.h"
#include "actions.h"
#include "plugin_findimages.h"

/////////////////////////////////////////////////////////////////////////////////////////////

typedef KGenericFactory<Plugin_FindImages> Factory;

K_EXPORT_COMPONENT_FACTORY( kipiplugin_findimages,
                            Factory("kipiplugin_findimages"));

Plugin_FindImages::Plugin_FindImages(QObject *parent, const char*, const QStringList&)
                 : KIPI::Plugin( Factory::instance(), parent, "FindImages")
{
    kdDebug( 51001 ) << "Plugin_FindImages plugin loaded" << endl;
}

void Plugin_FindImages::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );

    m_action_findDuplicateImages = new KAction(i18n("&Find Duplicate Images..."),
                                                    "finddupplicateimages",
                                                    0,
                                                    this,
                                                    SLOT(slotFindDuplicateImages()),
                                                    actionCollection(),
                                                    "findduplicateimages");

    addAction( m_action_findDuplicateImages );
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

Plugin_FindImages::~Plugin_FindImages()
{
}


/////////////////////////////////////// SLOTS ///////////////////////////////////////////////////////

void Plugin_FindImages::slotFindDuplicateImages()
{
    m_progressDlg = 0;

    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>( parent() );

    if ( !interface )
           {
           kdError( 51000 ) << "Kipi interface is null!" << endl;
           return;
           }

    m_findDuplicateOperation = new KIPIFindDupplicateImagesPlugin::FindDuplicateImages( interface, this);

    if (m_findDuplicateOperation->showDialog())
       {
       m_findDuplicateOperation->compareAlbums();
       }

    return;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_FindImages::slotCancel()
{
    m_findDuplicateOperation->terminate();
    m_findDuplicateOperation->wait();

    if (m_progressDlg)
       {
       m_progressDlg->reset();
       }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_FindImages::customEvent(QCustomEvent *event)
{
    if (!event) return;

    if (!m_progressDlg)
        {
        m_progressDlg = new KIPI::BatchProgressDialog(0, i18n("Find Duplicate Images"));

        connect(m_progressDlg, SIGNAL(cancelClicked()),
                this, SLOT(slotCancel()));

        m_current = 0;
        m_progressDlg->show();
        }

    KIPIFindDupplicateImagesPlugin::EventData *d = (KIPIFindDupplicateImagesPlugin::EventData*) event->data();

    if (!d) return;

    if (d->starting)
        {
        QString text;

        switch (d->action)
           {
           case(KIPIFindDupplicateImagesPlugin::Similar):
              {
              text = i18n("Similar comparison for '%1'").arg(QFileInfo(d->fileName).fileName() );
              break;
              }

           case(KIPIFindDupplicateImagesPlugin::Exact):
              {
              m_total = d->total;      // Needed because the total can change in this mode !
              text = i18n("Exact comparison for '%1'").arg(QFileInfo(d->fileName).fileName());
              break;
              }

           case(KIPIFindDupplicateImagesPlugin::Matrix):
              {
              text = i18n("Creating fingerprint for '%1'").arg(QFileInfo(d->fileName).fileName());
              break;
              }

           case(KIPIFindDupplicateImagesPlugin::FastParsing):
              {
              text = i18n("Fast parsing for '%1'").arg(QFileInfo(d->fileName).fileName());
              break;
              }

           case(KIPIFindDupplicateImagesPlugin::Progress):
              {
              m_current = 0;
              m_total = d->total;
              text = i18n("Checking 1 image...", "Checking %n images...", (int)(d->total/2));
              break;
              }

           default:
              {
              kdWarning( 51000 ) << "Plugin_FindImages: Unknown starting event: " << d->action << endl;
              }
           }

        m_progressDlg->addedAction(text, KIPI::StartingMessage);
        }
    else
        {
        if (!d->success)
            {
            QString text;

            switch (d->action)
               {
               case(KIPIFindDupplicateImagesPlugin::Matrix):
                  {
                  text = i18n("Failed to create fingerprint for '%1'")
                             .arg(QFileInfo(d->fileName).fileName());
                  break;
                  }
               
               case(KIPIFindDupplicateImagesPlugin::Similar):
                  {
                  text = i18n("Failed to find similar images.");
                  break;
                  }

               case(KIPIFindDupplicateImagesPlugin::Exact):
                  {
                  text = i18n("Failed to find exact image.");
                  break;
                  }

               case(KIPIFindDupplicateImagesPlugin::Progress):
                  {
                  m_total = d->total;
                  text = i18n("Failed to check images...");
                  break;
                  }
                  
               default:
                  {
                  kdWarning( 51000 ) << "Plugin_FindImages: Unknown failed event: " << d->action << endl;
                  }
               }

            m_progressDlg->addedAction(text, KIPI::WarningMessage);
            ++m_current;
            }
        else 
            {
            QString text;
            
            switch (d->action)
               {
               case(KIPIFindDupplicateImagesPlugin::Matrix):
                  {
                  text = i18n("Fingerprint created for '%1'")
                             .arg(QFileInfo(d->fileName).fileName());
                  break;
                  }
                  
               case(KIPIFindDupplicateImagesPlugin::FastParsing):
                  {
                  text = i18n("Fast parsing completed for '%1'")
                             .arg(QFileInfo(d->fileName).fileName());
                  break;
                  }
                  
               case(KIPIFindDupplicateImagesPlugin::Similar):
                  {
                  text = i18n("Finding similar images for '%1' completed.")
                             .arg(QFileInfo(d->fileName).fileName());
                  break;
                  }

               case(KIPIFindDupplicateImagesPlugin::Exact):
                  {
                  text = i18n("Finding exact images for '%1' completed.")
                             .arg(QFileInfo(d->fileName).fileName());
                  break;
                  }
                  
               case(KIPIFindDupplicateImagesPlugin::Progress):
                  {
                  m_total = d->total;
                  text = i18n("Checking images complete...");
                  break;
                  }
                                    
               default:
                  {
                  kdWarning( 51000 ) << "Plugin_FindImages: Unknown success event: " << d->action << endl;
                  }
               }
            
            m_progressDlg->addedAction(text, KIPI::SuccessMessage);
            ++m_current;
            }
            
        m_progressDlg->setProgress(m_current, m_total);

        if( d->action == KIPIFindDupplicateImagesPlugin::Progress )
           {
#if KDE_VERSION >= 0x30200
           m_progressDlg->setButtonCancel( KStdGuiItem::close() );
#else
           m_progressDlg->setButtonCancelText( i18n("&Close") );
#endif

           disconnect(m_progressDlg, SIGNAL(cancelClicked()),
                      this, SLOT(slotCancel()));
           
           m_progressDlg->addedAction(i18n("Displaying results..."),
                                      KIPI::StartingMessage); 
                                                           
           m_findDuplicateOperation->showResult();
           }
        }

    kapp->processEvents();
    delete d;
}

KIPI::Category Plugin_FindImages::category( KAction* action ) const
{
    if ( action == m_action_findDuplicateImages )
       return KIPI::COLLECTIONSPLUGIN;

    kdWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::COLLECTIONSPLUGIN; // no warning from compiler, please
}


#include "plugin_findimages.moc"
