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

#include <digikam/albummanager.h>
#include <digikam/albuminfo.h>

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

    kdDebug() << "Plugin_CommentsEditor plugin loaded"
              << endl;

    action = new KAction (i18n("Comments Editor..."),
                          "imagecomment",
                          0,
                          this,
                          SLOT(slotActivate()),
                          actionCollection(),
                          "commentseditor");
    action->setEnabled(false);

    connect(Digikam::AlbumManager::instance(),
            SIGNAL(signalAlbumCurrentChanged(Digikam::AlbumInfo*)),
            SLOT(slotAlbumChanged(Digikam::AlbumInfo*)));
}

Plugin_CommentsEditor::~Plugin_CommentsEditor()
{

}

void Plugin_CommentsEditor::slotActivate()
{
    Digikam::AlbumInfo *album =
        Digikam::AlbumManager::instance()->currentAlbum();
    if (!album)
        {
        KMessageBox::sorry(0, i18n("Please select an album for editing comments !"));
        return;
        }

    CommentsPlugin::CommentsEditor* editor =
        new CommentsPlugin::CommentsEditor(album);
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
    return KIPI::Tools;
}
