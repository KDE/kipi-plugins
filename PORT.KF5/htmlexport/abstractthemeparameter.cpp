/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2006-04-04
 * Description : A KIPI plugin to generate HTML image galleries
 *
 * Copyright (C) 2006-2010 by Aurelien Gateau <aurelien dot gateau at free.fr>
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

// vim: set tabstop=4 shiftwidth=4 noexpandtab:

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
