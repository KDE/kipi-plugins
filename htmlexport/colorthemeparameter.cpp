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
// Self
#include "colorthemeparameter.h"

// Qt

// KDE
#include <kcolorbutton.h>

// Local

namespace KIPIHTMLExport {


QWidget* ColorThemeParameter::createWidget(QWidget* parent, const QString& value) const {
	KColorButton* button = new KColorButton(parent);
	QColor color(value);
	button->setColor(color);
	return button;
}


QString ColorThemeParameter::valueFromWidget(QWidget* widget) const {
	KColorButton* button = static_cast<KColorButton*>(widget);
	return button->color().name();
}


} // namespace
