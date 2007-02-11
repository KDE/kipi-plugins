/***************************************************************************
 *   Copyright (C) 2007 by Markus Leuthold   *
 *   kusi (+at) forum.titlis.org   *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef PLUGIN_EXAMPLE_H
#define PLUGIN_EXAMPLE_H

#include <libkipi/plugin.h>
#include "ogl.h"
class KAction;

/**
 * @short integration with KIPI
 * @author Markus Leuthold <kusi (+at) forum.titlis.org>
 * @version 0.1
 */

class Plugin_viewer :public KIPI::Plugin
{
    Q_OBJECT
public:
    Plugin_viewer( QObject *parent, const char* name, const QStringList&  );
    virtual void setup( QWidget* widget );
    virtual KIPI::Category category( KAction* action ) const;

protected:
	ogl * widget;
    KAction * actionViewer;
	
public slots:
    void  slotActivate();
};

#endif /* PLUGIN_EXAMPLE_H */

