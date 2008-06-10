//////////////////////////////////////////////////////////////////////////////
//
//    PLUGIN_CDARCHIVING.CPP
//
//    Copyright (C) 2003-2004 Gilles Caulier <caulier dot gilles at gmail dot com>
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
//    Foundation, Inc., 51 Franklin Street, Fifth Floor, Cambridge, MA 02110-1301, USA.
//
//////////////////////////////////////////////////////////////////////////////

// KDE includes.

#include <kaction.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kdeversion.h>
#include <kgenericfactory.h>
#include <kinstance.h>
#include <klibloader.h>
#include <klocale.h>

// LibKipi includes.

#include <libkipi/batchprogressdialog.h>

// Local includes.

#include "actions.h"
#include "cdarchiving.h"
#include "plugin_cdarchiving.h"

typedef KGenericFactory<Plugin_CDArchiving> Factory;

K_EXPORT_COMPONENT_FACTORY( kipiplugin_cdarchiving,
                            Factory("kipiplugin_cdarchiving"))

// -----------------------------------------------------------
Plugin_CDArchiving::Plugin_CDArchiving(QObject *parent, const char*, const QStringList&)
                  : KIPI::Plugin( Factory::instance(), parent, "CDArchiving")
{
    kdDebug( 51001 ) << "Plugin_CDArchiving plugin loaded" << endl;
}

void Plugin_CDArchiving::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );

    m_action_cdarchiving = new KAction (i18n("Archive to CD/DVD..."),     // Menu message.
                                        "cd",                             // Menu icon.
                                        0,
                                        this,
                                        SLOT(slotActivate()),
                                        actionCollection(),
                                        "cd_archiving");

    addAction( m_action_cdarchiving );
    m_cdarchiving = 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

Plugin_CDArchiving::~Plugin_CDArchiving()
{
    // No need to delete m_cdarchiving as its a QObject child of this
    // and will be deleted automatically
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_CDArchiving::slotActivate()
{
    m_progressDlg = 0;

    KIPI::Interface* interface = dynamic_cast< KIPI::Interface* >( parent() );

    if ( !interface )
       {
       kdError( 51000 ) << "Kipi interface is null!" << endl;
       return;
       }

    m_cdarchiving = new KIPICDArchivingPlugin::CDArchiving(
                        interface,
                        this, m_action_cdarchiving);

    if ( m_cdarchiving->showDialog() )
       {
       m_cdarchiving->prepare();
       m_cdarchiving->run();
       }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_CDArchiving::slotCancel()
{
    m_cdarchiving->stop();
    m_cdarchiving->removeTmpFiles();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_CDArchiving::customEvent(QCustomEvent *event)
{
    if (!event) return;

    if (!m_progressDlg)
        {
        m_progressDlg = new KIPI::BatchProgressDialog(kapp->activeWindow(), i18n("Archive to CD/DVD"));

        connect(m_progressDlg, SIGNAL(cancelClicked()),
                this, SLOT(slotCancel()));

        m_current = 0;
        m_progressDlg->show();
        }

    KIPICDArchivingPlugin::EventData *d = (KIPICDArchivingPlugin::EventData*) event->data();

    if (!d) return;

    if (d->starting)
        {
        QString text;

        switch (d->action)
           {
           case(KIPICDArchivingPlugin::Initialize):
              {
              text = i18n("Initialising...");
              m_total = d->total;
              break;
              }

           case(KIPICDArchivingPlugin::BuildHTMLiface):
              {
              text = i18n("Making main HTML interface...");
              break;
              }

           case(KIPICDArchivingPlugin::BuildAlbumHTMLPage):
              {
              text = i18n("Making HTML pages for Album '%1'...").arg(d->albumName);
              break;
              }

           case(KIPICDArchivingPlugin::BuildAutoRuniface):
              {
              text = i18n("Making AutoRun interface...");
              break;
              }

           case(KIPICDArchivingPlugin::ResizeImages):
              {
              text = i18n("Creating thumbnail for '%1'...").arg(d->fileName);
              break;
              }

           case(KIPICDArchivingPlugin::BuildK3bProject):
              {
              text = i18n("Making K3b project...");
              break;
              }

           case(KIPICDArchivingPlugin::Progress):
              {
              text = d->message;
              break;
              }

           default:
              {
              kdWarning( 51000 ) << "Plugin_CDArchiving: Unknown 'Starting' event: " << d->action << endl;
              }
           }

        m_progressDlg->addedAction(text, KIPI::StartingMessage);
        }
    else
        {
        QString text;

        if (d->success)
            {
            switch (d->action)
               {
               case(KIPICDArchivingPlugin::BuildHTMLiface):
                  {
                  ++m_current;
                  text = i18n("Main HTML interface creation completed.");
                  break;
                  }

               case(KIPICDArchivingPlugin::BuildAlbumHTMLPage):
                  {
                  ++m_current;
                  text = i18n("HTML page creation for Album '%1' completed.").arg(d->albumName);
                  break;
                  }

              case(KIPICDArchivingPlugin::ResizeImages):
                  {
                  ++m_current;
                  text = i18n("Creating thumbnail for '%1' done.").arg(d->fileName);
                  break;
                  }

               case(KIPICDArchivingPlugin::BuildAutoRuniface):
                  {
                  ++m_current;
                  text = i18n("AutoRun interface creation completed.");
                  break;
                  }

               case(KIPICDArchivingPlugin::BuildK3bProject):
                  {
                  ++m_current;
                  text = i18n("K3b project creation completed.");
                  break;
                  }

               default:
                  {
                  kdWarning( 51000 ) << "Plugin_CDArchiving: Unknown 'Success' event: " << d->action << endl;
                  }
               }

            m_progressDlg->addedAction(text, KIPI::SuccessMessage);
            }
        else
            {
            switch (d->action)
               {
               case(KIPICDArchivingPlugin::ResizeImages):
                  {
                  text = i18n("Failed to create thumbnail for '%1'").arg(d->fileName);
                  m_progressDlg->addedAction(text, KIPI::WarningMessage);
                  m_progressDlg->setProgress(m_current, m_total);
                  break;
                  }

               case(KIPICDArchivingPlugin::BuildHTMLiface):
                  {
                  ++m_current;
                  text = i18n("Failed to create HTML interface: %1")
                              .arg(d->message);
                  m_progressDlg->addedAction(text, KIPI::ErrorMessage);
                  m_progressDlg->setProgress(m_current, m_total);
                  slotCancel();
                  return;
                  break;
                  }

               case(KIPICDArchivingPlugin::BuildAlbumHTMLPage):
                  {
                  text = i18n("Failed to create HTML pages for Album '%1'")
                              .arg(d->albumName);
                  m_progressDlg->addedAction(text, KIPI::ErrorMessage);
                  m_progressDlg->setProgress(m_current, m_total);
                  slotCancel();
                  return;
                  break;
                  }

               case(KIPICDArchivingPlugin::BuildK3bProject):
                  {
                  ++m_current;
                  text = i18n("Failed to create K3b project.");
                  m_progressDlg->addedAction(text, KIPI::ErrorMessage);
                  m_progressDlg->setProgress(m_current, m_total);
                  slotCancel();
                  return;
                  break;
                  }

               case(KIPICDArchivingPlugin::Error):
                  {
                  text = d->message;
                  m_progressDlg->addedAction(text, KIPI::ErrorMessage);
                  m_progressDlg->setProgress(m_current, m_total);
                  slotCancel();
                  return;
                  break;
                  }

               default:
                  {
                  kdWarning( 51000 ) << "Plugin_CDArchiving: Unknown 'Failed' event: " << d->action << endl;
                  }
               }
            }

        m_progressDlg->setProgress(m_current, m_total);

        if( d->action == KIPICDArchivingPlugin::BuildK3bProject )
           {
           m_current = 0;

#if KDE_VERSION >= 0x30200
           m_progressDlg->setButtonCancel( KStdGuiItem::close() );
#else
           m_progressDlg->setButtonCancelText( i18n("&Close") );
#endif

           disconnect(m_progressDlg, SIGNAL(cancelClicked()),
                      this, SLOT(slotCancel()));

           // Invoke K3b program.

           m_progressDlg->addedAction(i18n("Starting K3b program..."),
                                      KIPI::StartingMessage);
           m_cdarchiving->invokeK3b();
           }
        }

    kapp->processEvents();
    delete d;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

KIPI::Category Plugin_CDArchiving::category( KAction* action ) const
{
    if ( action == m_action_cdarchiving )
       return KIPI::EXPORTPLUGIN;

    kdWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::EXPORTPLUGIN; // no warning from compiler, please
}


#include "plugin_cdarchiving.moc"
