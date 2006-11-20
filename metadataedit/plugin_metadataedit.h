/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 * Date   : 2006-10-11
 * Description : a plugin to edit pictures metadata
 *
 * Copyright 2006 by Gilles Caulier
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

#ifndef PLUGIN_METADATAEDIT_H
#define PLUGIN_METADATAEDIT_H

// LibKIPI includes.

#include <libkipi/plugin.h>

class KActionMenu;

class Plugin_MetadataEdit : public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_MetadataEdit(QObject *parent, const char* name, const QStringList &args);

    virtual KIPI::Category category( KAction* action ) const;
    virtual void setup( QWidget* );

protected slots:

    void slotEditExif();
    void slotRemoveExif();
    void slotImportExif();

    void slotEditIptc();
    void slotRemoveIptc();
    void slotImportIptc();

    void slotEditComments();
    void slotRemoveComments();

private:

    KActionMenu     *m_actionMetadataEdit;

    KIPI::Interface *m_interface;
};

#endif // PLUGIN_METADATAEDIT_H
