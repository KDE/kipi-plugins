//////////////////////////////////////////////////////////////////////////////
//
//    PLUGIN_IMAGESGALLERY.CPP
//
//    Copyright (C) 2003-2004 Gilles Caulier <caulier dot gilles at free.fr>
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

// Include files for KDE

#include <klocale.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kapplication.h>

// Local include files

#include "actions.h"
#include "imagesgallery.h"
#include "batchprogressdialog.h"
#include "plugin_imagesgallery.h"

typedef KGenericFactory<Plugin_Imagesgallery> Factory;

K_EXPORT_COMPONENT_FACTORY( kipiplugin_imagesgallery,
                            Factory("kipiplugin_imagesgallery"));

// -----------------------------------------------------------
Plugin_Imagesgallery::Plugin_Imagesgallery(QObject *parent, const char*, const QStringList&)
                    : KIPI::Plugin( Factory::instance(), parent, "ImagesGallery")
{
    kdDebug( 51001 ) << "Plugin_Imagesgallery plugin loaded" << endl;
}

void Plugin_Imagesgallery::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );
    m_actionImagesGallery = new KAction (i18n("HTML export..."),        // Menu message.
                                         "www",                         // Menu icon.
                                         0,
                                         this,
                                         SLOT(slotActivate()),
                                         actionCollection(),
                                        "images_gallery");

    addAction( m_actionImagesGallery );
    m_gallery = 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

Plugin_Imagesgallery::~Plugin_Imagesgallery()
{
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_Imagesgallery::slotActivate()
{
    m_progressDlg = 0;
    
    KIPI::Interface* interface = dynamic_cast< KIPI::Interface* >( parent() );
   
    if ( !interface ) 
       {
       kdError( 51000 ) << "Kipi interface is null!" << endl;
       return;
       }
    
    m_gallery = new KIPIImagesGalleryPlugin::ImagesGallery( 
                    interface, this);
    
    if ( m_gallery->showDialog() )
       {
       if ( m_gallery->removeTargetGalleryFolder() == true)
          m_gallery->start();
       }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_Imagesgallery::slotCancel()
{
    m_gallery->terminate();
    m_gallery->wait();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

void Plugin_Imagesgallery::customEvent(QCustomEvent *event)
{
    if (!event) return;
    
    if (!m_progressDlg)
        {
        m_progressDlg = new KIPIImagesGalleryPlugin::BatchProgressDialog(0);
        
        connect(m_progressDlg, SIGNAL(cancelClicked()),
                this, SLOT(slotCancel()));

        m_current = 0;
        m_progressDlg->show();
        }

    KIPIImagesGalleryPlugin::EventData *d = (KIPIImagesGalleryPlugin::EventData*) event->data();
    
    if (!d) return;
    
    if (d->starting) 
        {
        QString text;
        
        switch (d->action) 
           {
           case(KIPIImagesGalleryPlugin::Initialize): 
              {
              text = i18n("Initialising...");
              m_total = d->total;
              break;
              }
           
           case(KIPIImagesGalleryPlugin::BuildHTMLiface): 
              {
              text = i18n("Making main HTML interface...");
              break;
              }
              
           case(KIPIImagesGalleryPlugin::BuildAlbumHTMLPage): 
              {
              text = i18n("Making HTML pages for Album '%1'...").arg(d->albumName);
              break;
              }
                  
           case(KIPIImagesGalleryPlugin::BuildImageHTMLPage): 
              {
              text = i18n("Making HTML page for image '%1'...").arg(d->fileName);
              break;
              }
           
           case(KIPIImagesGalleryPlugin::ResizeImages): 
              {
              text = i18n("Creating thumbnail for '%1'").arg(d->fileName);
              break;
              }
               
           case(KIPIImagesGalleryPlugin::Progress): 
              {
              text = d->message;
              break;
              }
              
           default: 
              {
              kdWarning( 51000 ) << "Plugin_CDArchiving: Unknown 'Starting' event: " << d->action << endl;
              }
           }

        m_progressDlg->addedAction(text, KIPIImagesGalleryPlugin::StartingMessage);
        }
    else 
        {
        QString text;

        if (d->success) 
            {
            switch (d->action) 
               {
               case(KIPIImagesGalleryPlugin::BuildHTMLiface): 
                  {
                  text = i18n("Main HTML interface creation done!");
                  break;
                  }
                  
               case(KIPIImagesGalleryPlugin::BuildAlbumHTMLPage): 
                  {
                  text = i18n("HTML pages creation for Album '%1' done!").arg(d->albumName);
                  break;
                  }
               
               default: 
                  {
                  kdWarning( 51000 ) << "Plugin_CDArchiving: Unknown 'Success' event: " << d->action << endl;
                  }
               }

            m_progressDlg->addedAction(text, KIPIImagesGalleryPlugin::SucessMessage);
            ++m_current;   
            }
        else
            {
            switch (d->action) 
               {
               case(KIPIImagesGalleryPlugin::ResizeImages): 
                  {
                  text = i18n("Failed to create thumbnail for '%1'").arg(d->fileName);
                  m_progressDlg->addedAction(text, KIPIImagesGalleryPlugin::WarningMessage);
                  m_progressDlg->setProgress(m_current, m_total);
                  break;
                  }
               
               case(KIPIImagesGalleryPlugin::BuildHTMLiface): 
                  {
                  text = i18n("Failed to create HTML interface: %1")
                              .arg(d->message);
                  m_progressDlg->addedAction(text, KIPIImagesGalleryPlugin::ErrorMessage);
                  m_progressDlg->setProgress(m_current, m_total);
                  slotCancel();
                  return;
                  break;
                  }

               case(KIPIImagesGalleryPlugin::BuildAlbumHTMLPage): 
                  {
                  text = i18n("Failed to create HTML page for Album '%1'")
                              .arg(d->albumName);
                  m_progressDlg->addedAction(text, KIPIImagesGalleryPlugin::ErrorMessage);
                  m_progressDlg->setProgress(m_current, m_total);
                  slotCancel();
                  return;
                  break;
                  }

               case(KIPIImagesGalleryPlugin::BuildImageHTMLPage): 
                  {
                  text = i18n("Failed to create HTML page for image '%1'")
                              .arg(d->fileName);
                  m_progressDlg->addedAction(text, KIPIImagesGalleryPlugin::ErrorMessage);
                  m_progressDlg->setProgress(m_current, m_total);
                  slotCancel();
                  return;
                  break;
                  }
                                   
               case(KIPIImagesGalleryPlugin::Error): 
                  {
                  text = d->message;
                  m_progressDlg->addedAction(text, KIPIImagesGalleryPlugin::ErrorMessage);
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

           // TODO
                
        if( d->action == KIPIImagesGalleryPlugin::BuildHTMLiface )
           {
           m_current = 0;
           m_progressDlg->setButtonCancel( KStdGuiItem::close() );
                   
           disconnect(m_progressDlg, SIGNAL(cancelClicked()),
                      this, SLOT(slotCancel()));
                
           // Invoke browser program.
           
           m_progressDlg->addedAction(i18n("Starting browser program..."),
                                      KIPIImagesGalleryPlugin::StartingMessage);
               
           m_gallery->invokeWebBrowser();
           }
        }
    
    kapp->processEvents();
    delete d;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

KIPI::Category Plugin_Imagesgallery::category( KAction* action ) const
{
    if ( action == m_actionImagesGallery )
       return KIPI::EXPORTPLUGIN;
    
    kdWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::EXPORTPLUGIN; // no warning from compiler, please
}

#include "plugin_imagesgallery.moc"
