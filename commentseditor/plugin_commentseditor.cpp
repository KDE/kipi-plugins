/* ============================================================
 * File  : plugin_commentseditor.cpp
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-09-26
 * Description :
 *
 * Copyright 2003 by Renchi Raju

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

#include <klocale.h>
#include <kaction.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kmessagebox.h>

#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>

#include "plugin_commentseditor.h"
#include "commentseditor.h"

K_EXPORT_COMPONENT_FACTORY( kipiplugin_commentseditor,
                            KGenericFactory<Plugin_CommentsEditor>("kipiplugin_commentseditor"));

Plugin_CommentsEditor::Plugin_CommentsEditor(QObject *parent,
                                             const char*,
                                             const QStringList &)
    : KIPI::Plugin(parent, "CommentsEditor")
{
    KGlobal::locale()->insertCatalogue("kipiplugin_commentseditor");

    kdDebug( 51001 ) << "Plugin_CommentsEditor plugin loaded"
              << endl;

    action = new KAction (i18n("Comments Editor..."),
                          "imagecomment",
                          0,
                          this,
                          SLOT(slotActivate()),
                          actionCollection(),
                          "commentseditor");

#ifdef TEMPORARILY_REMOVED
    action->setEnabled(false);
    connect(Digikam::AlbumManager::instance(),
            SIGNAL(signalAlbumCurrentChanged(Digikam::AlbumInfo*)),
            SLOT(slotAlbumChanged(Digikam::AlbumInfo*)));
#endif
}

Plugin_CommentsEditor::~Plugin_CommentsEditor()
{

}

void Plugin_CommentsEditor::slotActivate()
{
    KIPI::Interface* interface = dynamic_cast< KIPI::Interface* >( parent() );
    KIPI::ImageCollection images = interface->currentSelection();
    if ( images.images().count() == 0 )
        images = interface->currentAlbum();

    if ( images.images().count() == 0 ) {
        KMessageBox::sorry(0, i18n("Please select an album or a selection of images for editing comments !"));
        return;
    }

    KURL::List imgs = images.images();
    for( KURL::List::Iterator it = imgs.begin(); it != imgs.end(); ++it ) {
        qDebug(">%s", (*it).path().latin1() );
    }

    CommentsPlugin::CommentsEditor* editor =
        new CommentsPlugin::CommentsEditor( interface, images );
    editor->show();

}

void Plugin_CommentsEditor::slotAlbumChanged(Digikam::AlbumInfo* album)
{
    if (!album)
        action->setEnabled(false);
    else
        action->setEnabled(true);
}

KIPI::Category Plugin_CommentsEditor::category() const
{
    return KIPI::TOOLSPLUGIN;
}
