/* ============================================================
 * Author: Gilles Caulier <caulier dot gilles at free.fr>
 * Date  : 2003-10-01
 * Description : a kipi plugin to e-mailing images
 * 
 * Copyright 2003-2005 by Gilles Caulier
 *
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

// Include files for KDE

#include <klocale.h>
#include <kapplication.h>
#include <kglobal.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kinstance.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <ktempfile.h>
#include <kimageio.h>
#include <kdeversion.h>

// Lib KIPI includes.

#include <libkipi/batchprogressdialog.h>

// Local includes

#include "actions.h"
#include "sendimages.h"
#include "plugin_sendimages.h"
#include "plugin_sendimages.moc"

typedef KGenericFactory<Plugin_SendImages> Factory;

K_EXPORT_COMPONENT_FACTORY( kipiplugin_sendimages,
                            Factory("kipiplugin_sendimages"))

// -----------------------------------------------------------
Plugin_SendImages::Plugin_SendImages(QObject *parent, const char*, const QStringList&)
                 : KIPI::Plugin( Factory::instance(), parent, "SendImages")
{
    kdDebug( 51001 ) << "Plugin_SendImages plugin loaded" << endl;

}

void Plugin_SendImages::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );

    m_action_sendimages = new KAction (i18n("Email Images..."),    // Menu message.
                                       "mail_new",                 // Menu icon.
                                       0,
                                       this,
                                       SLOT(slotActivate()),
                                       actionCollection(),
                                       "send_images");

    addAction( m_action_sendimages );

    KIPI::Interface* interface = dynamic_cast< KIPI::Interface* >( parent() );

    if ( !interface )
    {
        kdError( 51000 ) << "Kipi interface is null!" << endl;
        return;
    }

    KIPI::ImageCollection selection = interface->currentSelection();
    m_action_sendimages->setEnabled( selection.isValid() &&
                                   !selection.images().isEmpty() );

    connect( interface, SIGNAL(selectionChanged(bool)),
             m_action_sendimages, SLOT(setEnabled(bool)));
}

Plugin_SendImages::~Plugin_SendImages()
{
}

void Plugin_SendImages::slotActivate()
{
    m_progressDlg = 0;

    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>( parent() );

    if ( !interface )
    {
       kdError( 51000 ) << "Kipi interface is null!" << endl;
       return;
    }

    KIPI::ImageCollection images = interface->currentSelection();

    if ( !images.isValid() || images.images().isEmpty() )
        return;

    KStandardDirs dir;
    QString Tmp = dir.saveLocation("tmp", "kipi-sendimagesplugin-" + QString::number(getpid()) + "/");

    m_sendImagesOperation = new KIPISendimagesPlugin::SendImages( interface, Tmp, images, this );

    m_sendImagesOperation->showDialog();
}

void Plugin_SendImages::slotAcceptedConfigDlg()
{
    m_sendImagesOperation->prepare();
    m_sendImagesOperation->start();
}

void Plugin_SendImages::slotCancel()
{
    m_sendImagesOperation->terminate();
    m_sendImagesOperation->wait();
    m_sendImagesOperation->removeTmpFiles();
}

void Plugin_SendImages::customEvent(QCustomEvent *event)
{
    if (!event) return;

    if (!m_progressDlg)
    {
        m_progressDlg = new KIPI::BatchProgressDialog(kapp->activeWindow(), i18n("Preparing images to send"));

        connect(m_progressDlg, SIGNAL(cancelClicked()),
                this, SLOT(slotCancel()));

        m_current = 0;
        m_progressDlg->show();
    }

    KIPISendimagesPlugin::EventData *d = (KIPISendimagesPlugin::EventData*) event->data();

    if (!d) return;

    if (d->starting)
    {
        QString text;

        switch (d->action)
        {
            case(KIPISendimagesPlugin::Initialize):
            {
                m_total = d->total;
                text = i18n("Preparing 1 image to send....", "Preparing %n images to send....", d->total);
                break;
            }

            case(KIPISendimagesPlugin::ResizeImages):
            {
                text = i18n("Resizing '%1' from Album '%2'...")
                            .arg(d->fileName).arg(d->albumName);
                break;
            }

            case(KIPISendimagesPlugin::Progress):
            {
                text = i18n("Using '%1' from Album '%2' without resizing...")
                            .arg(d->fileName).arg(d->albumName);
                break;
            }

            default:
            {
                kdWarning( 51000 ) << "Plugin_SendImages: Unknown 'Starting' event: " << d->action << endl;
            }
        }

        m_progressDlg->addedAction(text, KIPI::StartingMessage);
    }
    else
    {
        QString text;

        if (!d->success)
        {
            switch (d->action)
            {
                case(KIPISendimagesPlugin::ResizeImages):
                {
                    text = i18n("Failed to resize '%1' from Album '%2'")
                                .arg(d->fileName).arg(d->albumName);
                    break;
                }

                default:
                {
                    kdWarning( 51000 ) << "Plugin_SendImages: Unknown 'Failed' event: " << d->action << endl;
                }
            }

            m_progressDlg->addedAction(text, KIPI::WarningMessage);
        }
        else
        {
            switch (d->action)
            {
                case(KIPISendimagesPlugin::ResizeImages):
                {
                    text = i18n("Resizing '%1' from Album '%2' completed.")
                                .arg(d->fileName).arg(d->albumName);
                    break;
                }

                case(KIPISendimagesPlugin::Progress):
                {
                    text = i18n("All preparatory operations completed.");
                    break;
                }
    
                default:
                {
                    kdWarning( 51000 ) << "Plugin_CDArchiving: Unknown 'Success' event: " << d->action << endl;
                }
            }

            m_progressDlg->addedAction(text, KIPI::SuccessMessage);
        }

        ++m_current;
        m_progressDlg->setProgress(m_current, m_total);

        if( d->action == KIPISendimagesPlugin::Progress )
        {
            // If we have some errors during the resizing images process, show an error dialog.
    
            if ( m_sendImagesOperation->showErrors() == false )
            {
                delete m_progressDlg;
                return;
            }

#if KDE_VERSION >= 0x30200
            m_progressDlg->setButtonCancel( KStdGuiItem::close() );
#else
            m_progressDlg->setButtonCancelText( i18n("&Close") );
#endif

            disconnect(m_progressDlg, SIGNAL(cancelClicked()),
                        this, SLOT(slotCancel()));
    
            // Create a text file with images comments if necessary.
    
            m_sendImagesOperation->makeCommentsFile();
            m_progressDlg->addedAction(i18n("Creating comments file if necessary..."),
                                        KIPI::StartingMessage);
    
            // Invoke mailer agent call.
            int type = m_sendImagesOperation->invokeMailAgent() ? KIPI::SuccessMessage : KIPI::ErrorMessage;
            m_progressDlg->addedAction(i18n("Starting mailer agent..."), type);
    
            m_progressDlg->setProgress(m_total, m_total); 
        }
    }

    kapp->processEvents();
    delete d;
}

KIPI::Category Plugin_SendImages::category( KAction* action ) const
{
    if ( action == m_action_sendimages )
       return KIPI::IMAGESPLUGIN;

    kdWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::IMAGESPLUGIN; // no warning from compiler, please
}

