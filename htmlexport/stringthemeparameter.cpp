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
#include "stringthemeparameter.h"

// Qt
#include <qlineedit.h>

// KDE
#include <kconfigbase.h>

namespace KIPIHTMLExport {

struct StringThemeParameter::Private {
};

StringThemeParameter::StringThemeParameter() {
	d = new Private;
}

StringThemeParameter::~StringThemeParameter() {
	delete d;
}

void StringThemeParameter::init(const QCString& internalName, const KConfigBase* configFile) {
	AbstractThemeParameter::init(internalName, configFile);
}

QWidget* StringThemeParameter::createWidget(QWidget* parent, const QString& value) const {
	QLineEdit* edit = new QLineEdit(parent);
	edit->setText(value);

	return edit;
}

QString StringThemeParameter::valueFromWidget(QWidget* widget) const {
	Q_ASSERT(widget);
	QLineEdit* edit = static_cast<QLineEdit*>(widget);
	return edit->text();
}

} // namespace
