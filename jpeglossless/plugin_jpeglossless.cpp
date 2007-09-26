/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-09-26
 * Description : loss less images transformations plugin.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2004-2007 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// C++ includes.

#include <iostream>

// Qt includes.

#include <qdir.h>

// KDE includes.

#include <klocale.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kurl.h>
#include <kmessagebox.h>
#include <kapplication.h>

// LibKipi includes.

#include <libkipi/batchprogressdialog.h>
#include <libkipi/interface.h>

// Local includes.

#include "actions.h"
#include "actionthread.h"
#include "plugin_jpeglossless.h"
#include "plugin_jpeglossless.moc"

typedef KGenericFactory<Plugin_JPEGLossless> Factory;

K_EXPORT_COMPONENT_FACTORY( kipiplugin_jpeglossless,
                            Factory("kipiplugin_jpeglossless"))

Plugin_JPEGLossless::Plugin_JPEGLossless(QObject *parent, const char*, const QStringList &)
                   : KIPI::Plugin( Factory::instance(), parent, "JPEGLossless")
{
    m_total                    = 0;
    m_current                  = 0;
    m_action_Convert2GrayScale = 0;
    m_action_AutoExif          = 0;
    m_action_RotateImage       = 0;
    m_action_FlipImage         = 0;
    m_progressDlg              = 0;
    m_thread                   = 0;
    m_failed                   = false;

    kdDebug( 51001 ) << "Plugin_JPEGLossless plugin loaded" << endl;
}

void Plugin_JPEGLossless::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );

    m_action_AutoExif = new KAction(i18n("Auto Rotate/Flip Using Exif Information"),
                                  0, 0,
                                  this,
                                  SLOT(slotRotate()),
                                  actionCollection(),
                                  "rotate_exif");    

    m_action_RotateImage = new KActionMenu(i18n("Rotate"),
                                  "rotate_cw",
                                  actionCollection(),
                                  "jpeglossless_rotate");

    m_action_RotateImage->insert( new KAction(i18n("Left"),
                                  "rotate_ccw",
                                  SHIFT+CTRL+Key_Left,
                                  this,
                                  SLOT(slotRotate()),
                                  actionCollection(),
                                  "rotate_ccw") );
    m_action_RotateImage->insert( new KAction(i18n("Right"),
                                  "rotate_cw",
                                  SHIFT+CTRL+Key_Right,
                                  this,
                                  SLOT(slotRotate()),
                                  actionCollection(),
                                  "rotate_cw") );

    m_action_FlipImage = new KActionMenu(i18n("Flip"),
                                  "flip",
                                  actionCollection(),
                                  "jpeglossless_flip");

    m_action_FlipImage->insert( new KAction(i18n("Horizontally"),
                                  0,
                                  CTRL+Key_Asterisk,
                                  this,
                                  SLOT(slotFlip()),
                                  actionCollection(),
                                  "flip_horizontal") );

    m_action_FlipImage->insert( new KAction(i18n("Vertically"),
                                  0,
                                  CTRL+Key_Slash,
                                  this,
                                  SLOT(slotFlip()),
                                  actionCollection(),
                                  "flip_vertical") );

    m_action_Convert2GrayScale = new KAction(i18n("Convert to Black && White"),
                                  "grayscaleconvert",
                                  0,
                                  this,
                                  SLOT(slotConvert2GrayScale()),
                                  actionCollection(),
                                  "jpeglossless_convert2grayscale");

    addAction( m_action_AutoExif );
    addAction( m_action_RotateImage );
    addAction( m_action_FlipImage );
    addAction( m_action_Convert2GrayScale );

    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>( parent() );

    if ( !interface ) 
    {
        kdError( 51000 ) << "Kipi interface is null!" << endl;
        return;
    }

    m_action_AutoExif->setEnabled( false );
    m_action_RotateImage->setEnabled( false );
    m_action_FlipImage->setEnabled( false );
    m_action_Convert2GrayScale->setEnabled( false );

    m_thread = new KIPIJPEGLossLessPlugin::ActionThread(interface, this);

    connect( interface, SIGNAL( selectionChanged( bool ) ),
             m_action_AutoExif, SLOT( setEnabled( bool ) ) );

    connect( interface, SIGNAL( selectionChanged( bool ) ),
             m_action_RotateImage, SLOT( setEnabled( bool ) ) );

    connect( interface, SIGNAL( selectionChanged( bool ) ),
             m_action_FlipImage, SLOT( setEnabled( bool ) ) );

    connect( interface, SIGNAL( selectionChanged( bool ) ),
             m_action_Convert2GrayScale, SLOT( setEnabled( bool ) ) );
}

Plugin_JPEGLossless::~Plugin_JPEGLossless()
{
    delete m_thread;
    delete m_progressDlg;
}

void Plugin_JPEGLossless::slotFlip()
{
    KURL::List items = images();
    if (items.count() <= 0) return;

    QString from(sender()->name());
    QString title;
    bool proceed = false;

    if (from == "flip_horizontal") 
    {
        m_thread->flip(items, KIPIJPEGLossLessPlugin::FlipHorizontal);
        title = i18n("horizontaly");
        proceed = true;
    }
    else if (from == "flip_vertical") 
    {
        m_thread->flip(items, KIPIJPEGLossLessPlugin::FlipVertical);
        title = i18n("vertically");
        proceed = true;
    }
    else 
    {
        kdWarning( 51000 ) << "The impossible happened... unknown flip specified" << endl;
        return;
    }

    if (!proceed) return;
    m_total   = items.count();
    m_current = 0;
    m_failed  = false;

    if (m_progressDlg) 
    {
        delete m_progressDlg;
        m_progressDlg = 0;
    }

    m_progressDlg = new KIPI::BatchProgressDialog(kapp->activeWindow(), 
                        i18n("Flip images %1").arg(title));

    connect(m_progressDlg, SIGNAL(cancelClicked()),
            this, SLOT(slotCancel()));

    m_progressDlg->show();

    if (!m_thread->running())
        m_thread->start();
}

void Plugin_JPEGLossless::slotRotate()
{
    KURL::List items = images();
    if (items.count() <= 0) return;

    QString from(sender()->name());
    QString title;
    bool proceed = false;

    if (from == "rotate_cw") 
    {
        m_thread->rotate(items, KIPIJPEGLossLessPlugin::Rot90);
        title = i18n("right (clockwise)");
        proceed = true;
    }
    else if (from == "rotate_ccw") 
    {
        m_thread->rotate(items, KIPIJPEGLossLessPlugin::Rot270);
        title = i18n("left (counterclockwise)");
        proceed = true;
    }
    else if (from == "rotate_exif") 
    {
        m_thread->rotate(items, KIPIJPEGLossLessPlugin::Rot0);
        title = i18n("using Exif orientation tag");
        proceed = true;
    }
    else 
    {
        kdWarning( 51000 ) << "The impossible happened... unknown rotation angle specified" << endl;
        return;
    }

    if (!proceed) return;
    m_total   = items.count();
    m_current = 0;
    m_failed  = false;

    if (m_progressDlg) 
    {
        delete m_progressDlg;
        m_progressDlg = 0;
    }

    m_progressDlg = new KIPI::BatchProgressDialog(kapp->activeWindow(), 
                        i18n("Rotate images %1").arg(title));

    connect(m_progressDlg, SIGNAL(cancelClicked()),
            this, SLOT(slotCancel()));

    m_progressDlg->show();

    if (!m_thread->running())
        m_thread->start();
}

void Plugin_JPEGLossless::slotConvert2GrayScale()
{
    KURL::List items = images();
    if (items.count() <= 0 ||
        KMessageBox::No==KMessageBox::warningYesNo(kapp->activeWindow(),
                     i18n("<p>Are you sure you wish to convert the selected image(s) to "
                         "black and white? This operation <b>cannot</b> be undone.</p>")))
        return;

    QString from(sender()->name());

    m_total   = items.count();
    m_current = 0;
    m_failed  = false;

    if (m_progressDlg) 
    {
        delete m_progressDlg;
        m_progressDlg = 0;
    }

    m_progressDlg = new KIPI::BatchProgressDialog(kapp->activeWindow(), 
                        i18n("Convert images to black & white"));

    connect(m_progressDlg, SIGNAL(cancelClicked()),
            this, SLOT(slotCancel()));

    m_progressDlg->show();

    m_thread->convert2grayscale(items);
    if (!m_thread->running())
        m_thread->start();
}

void Plugin_JPEGLossless::slotCancel()
{
    m_thread->cancel();

    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>( parent() );

    if ( !interface ) 
    {
        kdError( 51000 ) << "Kipi interface is null!" << endl;
        return;
    }

    interface->refreshImages( m_images );
}

void Plugin_JPEGLossless::customEvent(QCustomEvent *event)
{
    if (!event) return;

    KIPIJPEGLossLessPlugin::EventData *d = (KIPIJPEGLossLessPlugin::EventData*) event->data();
    if (!d) return;

    QString text;

    if (d->starting) 
    {
        switch (d->action) 
        {
            case(KIPIJPEGLossLessPlugin::Rotate): 
            {
                text = i18n("Rotating Image \"%1\"").arg(d->fileName.section('/', -1));
                break;
            }
            case(KIPIJPEGLossLessPlugin::Flip): 
            {
                text = i18n("Flipping Image \"%1\"").arg(d->fileName.section('/', -1));
                break;
            }
            case(KIPIJPEGLossLessPlugin::GrayScale): 
            {
                text = i18n("Converting to Black & White \"%1\"").arg(d->fileName.section('/', -1));
                break;
            }
            default:
            {
                kdWarning( 51000 ) << "KIPIJPEGLossLessPlugin: Unknown event" << endl;
            }
        }

        m_progressDlg->addedAction(text, KIPI::StartingMessage);
    }
    else
    {
        if (!d->success) 
        {
            m_failed = true;

            switch (d->action) 
            {
                case(KIPIJPEGLossLessPlugin::Rotate): 
                {
                    text = i18n("Failed to Rotate image");
                    break;
                }
                case(KIPIJPEGLossLessPlugin::Flip): 
                {
                    text = i18n("Failed to Flip image");
                    break;
                }
                case(KIPIJPEGLossLessPlugin::GrayScale): 
                {
                    text = i18n("Failed to convert image to Black & White");
                    break;
                }
                default: 
                {
                    kdWarning( 51000 ) << "KIPIJPEGLossLessPlugin: Unknown event" << endl;
                }
            }

            m_progressDlg->addedAction(text, KIPI::WarningMessage);

            if (!d->errString.isEmpty())
                m_progressDlg->addedAction(d->errString, KIPI::WarningMessage);
        }
        else
        {
            switch (d->action)
            {
                case(KIPIJPEGLossLessPlugin::Rotate): 
                {
                    text = i18n("Rotate image complete");
                    break;
                }
                case(KIPIJPEGLossLessPlugin::Flip): 
                {
                    text = i18n("Flip image complete");
                    break;
                }
                case(KIPIJPEGLossLessPlugin::GrayScale): 
                {
                    text = i18n("Convert to Black & White complete");
                    break;
                }
                default: 
                {
                    kdWarning( 51000 ) << "KIPIJPEGLossLessPlugin: Unknown event" << endl;
                }
            }

        m_progressDlg->addedAction(text, KIPI::SuccessMessage);
        }

        m_current++;
        m_progressDlg->setProgress(m_current, m_total);
    }

    delete d;

    if (m_current >= m_total) 
    {
        m_current = 0;

        if (m_failed)
        {
#if KDE_VERSION >= 0x30200
            m_progressDlg->setButtonCancel( KStdGuiItem::close() );
#else
            m_progressDlg->setButtonCancelText( i18n("&Close") );
#endif

            disconnect(m_progressDlg, SIGNAL(cancelClicked()),
                    this, SLOT(slotCancel()));
        }
        else 
        {
            slotCancel();
            m_progressDlg->close();
            m_progressDlg = 0;
        }

        KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>( parent() );

        if ( !interface ) 
        {
           kdError( 51000 ) << "Kipi interface is null!" << endl;
           return;
        }

        interface->refreshImages( m_images );
    }
}

KIPI::Category Plugin_JPEGLossless::category( KAction* action ) const
{
    if (action == m_action_AutoExif)
        return KIPI::IMAGESPLUGIN;
    else if ( action == m_action_RotateImage )
       return KIPI::IMAGESPLUGIN;
    else if ( action == m_action_FlipImage )
       return KIPI::IMAGESPLUGIN;
    else if ( action == m_action_Convert2GrayScale )
       return KIPI::IMAGESPLUGIN;

    kdWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::IMAGESPLUGIN; // no warning from compiler, please
}

KURL::List Plugin_JPEGLossless::images()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>( parent() );

    if ( !interface ) 
    {
        kdError( 51000 ) << "Kipi interface is null!" << endl;
        return KURL::List();
    }

    KIPI::ImageCollection images = interface->currentSelection();
    if ( !images.isValid() )
        return KURL::List();

    // We don't want the set of images to change before we are done
    // and tells the host app to refresh the images.
    m_images = images.images();
    return images.images();
}
