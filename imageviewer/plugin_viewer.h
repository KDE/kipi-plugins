/***************************************************************************
 *   Copyright (C) 2007 by Markus Leuthold   *
 *   <kusi (+at) forum.titlis.org>   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Cambridge, MA 02110-1301, USA.        *
 ***************************************************************************/
#ifndef PLUGIN_VIEWER_H
#define PLUGIN_VIEWER_H

// kipi includes
#include <libkipi/plugin.h>

// local includes
#include "viewerwidget.h"


class KAction;

/**
 * @short integration with KIPI
 * @author Markus Leuthold <kusi (+at) forum.titlis.org>
 * @version 0.2
 */

class Plugin_viewer :public KIPI::Plugin
{
	Q_OBJECT
public:
	Plugin_viewer( QObject *parent, const QVariantList &  );
	virtual void setup( QWidget* widget );
	virtual KIPI::Category category( KAction* action ) const;

protected:
	KIPIviewer::ViewerWidget * widget;
	KAction * actionViewer;
	
public slots:
	void  slotActivate();
};

#endif /* PLUGIN_VIEWER_H */

