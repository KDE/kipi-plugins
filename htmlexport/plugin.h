// vim: set tabstop=4 shiftwidth=4 noexpandtab:
/*
A KIPI plugin to generate HTML image galleries
Copyright 2006 by Aurelien Gateau <aurelien dot gateau at free.fr>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Cambridge, MA 02110-1301, USA.

*/
#ifndef PLUGINHTMLEXPORT_H
#define PLUGINHTMLEXPORT_H

#include <QVariant>

#include <libkipi/plugin.h>

class KAction;

using namespace KIPI;

/**
 * Implementation of the Plugin abstract class for HTMLExport.
 */
class Plugin_HTMLExport : public Plugin
{
    Q_OBJECT

public:
    
    Plugin_HTMLExport(QObject* const parent, const QVariantList& args);
    virtual ~Plugin_HTMLExport();

    Category category( KAction* action ) const;
    virtual void setup( QWidget* widget );

private Q_SLOTS:

    void slotActivate();

private:

    struct Private;
    Private* d;
};

#endif // PLUGINHTMLEXPORT_H
