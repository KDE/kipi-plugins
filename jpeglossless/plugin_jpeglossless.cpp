/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-09-26
 * Description : loss less images transformations plugin.
 *
 * Copyright (C) 2003-2005 by Renchi Raju & Gilles Caulier <renchi@pooh.tam.uiuc.edu>
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

#include <kurl.h>

// KDE includes.

#include <kshortcut.h>
#include <klocale.h>
#include <kaction.h>
#include <kactionmenu.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kurl.h>
#include <kmessagebox.h>
#include <kapplication.h>
#include <kstandardguiitem.h>

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

Plugin_JPEGLossless::Plugin_JPEGLossless(QObject *parent, const QStringList &)
                   : KIPI::Plugin( Factory::componentData(), parent, "JPEGLossless")
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

    kDebug( 51001 ) << "Plugin_JPEGLossless plugin loaded" << endl;
}

void Plugin_JPEGLossless::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );

    m_action_AutoExif = new KAction(i18n("Auto Rotate/Flip Using Exif Information"), actionCollection());
    connect(m_action_AutoExif, SIGNAL(triggered(bool)), this, SLOT(slotRotate()));
    addAction(m_action_AutoExif);

    m_action_RotateImage = new KActionMenu(i18n("Rotate"), actionCollection());

    KAction *left = new KAction(i18n("Left"), actionCollection());
    left->setShortcut(Qt::SHIFT+Qt::CTRL+Qt::Key_Left);
    connect(left, SIGNAL(triggered(bool)), this, SLOT(slotRotate()));
    m_action_RotateImage->addAction(left);

    KAction *right = new KAction(i18n("Right"), actionCollection());
    right->setShortcut(Qt::SHIFT+Qt::CTRL+Qt::Key_Right);
    connect(right, SIGNAL(triggered(bool)), this, SLOT(slotRotate()));
    m_action_RotateImage->addAction(right);

    m_action_FlipImage = new KActionMenu(i18n("Flip"), actionCollection());

    KAction *hori = new KAction(i18n("Horizontally"), actionCollection());
    hori->setShortcut(Qt::CTRL+Qt::Key_Asterisk);
    connect(hori, SIGNAL(triggered(bool)), this, SLOT(slotFlip()));
    m_action_FlipImage->addAction(hori);

    KAction *verti = new KAction(i18n("Vertically"), actionCollection());
    verti->setShortcut(Qt::CTRL+Qt::Key_Slash);
    connect(verti, SIGNAL(triggered(bool)), this, SLOT(slotFlip()));
    m_action_FlipImage->addAction(verti);

    m_action_Convert2GrayScale = new KAction(i18n("Convert to Black && White"), actionCollection());
    connect(m_action_Convert2GrayScale, SIGNAL(triggered(bool)), this, SLOT(slotConvert2GrayScale()));
    addAction(m_action_Convert2GrayScale);

    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>( parent() );

    if ( !interface ) 
    {
        kError( 51000 ) << "Kipi interface is null!" << endl;
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
    KUrl::List items = images();
    if (items.count() <= 0) return;

    QString from(sender()->objectName());
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
        kWarning( 51000 ) << "The impossible happened... unknown flip specified" << endl;
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

    if (!m_thread->isRunning())
        m_thread->start();
}

void Plugin_JPEGLossless::slotRotate()
{
    KUrl::List items = images();
    if (items.count() <= 0) return;

    QString from(sender()->objectName());
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
        kWarning( 51000 ) << "The impossible happened... unknown rotation angle specified" << endl;
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

    if (!m_thread->isRunning())
        m_thread->start();
}

void Plugin_JPEGLossless::slotConvert2GrayScale()
{
    KUrl::List items = images();
    if (items.count() <= 0 ||
        KMessageBox::No==KMessageBox::warningYesNo(kapp->activeWindow(),
                     i18n("<p>Are you sure you wish to convert the selected image(s) to "
                         "black and white? This operation <b>cannot</b> be undone.</p>")))
        return;

    QString from(sender()->objectName());

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
    if (!m_thread->isRunning())
        m_thread->start();
}

void Plugin_JPEGLossless::slotCancel()
{
    m_thread->cancel();

    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>( parent() );

    if ( !interface ) 
    {
        kError( 51000 ) << "Kipi interface is null!" << endl;
        return;
    }

    interface->refreshImages( m_images );
}

void Plugin_JPEGLossless::customEvent(QEvent *event)
{
    if (!event) return;

    KIPIJPEGLossLessPlugin::EventData *d = (KIPIJPEGLossLessPlugin::EventData*) event;
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
                kWarning( 51000 ) << "KIPIJPEGLossLessPlugin: Unknown event" << endl;
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
                    kWarning( 51000 ) << "KIPIJPEGLossLessPlugin: Unknown event" << endl;
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
                    kWarning( 51000 ) << "KIPIJPEGLossLessPlugin: Unknown event" << endl;
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
            m_progressDlg->setButtonGuiItem(KDialog::Cancel, KStandardGuiItem::close());

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
           kError( 51000 ) << "Kipi interface is null!" << endl;
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

    kWarning( 51000 ) << "Unrecognized action for plugin category identification" << endl;
    return KIPI::IMAGESPLUGIN; // no warning from compiler, please
}

KUrl::List Plugin_JPEGLossless::images()
{
    KIPI::Interface* interface = dynamic_cast<KIPI::Interface*>( parent() );

    if ( !interface ) 
    {
        kError( 51000 ) << "Kipi interface is null!" << endl;
        return KUrl::List();
    }

    KIPI::ImageCollection images = interface->currentSelection();
    if ( !images.isValid() )
        return KUrl::List();

    // We don't want the set of images to change before we are done
    // and tells the host app to refresh the images.
    m_images = images.images();
    return images.images();
}
