/* ============================================================
 * File  : plugin_commentseditor.h
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

#ifndef PLUGIN_COMMENTSEDITOR_H
#define PLUGIN_COMMENTSEDITOR_H

// LibKIPi includes.

#include <libkipi/plugin.h>

class KAction;

namespace Digikam
{
class AlbumInfo;
}

class Plugin_CommentsEditor : public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_CommentsEditor(QObject *parent,
                          const char* name,
                          const QStringList &args);
    ~Plugin_CommentsEditor();
    virtual KIPI::Category category( KAction* action ) const;
    virtual void setup( QWidget* widget );

public slots:

    void slotActivate();
    void slotAlbumChanged(Digikam::AlbumInfo* album);

private:

    KAction *m_actionCommentsEditor;

};

#endif /* PLUGIN_COMMENTSEDITOR_H */
