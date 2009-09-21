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
#ifndef PLUGIN_H
#define PLUGIN_H

#include <QVariant>

#include <libkipi/plugin.h>

class KAction;

namespace KIPIHTMLExport {


/**
 * Implementation of the KIPI::Plugin abstract class for HTMLExport.
 */
class Plugin : public KIPI::Plugin {
	Q_OBJECT

public:
	Plugin(QObject *parent, const QVariantList &args);
	virtual ~Plugin();

	KIPI::Category category( KAction* action ) const;
	virtual void setup( QWidget* widget );

private Q_SLOTS:
	void slotActivate();

private:
	struct Private;
	Private* d;
};

} // namespace


#endif // PLUGIN_H
