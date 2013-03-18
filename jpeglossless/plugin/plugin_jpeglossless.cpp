/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2003-09-26
 * Description : loss less images transformations plugin.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2004-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2006-2013 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "plugin_jpeglossless.moc"

// C++ includes

#include <iostream>

// KDE includes

#include <kaction.h>
#include <kactioncollection.h>
#include <kactionmenu.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kmenu.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kshortcut.h>
#include <kstandardguiitem.h>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/pluginloader.h>

// Local includes

#include "kpbatchprogressdialog.h"
#include "actionthread.h"

using namespace KIPIPlugins;

namespace KIPIJPEGLossLessPlugin
{

K_PLUGIN_FACTORY( JPEGLosslessFactory, registerPlugin<Plugin_JPEGLossless>(); )
K_EXPORT_PLUGIN ( JPEGLosslessFactory("kipiplugin_jpeglossless") )

class Plugin_JPEGLossless::Private
{
public:

    Private()
    {
        total                    = 0;
        current                  = 0;
        action_Convert2GrayScale = 0;
        action_AutoExif          = 0;
        action_RotateImage       = 0;
        action_FlipImage         = 0;
        progressDlg              = 0;
        thread                   = 0;
        failed                   = false;
    }

    bool                   failed;

    int                    total;
    int                    current;

    KAction*               action_Convert2GrayScale;
    KAction*               action_AutoExif;
    KAction*               action_RotateImage;
    KAction*               action_FlipImage;

    KUrl::List             images;

    KPBatchProgressDialog* progressDlg;

    ActionThread*          thread;

    QWidget*               parentWidget;
};

Plugin_JPEGLossless::Plugin_JPEGLossless(QObject* const parent, const QVariantList&)
    : Plugin(JPEGLosslessFactory::componentData(), parent, "JPEGLossless"),
      d(new Private)
{
    kDebug(AREA_CODE_LOADING) << "Plugin_JPEGLossless plugin loaded";

    setUiBaseName("kipiplugin_jpeglosslessui.rc");
    setupXML();
}

Plugin_JPEGLossless::~Plugin_JPEGLossless()
{
    delete d->progressDlg;
    delete d;
}

void Plugin_JPEGLossless::setup(QWidget* const widget)
{
    d->parentWidget = widget;

    Plugin::setup( widget );
    setupActions();

    Interface* const iface = interface();

    if (!iface)
    {
        kError() << "Kipi interface is null!";
        return;
    }

    d->thread = new ActionThread(this);

    connect( d->thread, SIGNAL(starting(KUrl,int)),
             this, SLOT(slotStarting(KUrl,int)));

    connect( d->thread, SIGNAL(finished(KUrl,int)),
             this, SLOT(slotFinished(KUrl,int)));

    connect( d->thread, SIGNAL(failed(KUrl,int,QString)),
             this, SLOT(slotFailed(KUrl,int,QString)));

    bool hasSelection = iface->currentSelection().isValid();

    d->action_AutoExif->setEnabled( hasSelection );

    connect( iface, SIGNAL(selectionChanged(bool)),
             d->action_AutoExif, SLOT(setEnabled(bool)) );

    d->action_Convert2GrayScale->setEnabled( hasSelection );

    connect( iface, SIGNAL(selectionChanged(bool)),
             d->action_Convert2GrayScale, SLOT(setEnabled(bool)) );

    // We need to check these two actions
    if (d->action_RotateImage)
    {
        d->action_RotateImage->setEnabled( hasSelection );

        connect( iface, SIGNAL(selectionChanged(bool)),
                 d->action_RotateImage, SLOT(setEnabled(bool)) );
    }

    if (d->action_FlipImage)
    {
        d->action_FlipImage->setEnabled( hasSelection );

        connect( iface, SIGNAL(selectionChanged(bool)),
                 d->action_FlipImage, SLOT(setEnabled(bool)) );
    }
}

void Plugin_JPEGLossless::setupActions()
{
    setDefaultCategory(ImagesPlugin);

    QStringList disabledActions = PluginLoader::instance()->disabledPluginActions();

    if (!disabledActions.contains("jpeglossless_rotate"))
    {
        d->action_RotateImage = actionCollection()->addAction("jpeglossless_rotate");
        d->action_RotateImage->setText(i18n("Rotate"));
        d->action_RotateImage->setIcon(KIcon("object-rotate-right"));

        KMenu* const rotateMenu = new KMenu(d->parentWidget);
        d->action_RotateImage->setMenu(rotateMenu);

        KAction* const left = new KAction(this);
        left->setText(i18nc("rotate image left", "Rotate Left"));
        left->setShortcut(KShortcut(Qt::SHIFT+Qt::CTRL+Qt::Key_Left));
        rotateMenu->addAction(left);

        connect(left, SIGNAL(triggered(bool)),
                this, SLOT(slotRotateLeft()));

        addAction("rotate_ccw", left);

        KAction* const right = new KAction(this);
        right->setText(i18nc("rotate image right", "Rotate Right"));
        right->setShortcut(KShortcut(Qt::SHIFT+Qt::CTRL+Qt::Key_Right));
        rotateMenu->addAction(right);

        connect(right, SIGNAL(triggered(bool)),
                this, SLOT(slotRotateRight()));

        addAction("rotate_cw", right);
    }

    // -----------------------------------------------------------------------------------

    if (!disabledActions.contains("jpeglossless_flip"))
    {
        d->action_FlipImage = actionCollection()->addAction("jpeglossless_flip");
        d->action_FlipImage->setText(i18n("Flip"));
        d->action_FlipImage->setIcon(KIcon("flip-horizontal"));

        KMenu* const flipMenu = new KMenu(d->parentWidget);
        d->action_FlipImage->setMenu(flipMenu);

        KAction* const hori = new KAction(this);
        hori->setText(i18n("Flip Horizontally"));
        hori->setShortcut(KShortcut(Qt::CTRL+Qt::Key_Asterisk));
        flipMenu->addAction(hori);

        connect(hori, SIGNAL(triggered(bool)),
                this, SLOT(slotFlipHorizontally()));

        addAction("flip_horizontal", hori);

        KAction* const verti = new KAction(this);
        verti->setText(i18n("Flip Vertically"));
        verti->setShortcut(KShortcut(Qt::CTRL+Qt::Key_Slash));
        flipMenu->addAction(verti);

        connect(verti, SIGNAL(triggered(bool)),
                this, SLOT(slotFlipVertically()));

        addAction("flip_vertical", verti);
    }

    // -----------------------------------------------------------------------------------

    d->action_AutoExif = new KAction(this);
    d->action_AutoExif->setText(i18n("Auto Rotate/Flip Using Exif Information"));

    connect(d->action_AutoExif, SIGNAL(triggered(bool)),
            this, SLOT(slotRotateExif()));

    addAction("rotate_exif", d->action_AutoExif);

    // -----------------------------------------------------------------------------------

    d->action_Convert2GrayScale = new KAction(this);
    d->action_Convert2GrayScale->setText(i18n("Convert to Black && White"));
    d->action_Convert2GrayScale->setIcon(KIcon("grayscaleconvert"));

    connect(d->action_Convert2GrayScale, SIGNAL(triggered(bool)),
            this, SLOT(slotConvert2GrayScale()));

    addAction("jpeglossless_convert2grayscale", d->action_Convert2GrayScale);
}

void Plugin_JPEGLossless::slotFlipHorizontally()
{
    flip(FlipHorizontal, i18n("horizontally"));
}

void Plugin_JPEGLossless::slotFlipVertically()
{
    flip(FlipVertical, i18n("vertically"));
}

void Plugin_JPEGLossless::flip(FlipAction action, const QString& title)
{
    KUrl::List items = images();

    if (items.count() <= 0)
        return;

    d->thread->flip(items, action);

    d->total   = items.count();
    d->current = 0;
    d->failed  = false;

    delete d->progressDlg;
    d->progressDlg = 0;

    d->progressDlg = new KPBatchProgressDialog(kapp->activeWindow(),
                         i18n("Flip images %1", title));

    connect(d->progressDlg, SIGNAL(cancelClicked()),
            this, SLOT(slotCancel()));

    d->progressDlg->show();

    if (!d->thread->isRunning())
        d->thread->start();
}

void Plugin_JPEGLossless::slotRotateRight()
{
    rotate(Rot90, i18n("right (clockwise)"));
}

void Plugin_JPEGLossless::slotRotateLeft()
{
    rotate(Rot270, i18n("left (counterclockwise)"));
}

void Plugin_JPEGLossless::slotRotateExif()
{
    rotate(Rot0, i18n("using Exif orientation tag"));
}

void Plugin_JPEGLossless::rotate(RotateAction action, const QString& title)
{
    KUrl::List items = images();

    if (items.count() <= 0)
        return;

    d->thread->rotate(items, action);

    d->total   = items.count();
    d->current = 0;
    d->failed  = false;

    delete d->progressDlg;
    d->progressDlg = 0;
    d->progressDlg = new KPBatchProgressDialog(kapp->activeWindow(),
                         i18n("Rotate images %1", title));

    connect(d->progressDlg, SIGNAL(cancelClicked()),
            this, SLOT(slotCancel()));

    d->progressDlg->show();

    if (!d->thread->isRunning())
        d->thread->start();
}

void Plugin_JPEGLossless::slotConvert2GrayScale()
{
    KUrl::List items = images();

    if (items.count() <= 0 ||
        KMessageBox::No == KMessageBox::warningYesNo(kapp->activeWindow(),
                     i18n("<p>Are you sure you wish to convert the selected image(s) to "
                         "black and white? This operation <b>cannot</b> be undone.</p>")))
        return;

    QString from(sender()->objectName());

    d->total   = items.count();
    d->current = 0;
    d->failed  = false;

    delete d->progressDlg;
    d->progressDlg = 0;

    d->progressDlg = new KPBatchProgressDialog(kapp->activeWindow(),
                         i18n("Convert images to black & white"));

    connect(d->progressDlg, SIGNAL(cancelClicked()),
            this, SLOT(slotCancel()));

    d->progressDlg->show();

    d->thread->convert2grayscale(items);
    if (!d->thread->isRunning())
        d->thread->start();
}

void Plugin_JPEGLossless::slotCancel()
{
    d->thread->cancel();

    Interface* const interface = dynamic_cast<Interface*>( parent() );

    if ( !interface )
    {
        kError() << "Kipi interface is null!";
        return;
    }

    interface->refreshImages( d->images );
}

void Plugin_JPEGLossless::slotStarting(const KUrl& url, int action)
{
    QString text;
    QString filePath = url.toLocalFile();

    switch ((Action)action)
    {
        case(Rotate):
        {
            text = i18n("Rotating Image \"%1\"", filePath.section('/', -1));
            break;
        }
        case(Flip):
        {
            text = i18n("Flipping Image \"%1\"", filePath.section('/', -1));
            break;
        }
        case(GrayScale):
        {
            text = i18n("Converting to Black & White \"%1\"", filePath.section('/', -1));
            break;
        }
        default:
        {
            kWarning() << "KIPIJPEGLossLessPlugin: Unknown event";
            break;
        }
    }

    d->progressDlg->progressWidget()->addedAction(text, StartingMessage);
}

void Plugin_JPEGLossless::slotFinished(const KUrl& url, int action)
{
    Q_UNUSED(url);

    QString text;

    switch ((Action)action)
    {
        case(Rotate):
        {
            text = i18n("Rotate image complete");
            break;
        }
        case(Flip):
        {
            text = i18n("Flip image complete");
            break;
        }
        case(GrayScale):
        {
            text = i18n("Convert to Black & White complete");
            break;
        }
        default:
        {
            kWarning() << "KIPIJPEGLossLessPlugin: Unknown event";
            break;
        }
    }

    d->progressDlg->progressWidget()->addedAction(text, SuccessMessage);

    oneTaskCompleted();
}

void Plugin_JPEGLossless::slotFailed(const KUrl& url, int action, const QString& errString)
{
    Q_UNUSED(url);

    d->failed = true;
    QString text;

    switch ((Action)action)
    {
        case(Rotate):
        {
            text = i18n("Failed to Rotate image");
            break;
        }
        case(Flip):
        {
            text = i18n("Failed to Flip image");
            break;
        }
        case(GrayScale):
        {
            text = i18n("Failed to convert image to Black & White");
            break;
        }
        default:
        {
            kWarning() << "KIPIJPEGLossLessPlugin: Unknown event";
            break;
        }
    }

    d->progressDlg->progressWidget()->addedAction(text, WarningMessage);

    if (!errString.isEmpty())
        d->progressDlg->progressWidget()->addedAction(errString, WarningMessage);

    oneTaskCompleted();
}

void Plugin_JPEGLossless::oneTaskCompleted()
{
    d->current++;
    d->progressDlg->progressWidget()->setProgress(d->current, d->total);

    if (d->current >= d->total)
    {
        d->current = 0;

        if (d->failed)
        {
            d->progressDlg->setButtonGuiItem(KDialog::Cancel, KStandardGuiItem::close());

            disconnect(d->progressDlg, SIGNAL(cancelClicked()),
                       this, SLOT(slotCancel()));
        }
        else
        {
            slotCancel();
            d->progressDlg->close();
            d->progressDlg = 0;
        }

        Interface* const interface = dynamic_cast<Interface*>( parent() );

        if ( !interface )
        {
           kError() << "Kipi interface is null!";
           return;
        }

        interface->refreshImages( d->images );
    }
}

KUrl::List Plugin_JPEGLossless::images()
{
    Interface* const interface = dynamic_cast<Interface*>( parent() );

    if ( !interface )
    {
        kError() << "Kipi interface is null!";
        return KUrl::List();
    }

    ImageCollection images = interface->currentSelection();

    if ( !images.isValid() )
        return KUrl::List();

    // We don't want the set of images to change before we are done
    // and tells the host app to refresh the images.
    d->images = images.images();
    return images.images();
}

} // namespace KIPIJPEGLossLessPlugin
