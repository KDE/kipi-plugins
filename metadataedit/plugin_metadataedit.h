/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-11
 * Description : a plugin to edit pictures metadata
 *
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef PLUGIN_METADATAEDIT_H
#define PLUGIN_METADATAEDIT_H

// Qt includes

#include <QVariant>

// KDE includes

#include <KUrl>

// LibKIPI includes

#include <libkipi/plugin.h>

namespace KIPI
{
class Interface;
} // namespace KIPI

class KActionMenu;

class Plugin_MetadataEdit : public KIPI::Plugin
{
    Q_OBJECT

public:

    Plugin_MetadataEdit(QObject *parent, const QVariantList &args);

    virtual KIPI::Category category( KAction* action ) const;
    virtual void setup( QWidget* );

protected Q_SLOTS:

    void slotEditExif();
    void slotRemoveExif();
    void slotImportExif();

    void slotEditIptc();
    void slotRemoveIptc();
    void slotImportIptc();

    void slotEditXmp();
    void slotRemoveXmp();
    void slotImportXmp();

    void slotEditComments();
    void slotRemoveComments();

private:

    KActionMenu     *m_actionMetadataEdit;

    KIPI::Interface *m_interface;

    KUrl             m_lastSelectedDirectory;
};

#endif // PLUGIN_METADATAEDIT_H
