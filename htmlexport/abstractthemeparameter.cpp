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
#include "abstractthemeparameter.h"

// Qt
#include <QByteArray>
#include <QString>

// KDE
#include <kconfiggroup.h>

static const char* NAME_KEY = "Name";
static const char* DEFAULT_VALUE_KEY = "Default";

namespace KIPIHTMLExport {

class AbstractThemeParameter::Private {
public:
	QByteArray mInternalName;
	QString mName;
	QString mDefaultValue;
};

AbstractThemeParameter::AbstractThemeParameter() {
	d = new Private;
}


AbstractThemeParameter::~AbstractThemeParameter() {
	delete d;
}


void AbstractThemeParameter::init(const QByteArray& internalName, const KConfigGroup* group) {
	d->mInternalName = internalName;
	d->mName = group->readEntry(NAME_KEY);
	d->mDefaultValue = group->readEntry(DEFAULT_VALUE_KEY);
}


QByteArray AbstractThemeParameter::internalName() const {
	return d->mInternalName;
}


QString AbstractThemeParameter::name() const {
	return d->mName;
}


QString AbstractThemeParameter::defaultValue() const {
	return d->mDefaultValue;
}


} // namespace
