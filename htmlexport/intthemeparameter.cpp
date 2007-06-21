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
#include "intthemeparameter.h"

// Qt
#include <qspinbox.h>

// KDE
#include <kconfigbase.h>
#include <kdebug.h>

// Local

static const char* MIN_VALUE_KEY = "Min";
static const char* MAX_VALUE_KEY = "Max";

namespace KIPIHTMLExport {


struct IntThemeParameter::Private {
	int mMinValue;
	int mMaxValue;
};


IntThemeParameter::IntThemeParameter() {
	d = new Private;
}


IntThemeParameter::~IntThemeParameter() {
	delete d;
}


void IntThemeParameter::init(const QCString& internalName, const KConfigBase* configFile) {
	kdDebug() << "internalName:" << internalName << endl;
	AbstractThemeParameter::init(internalName, configFile);

	d->mMinValue = configFile->readNumEntry(MIN_VALUE_KEY, 0);
	d->mMaxValue = configFile->readNumEntry(MAX_VALUE_KEY, 99999);
}


QWidget* IntThemeParameter::createWidget(QWidget* parent, const QString& value) const {
	QSpinBox* spinBox = new QSpinBox(parent);
	spinBox->setValue(value.toInt());
	spinBox->setMinValue(d->mMinValue);
	spinBox->setMaxValue(d->mMaxValue);
	return spinBox;
}


QString IntThemeParameter::valueFromWidget(QWidget* widget) const {
	QSpinBox* spinBox = static_cast<QSpinBox*>(widget);
	return QString::number(spinBox->value());
}


} // namespace
